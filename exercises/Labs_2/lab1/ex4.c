#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    if(argc!=3){
        fprintf(stderr,"Wrong parameters \n");
        exit(1);
    }
    
    int n=atoi(argv[2]);
    int a=atoi(argv[1]);

    int i,j,pid;
    for (i=0; i<a; i++) {
        for (j=0; j<n; j++) {
        pid = fork ();
        if (pid==0) {
            sleep (1);
            break;
        } else {
            sleep (1);
            printf ("     pid=%d genera pid=%d\n", getpid(), pid);
            if (j==n-1) {
        printf ("  EXIT pid=%d\n", getpid());
            exit (0);
            }
        }
    }
  }

  printf ("RETURN pid=%d\n", getpid());

  return (0);


}