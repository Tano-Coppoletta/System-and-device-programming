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
#include <sys/msg.h>


#define SHM_SIZE 1024

extern int errno;

void P1(int shmid,int msgq_id);
void P2(int shmid, int msgq_id);
void upper_string(char *str,int length);


typedef struct mymesg{
    long int mtype;
    int size;
}message;

int main(int argc, char* argv[]){
    setbuf(stdout, 0);

    key_t key;
    key_t key_queue;
    int shmid,msgq_id;




    //generate the key
    if((key=ftok("key.txt",1))==-1){
        perror("ftok");
        exit(1);
    }

    if((shmid=shmget(key,SHM_SIZE,0666 | IPC_CREAT))==-1){
        perror("shmget");
        exit(1);
    }

    //generate the key for the msg queue
    if((key_queue=ftok("key.txt",5))==-1){
        perror("ftok keyqueue");
        exit(1);
    }

    if((msgq_id=msgget(key_queue,0644 | IPC_CREAT))==-1){
        perror("msgq_id");
        exit(1);
    }

    if(fork()){
        //P1
        P1(shmid,msgq_id);
    }else{
        P2(shmid, msgq_id);
    }
  

}


void P1(int shmid, int msgq_id){
    int i;
    char *string,*data;
    unsigned int seed=1;
    message msg;


    //attach to the shared memory
    data=shmat(shmid,NULL,0);
    if(data==(char*)(-1)){
        perror("shmat");
        exit(1);
    }


//    msg2.mtype=1;

    while(1){
        msg.mtype=1;

        int n=rand_r(&seed)%SHM_SIZE;
        //write the size in the message struct
        msg.size=n;
        //allocate string
        string=malloc(n*sizeof(char));

        if(string==NULL){
            fprintf(stderr,"Memory allocation problem\n");
            exit(1);
        }
        //if n==0 i write 0 and the child knows that he has to terminate
        if(n==0){
            //send the message
            msgsnd(msgq_id,(void*)&msg,sizeof(message),0);
            fprintf(stdout,"P1 generates 0 : stopped\n");
            //detach memory
            if(shmdt(data)==-1){
                perror("shmdt");
                exit(1);
            }
            //remove the msg queue
            msgctl(msgq_id,IPC_RMID,NULL);
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

        //wake up P2 writing n in the msg
        msgsnd(msgq_id,(void*)&msg,sizeof(message),0);

        //msg1.mtype=2;
        //wait that P2 writes
        msgrcv(msgq_id,(void*)&msg,sizeof(message),2,0);


        if(msg.size==0){
            fprintf(stdout,"P1 receives 0 : STOPPED\n\n");
            if(shmdt(data)==-1){
                perror("shmdt");
                exit(1);
            }    
            msgctl(msgq_id,IPC_RMID,NULL);
           // msgctl(msgid2,IPC_RMID,NULL);                    
            return;
        }

        fprintf(stdout,"P1 receives: %s\n\n",data);
        //toUpper
        upper_string(data,msg.size);

        fprintf(stdout,"---P1 convert: %s\n\n",data);

        free(string);

    }
    if(shmdt(data)==-1){
        perror("shmdt");
        exit(1);
    }
    msgctl(msgq_id,IPC_RMID,NULL);   
}
void P2(int shmid,int msgq_id){
    int n,i;
    char *string,*data;
    unsigned int seed=1;

    message msg;

    //write on the shared memory
    data=shmat(shmid,NULL,0);
    if(data==(char*)(-1)){
        perror("shmat");
        exit(1);
    }


    while(1){
        //P2
        //receive the msg
        msgrcv(msgq_id,(void*)&msg,sizeof(message),1,0);
        //check if msg2.size==0
        if(msg.size==0){
            fprintf(stdout,"P2 receives 0 : STOPPED\n");
            if(shmdt(data)==-1){
                perror("shmdt");
                exit(1);
            }
            msgctl(msgq_id,IPC_RMID,NULL);             
            return;
        }


        fprintf(stdout,"P2 receives: %s\n\n",data);
        //toUpper
        upper_string(data,msg.size);

        fprintf(stdout,"---P2 convert: %s\n\n",data);

        //generate string and write on shmem
        int n=rand_r(&seed)%SHM_SIZE;

        //WRITE n in the msg
        msg.size=n;
        msg.mtype=2;
        //if n==0 i write 0 and the child knows that he has to terminate
        if(n==0){
            //send the message
            msgsnd(msgq_id,(void*)&msg,sizeof(message),0);
            fprintf(stdout,"P2 receives 0 : STOPPED\n");
            if(shmdt(data)==-1){
                perror("shmdt");
                exit(1);
            }
            msgctl(msgq_id,IPC_RMID,NULL);
           // msgctl(msgid2,IPC_RMID,NULL);           
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
        msgsnd(msgq_id,(void*)&msg,sizeof(message),0);
        free(string);


    }
    if(shmdt(data)==-1){
        perror("shmdt");
        exit(1);
    }
    msgctl(msgq_id,IPC_RMID,NULL);  
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
