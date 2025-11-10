#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/prctl.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

#define file_name "/var/log/sysmond.log"

int fd = -1;
char buff[50];

void write_to_log(const char *fmt, ...) {
	int len = 0;
	va_list args;

	if (fd == -1)
		return;

	va_start(args, fmt);
	len = vsnprintf(buff, sizeof(buff), fmt, args);
	va_end(args);
	write(fd, buff, len);
	fsync(fd);
}

void gen_sig_handler(int sig) {
	char sig_name[10] = "dummy";

	if (sig == SIGINT) {
		strcpy(sig_name, "SIGINT");
	} else if (sig == SIGHUP) {
		strcpy(sig_name, "SIGHUP");
	}

	write_to_log("%s(%d) receieved\n", sig_name, sig);
	exit(1);
}
int main() {
	pid_t pid, sid;
	int len = 0;

	fd = open(file_name, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fd == -1) {
		perror(file_name);
		return -EEXIST;
	}

	pid = fork();

	if (pid < 0){
		printf("Fork failed\n");
	} else if (pid == 0) {
		write_to_log("Child process will have pid as %d\n",getpid());
	
		sid = setsid();
		prctl(PR_SET_NAME, "child_session", 0, 0, 0);
		if (sid == -1) {
	    		perror("setsid failed");
        		exit(1);
		}
		signal(SIGINT, gen_sig_handler);
		signal(SIGHUP, gen_sig_handler);
		printf("handlers added\n");
		write_to_log("New session created. Session ID = %d\n", sid);
		while(1) {
			write_to_log("Current TS is %ld\n", time(NULL));
			sleep(5);
		}
	}

}
