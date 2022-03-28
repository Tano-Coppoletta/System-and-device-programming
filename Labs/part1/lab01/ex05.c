#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
     int i;
     int n,*vett;

     if(argc!=2){
         fprintf(stdout,"Wrong parameters!\n");
         exit(1);
     }
     
     n=atoi(argv[1]);
     vett=(int*)malloc(n*sizeof(int));

     for(i=0;i<=n;i++){
        if (i == n) {
            for (int j=0; j<n; j++) {
                printf("%d", vett[j]);
            }
            printf("\n");
            exit (0);
        }else{
            if(fork()){                //parent
                vett[i]=0;
            }else{
                //child
                 vett[i]=1;
             }
         }
    }
}