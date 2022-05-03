#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024

void P1(int *fd1,int* fd2,int shmid);
void P2(int *fd1,int* fd2,int shmid);
void upper_string(char *str,int length);

int main(int argc, char* argv[]){
    setbuf(stdout, 0);

    int fd1[2],fd2[2]; //for the pipe
    key_t key;
    int shmid;


    //generate the pipe
	if(pipe(fd1)!=0){
		fprintf(stderr,"Error in creating pipe1\n");
		exit(1);
	}
    if(pipe(fd2)!=0){
		fprintf(stderr,"Error in creating pipe2\n");
		exit(1);
	}

    //generate the key
    if((key=ftok("key.txt",1))==-1){
        perror("ftok");
        exit(1);
    }

    if((shmid=shmget(key,SHM_SIZE,0644 | IPC_CREAT))==-1){
        perror("shmget");
        exit(1);
    }


    if(fork()){
        //P1
        close(fd2[1]);
        close(fd1[0]);
        P1(fd1,fd2,shmid);
       

    }else{
        close(fd2[0]);
        close(fd1[1]);
        P2(fd1,fd2,shmid);

    }

    




}


void P1(int *fd1,int* fd2,int shmid){
    int i;
    char *string,*data;
    unsigned int seed=1;


    //attach to the shared memory
    data=shmat(shmid,NULL,0);
    if(data==(char*)(-1)){
        perror("shmat");
        exit(1);
    }

    while(1){

        int n=rand_r(&seed)%SHM_SIZE;
        //allocate string
        string=malloc(n*sizeof(char));

        if(string==NULL){
            fprintf(stderr,"Memory allocation problem\n");
            exit(1);
        }
        //if n==0 i write 0 and the child knows that he has to terminate
        if(n==0){
            if(write(fd1[1],&n,sizeof(int))!=sizeof(int)){
                fprintf(stderr,"Erron in write\n");
                exit(1);
            }
            fprintf(stdout,"P1 generates 0 : stopped\n");
            //detach memory
            if(shmdt(data)==-1){
                perror("shmdt");
                exit(1);
            }
            return;
        }

        //generates a random string of size n
        for(i=0;i<n-1;i++){
            string[i]=(rand_r(&seed) %28);
            if(string[i]==26){
                string[i]=' ';
            }else if(string[i]==27){
                string[i]='\n';
            }else{
                string[i]+='a';
            }
        }
        string[i]='\0';

        fprintf(stdout,"P1 generates: %s\n\n",string);


        //write the string
        strncpy(data,string,SHM_SIZE);


        //wake up P2 writing n in the pipe
        if(write(fd1[1],&n,sizeof(int))!=sizeof(int)){
            fprintf(stderr,"Erron in write\n");
            exit(1);
        }

        //wait that P2 writes
        if(read(fd2[0],&n,sizeof(int))<0){
            fprintf(stderr,"Error in read P1\n");
            exit(1);
        }

        if(n==0){
            fprintf(stdout,"P1 receives 0 : STOPPED\n\n");
            if(shmdt(data)==-1){
                perror("shmdt");
                exit(1);
            }            
            return;
        }

        fprintf(stdout,"P1 receives: %s\n\n",data);
        //toUpper
        upper_string(data,n);

        fprintf(stdout,"---P1 convert: %s\n\n",data);

        free(string);

    }
    if(shmdt(data)==-1){
        perror("shmdt");
        exit(1);
    }
}
void P2(int *fd1,int* fd2,int shmid){
    int n,i;
    char *string,*data;
    unsigned int seed=1;

    //write on the shared memory
    data=shmat(shmid,NULL,0);
    if(data==(char*)(-1)){
        perror("shmat");
        exit(1);
    }

    while(1){
        //P2
        if(read(fd1[0],&n,sizeof(int))<0){
            fprintf(stderr,"Error in read P2\n");
            exit(1);
        }
        //check if n==0
        if(n==0){
            fprintf(stdout,"P2 receives 0 : STOPPED\n");
            if(shmdt(data)==-1){
                perror("shmdt");
                exit(1);
            }            
            return;
        }


        fprintf(stdout,"P2 receives: %s\n\n",data);
        //toUpper
        upper_string(data,n);

        fprintf(stdout,"---P2 convert: %s\n\n",data);

        //generate string and write on shmem
        int n=rand_r(&seed)%SHM_SIZE;


        //if n==0 i write 0 and the child knows that he has to terminate
        if(n==0){
            if(write(fd2[1],&n,sizeof(int))!=sizeof(int)){
                fprintf(stderr,"Erron in write\n");
                exit(1);
            }
            fprintf(stdout,"P2 receives 0 : STOPPED\n");
            if(shmdt(data)==-1){
                perror("shmdt");
                exit(1);
            }           
            return;
        }

        //allocate string
        string=malloc(n*sizeof(char));

        //generates a random string of size n
        for(i=0;i<n-1;i++){
            string[i]=(rand_r(&seed) %28);
            if(string[i]==26){
                string[i]=' ';
            }else if(string[i]==27){
                string[i]='\n';
            }else{
                string[i]+='a';
            }
        }
        string[i]='\0';

        fprintf(stdout,"P2 generates: %s\n\n",string);

        strncpy(data,string,SHM_SIZE);

        //wake up P2 writing n in the pipe
        if(write(fd2[1],&n,sizeof(int))!=sizeof(int)){
            fprintf(stderr,"Erron in write\n");
            exit(1);
        }
        free(string);


    }
    if(shmdt(data)==-1){
        perror("shmdt");
        exit(1);
    }
}

void upper_string(char *str,int length){
	int i;
	for(i=0;i<length-1;i++){
        if(str[i]!=' ' && str[i]!='\n'){
		    str[i]=str[i] - 'a' + 'A';
		   // printf("%c",str[i]);
        }
	}
	str[length]='\0';
	//printf("\n");
}
