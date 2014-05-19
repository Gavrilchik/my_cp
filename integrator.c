#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

double func( double x) { 
	return (x*x)+7;
}

int main(int argc, char **argv) {
	if(argc!=5) {
		fprintf(stderr, "Error arguments\n");
		return 1;
	}
	double a=atof(argv[1]);
	double b=atof(argv[2]);
	double dx=atof(argv[3]);
	int pn=atoi(argv[4]);
	
	double delta=(b-a)/pn;
	int fd[2];
	if(pipe(fd)) {
		perror("Error creating pipe");
		return 1;
	}
	for(int i = 0; i<pn; ++i) {
		int pid =fork();
		if (pid==0) {
			close(fd[0]);
			double result=0;
			for(double x=a+delta*i; x<a+delta*(i+1); x+=dx)
				result+=(func(x) + func(x+dx))/2*dx;
			write(fd[1], &result, sizeof (double));
			close(fd[1]);
			return 0;
		}
	}
	close(fd[1]);
	double res, res_total=0;
	for(int i=0; i<pn; ++i) {
		read(fd[0], &res, sizeof(res));
		res_total+=res;
	}
	close(fd[0]);
	printf("%lf\n", res_total);
	return 0;
}
