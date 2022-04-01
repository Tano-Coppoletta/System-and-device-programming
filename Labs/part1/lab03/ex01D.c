#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <aio.h>
#include <signal.h>

#define WAIT_TIME_1 1
#define WAIT_TIME_2 2
#define STR_NUM 4
#define STR_SIZE 17

void upper_string(char *str,int length);
int max(int n1,int n2);

typedef struct buffer{
	int length;
	char string[STR_SIZE];
}buffer;

int done=0;
struct buffer buf1;
struct buffer buf2;
int childN=0;

void manager(int sig){
	if(sig==SIGUSR1){
		printf("Received SIGUSR1, length read\n");
		//upper_string(buf1.string,buf1.length);
		//printf("child1 %d %s\n",buf1.length,buf1.string);
		//done++;
	}else if(sig==SIGUSR2){
		printf("Received SIGUSR2, string read\n");
		upper_string(buf1.string,buf1.length);
		printf("child%d %d %s\n",childN,buf1.length,buf1.string);
		done++;
	}
}


int main(int argc,char *argv[]){
	int fd1[2];
	int fd2[2];
	pid_t pid1,pid2;
	char *string;
	int n,j,lenght;

	signal(SIGUSR1,manager);
	signal(SIGUSR2,manager);
	
	
	if(pipe(fd1)!=0){
		fprintf(stderr,"Error in creating pipe1\n");
		exit(1);
	}
	pid1=fork();
	if(pid1==-1){
		fprintf(stderr,"Error in fork1\n");
		exit(1);
	}else if(pid1==0){
		//Child 1
		close(fd1[0]);
		unsigned int seed1=WAIT_TIME_1;
		for(int i=0;i<STR_NUM;i++){
			sleep(WAIT_TIME_1);
			//generate the lenght of the string
			lenght=(rand_r(&seed1) % STR_SIZE) + 1;
			//generate a random string
			string=malloc(lenght*sizeof(char));
			for(j=0;j<lenght-1;j++)
				string[j]=(rand_r(&seed1) %26)+ 'a';
			string[j]='\0';
			printf("p1 generates %s\n",string);
			//write lenght on the pipe
			if(write(fd1[1],&lenght,sizeof(int))<=0){
				printf("Erron in writing pipe1\n");
				exit(1);
			}
			if((n=write(fd1[1],string,lenght))<=0){
				printf("Erron in writing pipe1\n");
				exit(1);
			}
			free(string);
		}
		exit(1); //Child 1 exit at the end of execution
	}
	
	//Father reads from 2 pipes
	close(fd1[1]); //close unused end
	//generate another pipe and fork
	if(pipe(fd2)!=0){
		fprintf(stderr,"Error in creating pipe2\n");
		exit(1);
	}
	pid2=fork();
	if(pid2==-1){
		fprintf(stderr,"Error in fork1\n");
		exit(1);
	}else if(pid2==0){
		//Child 2
		close(fd2[0]);
		close(fd1[1]);
		close(fd1[0]);
		unsigned int seed2=WAIT_TIME_2;
		sleep(WAIT_TIME_2);
		
		for(int i=0;i<STR_NUM;i++){
			//generate the lenght of the string
			lenght=(rand_r(&seed2) % STR_SIZE) + 1;
			//generate a random string
			string=malloc(lenght*sizeof(char));
		
			for(j=0;j<lenght-1;j++)
				string[j]=(rand_r(&seed2) %26)+ 'a';
			string[j]='\0';
			fprintf(stdout,"p2 generates %s\n",string);	
			if(write(fd2[1],&lenght,sizeof(int))<=0){
				printf("Erron in writing pipe2\n");
				exit(1);
			}
			if((n=write(fd2[1],string,lenght))<=0){
				printf("Error writing in pipe2\n");
				exit(1);
			}
			free(string);
		}
		exit(1); //Child 2 exit at the end of its execution
	}


	//Father
	//close(fd2[1]);

    int num_read=STR_NUM,strl,l;
	
    
    //AIO
    struct aiocb aiostruct1;
    struct aiocb aiostruct2;

    aiostruct1.aio_fildes=fd1[0];
    aiostruct2.aio_fildes=fd2[0];

	struct sigevent sigev1;

	sigev1.sigev_signo=SIGUSR1;
	

	aiostruct1.aio_sigevent=sigev1;	
	
	childN=1;
	while(num_read>0){
		//read the number
		aiostruct1.aio_buf=&buf1.length;
		aiostruct1.aio_nbytes=sizeof(int);
		//when I read the length I want sigusr1
		sigev1.sigev_notify=SIGEV_SIGNAL;
		sigev1.sigev_signo=SIGUSR1;
		aiostruct1.aio_sigevent=sigev1;	
	

		if(aio_read(&aiostruct1)==-1){
			printf("ERROR IN AIO_READ length1\n");
		}
		pause();
		sigev1.sigev_notify=SIGEV_SIGNAL;
		sigev1.sigev_signo=SIGUSR2;
		aiostruct1.aio_sigevent=sigev1;	

		aiostruct1.aio_buf=&buf1.string;
		aiostruct1.aio_nbytes=buf1.length*sizeof(char);
		if(aio_read(&aiostruct1)==-1){
			printf("ERROR IN AIO_READ string1\n");
		}
		pause();
		num_read--;
	}
	num_read=STR_NUM;
	
	childN=2;
	aiostruct1.aio_fildes=fd2[0];

		while(num_read>0){
		//read the number
		aiostruct1.aio_buf=&buf1.length;
		aiostruct1.aio_nbytes=sizeof(int);
		//when I read the length I want sigusr1
		sigev1.sigev_notify=SIGEV_SIGNAL;
		sigev1.sigev_signo=SIGUSR1;
		aiostruct1.aio_sigevent=sigev1;	
	

		if(aio_read(&aiostruct1)==-1){
			printf("ERROR IN AIO_READ length2\n");
		}
		//Wait for the AIO to be completed
		pause();
		sigev1.sigev_notify=SIGEV_SIGNAL;
		//when I read the string I want to receive sigusr2
		sigev1.sigev_signo=SIGUSR2;
		aiostruct1.aio_sigevent=sigev1;	

		aiostruct1.aio_buf=&buf1.string;
		aiostruct1.aio_nbytes=buf1.length*sizeof(char);
		if(aio_read(&aiostruct1)==-1){
			printf("ERROR IN AIO_READ string2\n");
		}
		pause();
		num_read--;
	}

	fprintf(stdout,"strings written: %d\n",done); 

	wait(0);
	wait(0);
	close(fd1[0]);
	close(fd2[0]);
	return 0;
}


void upper_string(char *str,int length){
	int i;
	for(i=0;i<length;i++){
		str[i]=str[i] + ('A'-'a');
		//printf("%c",str[i]);
	}
	str[length-1]='\0';
	//printf("\n");
}
