#include <stdio.h>
#include <unistd.h>

const int nbyte=5000;

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("error: wrong count arg\n");
		return 1;
	}

	int fd[2];
	if (pipe(fd)<0) {
		perror("error: Pipe");
		return 1;
	}
	int pid = fork();
	if (pid == 0) {	
		close(fd[0]);
		
		if (dup2(fd[1],1) < 0) {
			perror("error: dup2");
			return 1;
		}

		if (execvp(argv[1], argv+1)<0) {
			perror("error: execvp");
			return 1;
		}
	} 
	else {
		if (pid < 0) {
			perror("error: fork");
			close(fd[0]);
			close(fd[1]);
			return 1;
		}
		close(fd[1]);

		char buff [nbyte];

		int num_str=0;
		int temp_n;
		while ((temp_n=read(fd[0], buff, nbyte))!=0) {
			write (1,buff,temp_n);
			for(int i=0; i<temp_n; i++)
				if(buff[i]=='\n')
					num_str+=1;
		}
		close(fd[0]);
		
		printf("\n------\n num_of_str: %d\n",num_str);
		return 0;
		}
}
