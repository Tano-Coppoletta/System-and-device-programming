#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>

static void *thread_f (void *arg);
static void * my_malloc (int, int);
  
typedef struct thread_s {
  int n;
  int size;
  long int *v;
} thread_t;


int main(int argc,char *argv[]){
    thread_t thread;
    pthread_t tid;

    thread.n=atoi(argv[1]);
    thread.size=0;
    thread.v=NULL;

    pthread_create(&tid,NULL,thread_f,(void*)&thread);
    pthread_join(tid,NULL);

    return 1;
    
}

static void *thread_f (void *arg){
    thread_t *p,td1,td2;
    pthread_t tid1,tid2;

    p=(thread_t *) arg;

    if(p->n<=0){
        for(int i=0;i<p->size;i++){
            fprintf(stdout,"%ld ",p->v[i]);
        }
        fprintf(stdout,"\n");
        pthread_exit((void *) 1 );
    }

    td1.n=p->n-1;
    td1.size=p->size+1;
    td1.v=my_malloc(td1.size,sizeof(long int));

    for (int i=0; i<p->size; i++) {
        td1.v[i] = p->v[i];
    }

    td1.v[p->size]=pthread_self();
    pthread_create(&tid1,NULL,thread_f,(void *) &td1);

    td2.n=p->n-1;
    td2.size=p->size+1;
    td2.v=my_malloc(td2.size,sizeof(long int));

    for (int i=0; i<p->size; i++) {
        td2.v[i] = p->v[i];
    }

    td2.v[p->size]=pthread_self();
    pthread_create(&tid2,NULL,thread_f,(void *) &td2);

    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);


}

static void * my_malloc (int n, int size) {
  void *p;
  
  p = (void *)  malloc (n * size);
  if (p == NULL) {
    fprintf (stderr, "Allocation error.\n");
    exit (1);
  }

  return (p);
}
