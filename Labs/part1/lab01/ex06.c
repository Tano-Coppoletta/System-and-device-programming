#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

//global variable that takes track of what signal I have received
int received1=0;
int received2=0;

void manager(int sig){
    if(sig==SIGUSR1){
        if(received1){
            //if I have already received sigusr1 I display an error msg
            fprintf(stdout,"SIGUSR1 already received!\n");
            received1++;
            if(received1==3){
                fprintf(stdout,"SIGUSR1 received 3 times, QUITTING!\n");
                exit(1);
            }
        }else{
            received1=1;
            fprintf(stdout,"Received SIGUSR1\n");
            if(received2){
                fprintf(stdout,"SUCCESS!\n");
                received1=0;
                received2=0;
            }
        }
    }
    if(sig==SIGUSR2){
        if(received2){
            //if I have already received sigusr2 I display an error msg
            fprintf(stdout,"SIGUSR2 already received!\n");
            received2++;
            if(received2==3){
                fprintf(stdout,"SIGUSR2 received 3 times, QUITTING!\n");
                exit(1);
            }
        }else{
            received2=1;
            fprintf(stdout,"Received SIGUSR2\n");
            if(received1){
                fprintf(stdout,"SUCCESS!\n");
                received1=0;
                received2=0;
            }
        }
    }
}

int main(int argc,char *argv[]){


   if (signal(SIGUSR1, manager) == SIG_ERR) {
        fprintf (stderr, "Signal Handler Error.\n");
        return (1);
    }
    if (signal(SIGUSR2, manager) == SIG_ERR) {
        fprintf (stderr, "Signal Handler Error.\n");
        return (1);
    }

    while (1) {
        pause ();
    }

    return (0);
}