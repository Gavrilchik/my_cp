#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

double func( double x) { 
	return (x*x)+7; }

int main(int argc, char **argv) {
	double a=atof(argv[1]);
	double b=atof(argv[2]);
	double dx=atof(argv[3]);
	int pn=atoi(argv[4]);
	
	double delta=(b-a)/pn;
	for(int i = 0; i<pn; ++i) {
		int pid =fork();
		if (pid==0) {
			int fd=open("temp",O_WRONLY|O_CREAT|O_APPEND, 0666);
			double result=0;
			for(double x=a+delta*i; x<a+delta*(i+1); x+=dx)
				result+=(func(x) + func(x+dx))/2*dx;
			write(fd, &result, sizeof (double));
			close(fd);
			exit(0);
		}
	}	
			double res, res_total=0;
			int fd=open("temp",O_RDONLY);
			for(int i=0; i<pn; ++i) {
				while( read(fd, &res, sizeof(res))!= sizeof(res)) sleep(1);
				res_total+=res;
			}
			close(fd);
			unlink("temp");
			printf("%lf", res_total);
	return 0;
}
