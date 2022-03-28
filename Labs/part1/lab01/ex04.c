#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc,char *argv[]){
	int h=atoi(argv[1]);
	int n=atoi(argv[2]);

	if(argc!=3){
		fprintf(stdout,"Wrong number of parameters!\n");
		return;
	}

	for(int i=0;i<h;i++){
		for(int j=0;j<n;j++){
			if(fork()){
				//father
				sleep(1);
			}else{
				fprintf(stdout, "pid= %d\n",getpid());
				if(j==n-1){
					fprintf(stdout,"EXIT pid= %d\n",getpid());
					exit(0);

				}
			}
		}
	}
	  printf ("RETURN pid=%d\n", getpid());
	  return(0);
}
