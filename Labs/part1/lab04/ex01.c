#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>



pthread_barrier_t bar_read;
pthread_barrier_t bar_write;

typedef struct thread_s{
    int id;
    int n; //argv[1]
    int n_elements; //2^n elements in the array
    int *v; //array

    int sum;

}thread_t;

void *tF(void* arg);

int n_thread;

sem_t mutex; //mutex to access n_thread

int main(int argc,char *argv[]){
    int n,n_elements;
    int *v;
    //int v[]={2,4,6,1,3,5,8,7};
    pthread_t *tid;
    thread_t *ts;

    if(argc!=2){
        fprintf(stderr,"Wrong parameters, usage: ex01 n\n");
        exit(1);
    }

    n=atoi(argv[1]);
    printf("n=%d\n",n);
    n_elements=(1 << n);
    n_thread=n_elements-1;
    printf("n_elements=%d\n",n_elements);

    //allocation of v and tid  and ts
    v=malloc(n_elements*sizeof(int));
    tid=malloc((n_elements-1)*sizeof(pthread_t));
    ts=malloc((n_elements-1)*sizeof(thread_t));
    if(v==NULL || tid==NULL){
        fprintf(stderr,"Error in malloc v\n");
        exit(1);
    }

    //initialize mutex
    sem_init(&mutex,0,1);
    //generate elements of the array
    for(int i=0;i<n_elements;i++){
        v[i]= rand() % 9 + 1; //from 1 to 9
        fprintf(stdout,"%d ",v[i]);
    }
    fprintf(stdout,"\n");

/*
    for(int i=0;i<n_elements;i++){
        fprintf(stdout,"%d ",v[i]);
    }
    fprintf(stdout,"\n");*/
    //init barrier
    pthread_barrier_init(&bar_read,NULL,n_elements-1);
    pthread_barrier_init(&bar_write,NULL,n_elements-1);
    
    for(int i=0;i<n_elements-1;i++){
        ts[i].id=i+1;
        ts[i].n=n;
        ts[i].n_elements=n_elements;
        ts[i].v=v;
        pthread_create(&tid[i],NULL,tF,&ts[i]);
    }

    for(int i=0;i<n_elements-1;i++){
        pthread_join(tid[i],NULL);
    }

    //destroy barrier
    pthread_barrier_destroy(&bar_read);
    pthread_barrier_destroy(&bar_write);

   
    for(int i=0;i<n_elements;i++){
        fprintf(stdout,"%d ",v[i]);
    }
    fprintf(stdout,"\n");

    //free
   // free(v);

    return 0;
}

void *tF(void* arg){
    thread_t *ts=(thread_t*)arg;
    int gap=1;

    for(int i=0;i<ts->n;i++){
        gap=1<<i;
        //read and store the sum computed by the thread
        
        ts->sum=ts->v[ts->id]+ts->v[ts->id-gap];
        
        fprintf(stdout,"T: %d sum: %d\n",ts->id,ts->sum);
        
        if(ts->id<(gap+1)){
            //reinitialize the barrier and exit
            sem_wait(&mutex);
            //update n_thread
             n_thread--;
            
            pthread_barrier_destroy(&bar_write);
            
            pthread_barrier_init(&bar_write,NULL,n_thread);

            sem_post(&mutex);
              
        }
         //wait on the barrier of the reader
        pthread_barrier_wait(&bar_read);

        //write the sum in the array

        ts->v[ts->id]=ts->sum;

        if(ts->id<(gap+1)){
            //reinitialize the barrier and exit
            fprintf(stdout,"Killing t: %d\n",ts->id);
            sem_wait(&mutex);
           
            
            fprintf(stdout,"n_thread: %d\n",n_thread);

            pthread_barrier_destroy(&bar_read);

            pthread_barrier_init(&bar_read,NULL,n_thread);

            sem_post(&mutex);
            
            pthread_exit(NULL);
        }
        //wait on the write barrier
        pthread_barrier_wait(&bar_write);
   
    }
}

