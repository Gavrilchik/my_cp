#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

const int NBYTE=100;

int open_check_file(char* file_name, int mask) {
	int fd=open(file_name, mask, 0600);
	if (fd<0) {
		perror("error_open_file_error");
		_exit(1);
	}
	return fd;
}

void copy_file(int fd1, int fd2) {
	void *buff=malloc(NBYTE);
	int posl_byte_read=0;
	while( (posl_byte_read=read(fd1, buff, NBYTE) )!=0) {	
		if (posl_byte_read<0) {
			perror("error_copy_error");
			_exit(1);
		}
		write(fd2, buff, posl_byte_read);
	}
	free(buff);
}

int main(int argc, char **argv) {
	if (argc!=3) {
		write(2,"error_wrong_number_argument_error\n", 50);
		_exit(1);
	}
	int fd1=open_check_file(argv[1], O_RDONLY);
	int fd2=open_check_file(argv[2], O_CREAT|O_TRUNC|O_WRONLY);
	copy_file(fd1, fd2);
	close(fd1);
	close(fd2);
	_exit(0);
}
