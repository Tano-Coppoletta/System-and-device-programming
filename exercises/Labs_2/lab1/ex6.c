#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int received=0, last=0, last_last=0,finish=0;

void signal_handler(int signo){
    if(signo==SIGUSR1){
        received=signo;
        if(received==last){
            if(received==last_last){
                //terminate
                printf("TERMINATING\n");
                finish=1;
            }else{
                //two consecutive SIGUSR1 show error
                printf("Two SIGUSR1\n");
            }
        }else{
            printf("SUCCESS: received SIGUSR1\n");
        }
        last_last=last;
        last=received;
    }else if(signo==SIGUSR2){
        received=signo;
        if(received==last){
            if(received==last_last){
                //terminate
                printf("TERMINATING\n");
                finish=1;
            }else{
                //two consecutive SIGUSR1 show error
                printf("Two SIGUSR2\n");
            }
        }else{
            printf("SUCCESS: received SIGUSR2\n");
        }
        last_last=last;
        last=received;
    }
}


int main(int argc, char *argv[]){
    signal(SIGUSR1,signal_handler);
    signal(SIGUSR2,signal_handler);

    while(!finish){
        pause();
    }
}