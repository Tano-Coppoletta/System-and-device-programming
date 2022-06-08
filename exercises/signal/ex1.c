#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int sig1=0,sig2=0,finish=0;

void signal_handler(int signo){
    if(signo==SIGUSR1){
        sig2=0;
        sig1++;
        if(sig1==3){
            //end
            finish=1;
        }
        printf("Signal received from P1\n");
    }else if(signo==SIGUSR2){
        sig1=0;
        sig2++;
        if(sig2==3){
            finish=1;
        }
        printf("Signal received from P2\n");

    }
}

void P1(){
    int wait_time;
    while(!finish){
        wait_time=rand()%3;
        sleep(wait_time);
        kill(getppid(),SIGUSR1);
    }
}

void P2(){
    int wait_time;
    while(!finish){
         wait_time=rand()%2;
         sleep(wait_time);
         kill(getppid(),SIGUSR2);
    }
}

int main(){
    signal(SIGUSR1,signal_handler);
    signal(SIGUSR2,signal_handler);

    int pid1,pid2;

    if((pid1=fork())){
        //father
        if((pid2=fork())){
            //father
        }else{
            //p2
            P2();
        }
    }else{
        //p1
        P1();
    }

    char str[100];
    while(1){
        pause();
        if(finish){
            //kill the process with a shell command
            sprintf(str,"kill -9 %d",pid1);
            system(str);
            sprintf(str,"kill -9 %d",pid2);
            system(str);
            exit(0);

        }
    }
}