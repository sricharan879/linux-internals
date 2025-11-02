#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

/* In this exercise we will learn how do we use system calls like
 * open, read, write to manipulate the files instead of using
 * the fops provied by C standard library.
 */

/* stdin	- fd 0
 * stdout	- fd 1
 * stderr	- fd 2
 */

/*
	read(fd, void *buf, size_t count)
	fd	-	file descriptor
	buf	-	Pointer a memoy buffer to store the data being read
	count	-	max bytes to be read

	write(fd, void *buf, size_t count)
	fd	-	file descriptor
	buf	-	pointer to the buffer that needs to be written to fd (This can be a string as well)
	count	-	size of the buffer

	open(filepath, flags, mode)
	filepath	-	path to the file to be opened
	flags		-	ACESS_FLAGS|FILE_CREATION_FLAGS|FILE_STATUS_FLAGS
	mod		-	file permissions
	+===============+===============+
	|		|O_RDONLY	|
	|access flags	|O_WRONLY	|
	|		|O_RDWR		|
	+---------------+---------------+
	|file		|O_CREAT	|
	|creation	|O_EXCL		|		
	|flags		|O_TRUNC	|
	+---------------+---------------+
	|file		|O_APPEND	|
	|status		|O_NONBLOCK	|
	|flags		|O_CLOEXEC	|
	+---------------+---------------+
*/	
int main() {
	int fd = creat("ex_fops.txt", 0644);

	if (fd == -1) {
		perror("failed to create file\n");
		return -1;
	}

	printf("file is created sucesfully fd is %d\n",fd);

	close(fd);
	
	if (errno) {
		perror("failed to close");
	}
	
	fd = open("ex_fops.txt",O_WRONLY,0644);
	write(fd, "Too bad bruh!!!\n",17);
	return 0;
}
