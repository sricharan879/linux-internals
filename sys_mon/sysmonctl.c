#include "cmn.h"

int main(int argc, char *argv[]) {
	int fd = -1;
	struct sockaddr_un addr;
	char buf[4096];

	if (argc > 2) {
		printf("only 1 argument expected\n");
		exit(1);
	}
	
	fd = socket(AF_UNIX, SOCK_STREAM, 0);

	if (fd < 0) {
		perror("failed to open socket");
		exit(1);
	}
	
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("connect"); close(fd); exit(1);
	}
	write(fd, argv[1], strlen(argv[1])+1);

	if (read(fd, buf, sizeof(buf))) {
		printf("%s",buf);
	}

	sleep(1);
	close(fd);
	return 0;
}
