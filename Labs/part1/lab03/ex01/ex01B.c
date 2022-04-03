#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>


#define WAIT_TIME_1 1
#define WAIT_TIME_2 2
#define STR_NUM 4
#define STR_SIZE 17

void upper_string(char *str,int lenght);

void set_fnctl(int fd, int flags);

int main(int argc,char *argv[]){
	int fd1[2];
	int fd2[2];
	pid_t pid1,pid2;
	char *string;
	char read_string1[STR_SIZE];
	char read_string2[STR_SIZE];
	int n,j,lenght,num_read=STR_NUM*2;
	
	
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
			for(j=0;j<lenght;j++)
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
			fprintf(stdout,"c1: %d %s\n",lenght,string);
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
		
			for(j=0;j<=lenght-1;j++)
				string[j]=(rand_r(&seed2) %26)+ 'a';
			string[j+1]='\0';
			//fprintf(stdout,"p2 generates %s\n",string);	
			if(write(fd2[1],&lenght,sizeof(int))<=0){
				printf("Erron in writing pipe2\n");
				exit(1);
			}
			if((n=write(fd2[1],string,lenght))<=0){
				printf("Error writing in pipe2\n");
				exit(1);
			}
			fprintf(stdout,"c2: %d %s\n",lenght,string);
			free(string);
		}
		exit(1); //Child 2 exit at the end of its execution
	}
	//Father
	close(fd2[1]);
	//use non-blocking read
	set_fnctl(fd1[0],O_NONBLOCK);
	set_fnctl(fd2[0],O_NONBLOCK);
	int n1,n2,strl1,strl2,l1,l2,done=0;
	while(num_read>0){
		n1=read(fd1[0],&l1,sizeof(int));
		if(n1>0){
			sleep(3);
			strl1=read(fd1[0],read_string1,l1);
			if(strl1>0){
				upper_string(read_string1,l1);
				printf("child1 %s\n",read_string1);
				num_read--;
				done++;
			}
		}
		
		n2=read(fd2[0],&l2,sizeof(int));
		if(n2>0){
			sleep(3);
			strl2=read(fd2[0],read_string2,l2);
			if(strl2>0){
				upper_string(read_string2,l2);
				printf("child2 %s\n",read_string2);
				num_read--;
				done++;
			}
		}
	}
	
	fprintf(stdout,"strings written: %d\n",done);
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
		printf("%c",str[i]);
	}
	str[lenght-1]='\0';
	printf("\n");
}

void set_fnctl(int fd, int flags){
	int val;
	if((val=fcntl(fd,F_GETFL,0))<0){
		//ERROR
		fprintf(stderr,"Error in fcntl\n");
		exit(1);
	}
	val |= flags;
	if(fcntl(fd,F_SETFL,val)<0){
		fprintf(stderr,"Error setting the flags\n");
		exit(1);
	}
}
