#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>

#define WAIT_TIME_1 2
#define WAIT_TIME_2 3
#define STR_SIZE 10
#define STR_NUM 3

void set_fcntl(int fd,int flags);
void upper_string(char *string,int n);

int main(int argc, char* argv[]){
    pid_t c1,c2;
    int p1[2],p2[2];
    //pipe for p and c1
    if(pipe(p1)!=0){
        fprintf(stderr,"Error in creating pipe\n");
        exit(1);
    }

    if((c1=fork())>0){
        //father P
        //pipe for p and c2
        if(pipe(p2)!=0){
            fprintf(stderr,"Error in creating pipe\n");
            exit(1);
        }
        //close the extreme for writing
        close(p1[1]);
        if((c2=fork())>0){
            //father P
            //close the extreme for writing
            close(p2[1]);

            //the father wants to do non-blocking read
            set_fcntl(p1[0],O_NONBLOCK);
            set_fcntl(p2[0],O_NONBLOCK);


        }else{
            //childe c2
            //close the extreme for reading
            close(p2[0]);
        }
    }else{
        //Child c1
        //close the extreme for reading
        close(p1[0]);

    }
}


void set_fcntl(int fd,int flags){
    int val;
    if((val=fcntl(fd,F_GETFL,0)) <0){
        fprintf(stderr,"ERROR IN F_GETFL\n");
        exit(1);
    }
    val |= flags;
    if(fcntl(fd,F_SETFL,val)<0){
        fprintf(stderr,"ERROR IN F_SETFL\n");
        exit(1);
    }
}

void upper_string(char *string,int n){
    int i;
    for(i=0;i<n;i++){
        string[i]+=('A'-'a');
    }
    string[n]='\n';
}