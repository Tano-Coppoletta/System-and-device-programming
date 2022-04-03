#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/select.h>

#define WAIT_TIME_1 1
#define WAIT_TIME_2 2
#define STR_NUM 4
#define STR_SIZE 17

void upper_string(char *str,int lenght);
int max(int n1,int n2);


int main(int argc,char *argv[]){
	int fd1[2];
	int fd2[2];
	pid_t pid1,pid2;
	char *string;
	int n,j,lenght;
	
	
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
	close(fd2[1]);
	fd_set readfds;
	
	FD_ZERO(&readfds);
	FD_SET(fd1[0],&readfds);
	FD_SET(fd2[0],&readfds);
	struct timeval tvptr;
	
	tvptr.tv_sec=2;
	tvptr.tv_usec=2000;
	int ret_select,num_read=STR_NUM*2,done=0,strl,l;
	int to_read1=STR_NUM,to_read2=STR_NUM;
	char read_string1[STR_SIZE];
	char read_string2[STR_SIZE];

	while(num_read>0){
		//read lenght 
		if((ret_select=select(max(fd1[0],fd2[0])+1,&readfds,NULL,NULL,&tvptr))==-1){
			//error
			fprintf(stderr,"Error in select\n");
			exit(1);
		}else if(ret_select==0){
			//no one is ready
			//set all the flag again
			FD_SET(fd1[0],&readfds);
			FD_SET(fd2[0],&readfds);
		}else{
			
			//someone is ready
			if(FD_ISSET(fd1[0],&readfds) && to_read1>0){
				//fd1 is ready, read
				n=read(fd1[0],&l,sizeof(int));
				if(n>0){
					sleep(1);
					strl=read(fd1[0],read_string1,l);
					if(strl>0){
						upper_string(read_string1,l);
						printf("child1 %s\n",read_string1);
						num_read--;
						done++;
						to_read1--;
					}
				}
				//set again the flag
				FD_SET(fd1[0],&readfds);
				FD_SET(fd2[0],&readfds);

			}else if(FD_ISSET(fd2[0],&readfds) && to_read2>0){
				n=read(fd2[0],&l,sizeof(int));
				if(n>0){
					sleep(1);
					strl=read(fd2[0],read_string2,l);
					if(strl>0){
						upper_string(read_string2,l);
						printf("child2 %s\n",read_string2);
						num_read--;
						done++;
						to_read2--;
					}
				}
				//set again the flag
				FD_SET(fd1[0],&readfds);
				FD_SET(fd2[0],&readfds);
			}
		}

	}
	fprintf(stdout,"strings written: %d\n",done);
	//clear all the flags
	FD_ZERO(&readfds);
	wait(0);
	wait(0);
	close(fd1[0]);
	close(fd2[0]);
	return 0;
}


void upper_string(char *str,int lenght){
	int i;
	for(i=0;i<lenght;i++){
		str[i]=str[i] + ('A'-'a');
		//printf("%c",str[i]);
	}
	str[lenght-1]='\0';
	//printf("\n");
}

int max(int n1,int n2){
	if(n1>n2)
		return n1;
	return n2;
}
