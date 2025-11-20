#include "cmn.h"

int cpu_info_fd = -1, netdev_info_fd = -1;
char buff[1024];

struct sys_stats {
	float cpu_usage;
	long mem_total;
	long mem_free;
	long rx_bytes;
	long tx_bytes;
} current_stats;

void write_to_log(int local_fd,const char *fmt, ...) {
	int len = 0;
	va_list args;

	if (local_fd == -1)
		return;

	va_start(args, fmt);
	len = vsnprintf(buff, sizeof(buff), fmt, args);
	va_end(args);
	write(local_fd, buff, len);
}

void gen_sig_handler(int sig) {
	char sig_name[10] = "dummy";

	if (sig == SIGINT) {
		strcpy(sig_name, "SIGINT");
	} else if (sig == SIGHUP) {
		strcpy(sig_name, "SIGHUP");
	}

	unlink(sock_path);
	exit(1);
}

void update_netdev_info(int fd) {
	
	int netdev_info_fd = open(netdev_info, O_RDONLY);
	char *line, log_buff[4096];
	int skip = 0;
	char iface_name[32];
	long tx_bytes, rx_bytes;

	if (netdev_info_fd == -1)
		perror(netdev_info);

	read(netdev_info_fd, log_buff, sizeof(log_buff));
	close(netdev_info_fd);

	current_stats.rx_bytes = 0;
	current_stats.tx_bytes = 0;

	line = strtok(log_buff, "\n");
	while(line) {
		if (skip >= 2) {
			sscanf(line, " %31[^:]: %lu %*s %*s %*s %*s %*s %*s %*s %lu",iface_name, &rx_bytes, &tx_bytes);
			write_to_log(fd, "Interface Name : %s Rx bytes : %lu Tx bytes : %lu\n",iface_name, rx_bytes, tx_bytes);
			current_stats.rx_bytes += rx_bytes;
			current_stats.tx_bytes += tx_bytes;
		}
		skip++;
		line = strtok(NULL, "\n");
	}
	write_to_log(fd, "Total Rx bytes : %lu Total Tx bytes : %lu\n",current_stats.rx_bytes, current_stats.tx_bytes);

}

void update_mem_info(int fd) {
	int mem_info_fd = open(mem_info, O_RDONLY);
	char *m_t = "MemTotal:", *m_a = "MemAvailable:";
	char *line, log_buff[4096];

	if (mem_info_fd == -1)
		perror(mem_info);

	read(mem_info_fd, log_buff, sizeof(log_buff));
	close(mem_info_fd);

	line = strtok(log_buff, "\n");
	while(line) {
		if (strncmp(line, m_t, strlen(m_t)) == 0)
			sscanf(line+strlen(m_t), "%lu", &current_stats.mem_total);
		else if(strncmp(line, m_a, strlen(m_a)) == 0)
			sscanf(line+strlen(m_a), "%lu", &current_stats.mem_free);
		line = strtok(NULL, "\n");
	}
	
	write_to_log(fd, "Mem total is %lu Mem free is %lu\n",
		      current_stats.mem_total, current_stats.mem_free);
}

void handle_req(int conn_fd, char* buff) {
	
	if (!strcmp(buff, net)) {
		update_netdev_info(conn_fd);
	} else if (!strcmp(buff, mem)) {
		update_mem_info(conn_fd);
	} else if (!strcmp(buff, all)) {
		update_netdev_info(conn_fd);
		update_mem_info(conn_fd);
	} else if (!strcmp(buff, stop)) {
		write_to_log(conn_fd, "Killing PID %lu\n",getpid());
		exit(1);
	} else {
		write_to_log(conn_fd, "Invalid request\n");
	}
	fsync(conn_fd);
}

int create_bind_sock() {
	int ret = 0;
	int listen_fd = -1, conn_fd = -1;
	struct sockaddr_un addr;
	char buff[48];

	listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);

	if (listen_fd < 0) {
		perror("failed to open socket");
		return -1;
	}
	
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path)-1);

	unlink(sock_path);

	if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("failed to bind");
		close(listen_fd);
		return -2;
	}

	if (listen(listen_fd, 5) < 0) {
		perror("listen");
		close(listen_fd);
		unlink(sock_path);
		return -3;
	}
	
	while(1) {
		conn_fd = accept(listen_fd, NULL, NULL);

		if (conn_fd < 0) {
			perror("accept");
			break;
		}

		read(conn_fd, buff, sizeof(buff));
		handle_req(conn_fd, buff);
	}

	close(listen_fd);
    	unlink(sock_path); // cleanup
	return 0;
}

int main() {
	pid_t pid, sid;
	int len = 0, fd = -1;
	struct sockaddr_un sock;
	int ret;

	fd = open(file_name, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fd == -1) {
		perror(file_name);
		return -EEXIST;
	}

	pid = fork();

	if (pid < 0){
		printf("Fork failed\n");
	} else if (pid == 0) {
		write_to_log(fd, "Child process will have pid as %d\n",getpid());
	
		sid = setsid();
		prctl(PR_SET_NAME, "child_session", 0, 0, 0);
		if (sid == -1) {
	    		perror("setsid failed");
        		exit(1);
		}
		signal(SIGINT, gen_sig_handler);
		signal(SIGHUP, gen_sig_handler);
		printf("handlers added\n");
		ret = create_bind_sock();
		if (ret) {
			printf("Failed to create socket\n");
			exit(1);
		}
		write_to_log(fd, "New session created. Session ID = %d\n", sid);
	}

}

