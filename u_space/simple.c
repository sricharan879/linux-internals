#include <stdio.h>
#include <errno.h>
#include <unistd.h>

/*In this one we see about system calls like read/write and
 * what is errno how is it used.
 * Once a system call is made the corresponding operation
 * will performed and the errno is a value updated by the
 * kernel indicating the status of the system call. 0
 * indicates success and anyother will be failure.
 * Now when we invoke perror kernel will print the appropriate
 * reason for the errno value.
 */
int main() {
	int i;
	char msg[] = "Enter a number\n";
	// write(fd table entry, msg, size in characters to be printed)

	write(2, msg, sizeof(msg));
	scanf("%d",&i);

	/* As we are using a scanf here the system call is only responsible
	 * to read something and if it is sucessful errno will be 0. So even
	 * if u invoke perror it will show sucess.
	 */

	if (i < 100)
		printf("The number u entered is %d\n",i);
	else
		perror("invalid value");
}
