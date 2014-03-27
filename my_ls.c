#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>  
#include <stdio.h>
#include <time.h>

#define MAX_DEPTH 2

int r=0, l=0;

void print_mode(int mode)
{
	char str[11]="----------\0";
	if (S_ISDIR(mode)) str[0]='d';
	if (mode & S_IRUSR) str[1]='r';
	if (mode & S_IWUSR) str[2]='w';
	if (mode & S_IXUSR) str[3]='x';
	if (mode & S_IRGRP) str[4]='r';
	if (mode & S_IWGRP) str[5]='w';
	if (mode & S_IXGRP) str[6]='x';
	if (mode & S_IROTH) str[7]='r';
	if (mode & S_IWOTH) str[8]='w';
	if (mode & S_IXOTH) str[9]='x';
	printf("%s ", str);
}

void print(struct dirent *dir, struct stat *buf, int depth)
{
	if (dir->d_name[0]=='.') return;
	for(int i=0; i<depth*4; ++i) printf(" ");
	if(l==1)
	{
		print_mode(buf->st_mode);
		printf("%d ", (int)buf->st_nlink);
		printf("%.12s ", 4+ctime(&(buf->st_mtime)));
		printf("%ld ", (long)buf->st_size); 
	}
	printf("%s\n", dir->d_name);
	return;
}

//0-bad, else 1
int ls( char *dirname, int depth)
{	
	struct stat buf;
	if ( chdir(dirname)<0 ) {
		perror(dirname);
		return 0;
	}
	 
	DIR *dir=opendir("./");
	
	if(dir==NULL)
	{
		perror(dirname);
		return 0;
	}
	
	struct dirent *curr_dir;
	
    while ( (curr_dir = readdir(dir)) != NULL) 
    {
		if (stat(curr_dir->d_name, &buf) < 0)
			perror(curr_dir->d_name);
		print(curr_dir, &buf, depth);
		if (r==1 && S_ISDIR(buf.st_mode) && depth<MAX_DEPTH && curr_dir->d_name[0]!='.')
			if (ls(curr_dir->d_name, depth+1)==1)
				if (chdir("./../")< 0){
					perror("fatal_error");
					_exit(1);
				}
    }
	closedir(dir);
	return 1;	
}

int main(int argc, char **argv)
{
	for (int i=1; i<argc; ++i)
	{
		if(strcmp(argv[i],"-l")==0) l=1;
		else if(strcmp(argv[i],"-r")==0) r=1;
		else if (chdir(argv[i])<0){
			perror(argv[i]);
			_exit(1);
		}
	}
	ls("./", 0);
	_exit(0);
}
