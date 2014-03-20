#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

double f( double x) { 
	return (x*x)+7; }

double simple_integrate(double a, double b, double dx, double (*f)(double) ) {
	double result=0;
	for(double x=a; x<b; x+=dx)
		result+=(f(x) + f(x+dx))/2*dx;
	return result;
}

void mp_integrate(double a, double b, double dx, double (*f)(double) , int pn) {
	double delta=(b-a)/pn;
	for(int i = 0; i<pn; ++i) {
		int pid =fork();
		if (pid==0) {
			int fd=open("temp",O_WRONLY|O_CREAT|O_APPEND, 0666);
			double res = simple_integrate(a+delta*i, a+delta*(i+1), dx, f);
			write(fd, &res, sizeof(res));
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
}

int main() {	
	mp_integrate(0,10, 0.00000001, f, 10);
	return 0;
}
