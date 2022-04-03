#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#define WAIT_TIME_1 1
#define WAIT_TIME_2 2
#define STR_NUM 4
#define STR_SIZE 17

void upper_string(char *str,int lenght);


int main(int argc,char *argv[]){
	int fd1[2];
	int fd2[2];
	pid_t pid1,pid2;
	char *string;
	char read_string[STR_SIZE];
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
			//printf("p1 generates %s\n",string);
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
			//fprintf(stdout,"p2 generates %s\n",string);	
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
	
	for(int i=0;i<STR_NUM;i++){
		//read lenght 
		read(fd1[0],&lenght,sizeof(int));
		read(fd1[0],read_string,lenght);
		upper_string(read_string,lenght);
		printf("i=%d child1 %s\n",i,read_string);
		
		read(fd2[0],&lenght,sizeof(int));
		read(fd2[0],read_string,lenght);
		upper_string(read_string,lenght);
		printf("i=%d, child2 %s\n",i,read_string);
	}
	
	
	return 0;
}


void upper_string(char *str,int lenght){
	for(int i=0;i<lenght;i++){
		str[i]=toupper(str[i]);
	}
}
