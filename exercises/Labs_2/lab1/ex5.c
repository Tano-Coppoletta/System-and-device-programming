#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    int n;
    int *vet;

    if(argc!=2){
        fprintf(stderr,"Wrong parameters\n");
        exit(1);
    }

    n=atoi(argv[1]);
    vet=(int*)malloc(n*sizeof(int));

    if(vet==NULL){
        fprintf(stderr,"Allocation error\n");
        exit(1);
    }

    int i;
    int j=0;

    for(i=0;i<=n;i++){
        if(i==n){
            for(j=0;j<n;j++)
                printf("%d", vet[j]);
            printf("\n");
            exit (0);
        }
        if(fork()){
            vet[i]=0;
        }else{
            vet[i]=1;
        }
    }

    free(vet);
}