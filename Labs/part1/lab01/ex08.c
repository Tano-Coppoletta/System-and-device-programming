#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct thread_s{
    int i;
    int j;
    int x
}thread_t;

int matA[3][3]={{1,0,0},{0,1,0},{0,0,1}};
int matB[3][2]={{1,2},{3,4},{5,6}};
int matC[3][2];

void mat_mul (int **A, int **B, int r, int x, int c, int **C);
void *tF(void *arg);

int main(int argc,char* argv[]){
    mat_mul(matA,matB,3,3,2,matC);

    int **mat=matA;

    for(int i=0;i<3;i++){
        for(int j=0;j<2;j++){
            fprintf(stdout,"%d ",matC[i][j]);
        }
        fprintf(stdout,"\n");
    }
}

void mat_mul (int **A, int **B, int r, int x, int c, int **C){
    int i,j;
    thread_t *tStruct=malloc(r*c*sizeof(thread_t));
    pthread_t *tid=malloc(r*c*sizeof(pthread_t));

    //I need r*c thread
    for(i=0;i<r;i++){
        for(j=0;j<c;j++){
            tStruct[i*c+j].i=i;
            tStruct[i*c+j].j=j;
            tStruct[i*c+j].x=x;
            pthread_create(&tid[i*c+j],NULL,tF,(void *) &tStruct[i*c+j]);
            
        }
    }

    for (i=0; i<r; i++) {
        for (j=0; j<c; j++) {
            pthread_join (tid[i*c+j], NULL);
        }
    }

}

void *tF(void *arg){
    thread_t *s=(thread_t *)arg;

    int k;
    matC[s->i][s->j]=0;
    for(k=0;k<s->x;k++){
        matC[s->i][s->j]=matC[s->i][s->j]+matA[s->i][k]*matB[k][s->j];
    }
    pthread_exit((void*)1);
}