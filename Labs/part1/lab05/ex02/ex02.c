#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <pthread.h>
#include <fcntl.h>
#include "semaphore.h"
#include <sys/types.h>
#include <sys/stat.h>

#define DEBUG 0

typedef struct thread_s{
    char *file;
    int id;
    int *array;
    int length;
}thread_t;

int finish=0; //used to tell who has finished
pthread_mutex_t mxtSorter;
pthread_mutex_t mtxMerger;

void* tF(void* arg);
void sort(int *arr,int num_elements);
int merge(thread_t tS,int *oldVect,int length,int *newVect);

int main(int argc, char* argv[]){
    setbuf(stdout,0);

    thread_t *tS;
    int num_input_file,*total,*toMerge,totalN,toMergeN;
    pthread_t *t;
    totalN=0;
    toMerge=NULL;
    toMergeN=0;


    if(argc<3){
        fprintf(stderr,"Invalid argument use prgm file1 file2 ...\n");
        exit(1);
    }

    //num_input_file is equal to argc - name of the program - last file
    num_input_file=argc-2;

    //allocation
    tS=malloc(num_input_file*sizeof(thread_t));
    t=malloc(num_input_file*sizeof(pthread_t));

    if(tS==NULL || t==NULL){
        fprintf(stderr,"error in allocation\n");
        exit(1);
    }

    //initialize the mutex
    if((pthread_mutex_init(&mtxMerger,NULL))!=0){
        printf("Error in mutex init\n");
        exit(1);
    }
    if((pthread_mutex_init(&mxtSorter,NULL))!=0){
        printf("Error in mutex init\n");
        exit(1);
    }

    //create the threads
    for(int i=0;i<num_input_file;i++){
        tS[i].id=i+1;
        tS[i].file=argv[i+1];
        
        pthread_create(&t[i],NULL,tF,(void*)&tS[i]);
        


    }
    total=malloc(1*sizeof(int));

    int j;
    pthread_mutex_lock(&mtxMerger);
    for(int i=0;i<num_input_file;i++){
        pthread_mutex_lock(&mtxMerger);
        j = finish;
#if DEBUG
        printf("finish : %d\n",j);
#endif
        pthread_mutex_unlock(&mxtSorter);
        
        toMerge=malloc(tS[j].length*sizeof(int));
        total=realloc(total,(tS[j].length+totalN)*sizeof(int));
        
/*
        printf("realloc: ");
        for(int i=0;i<totalN+tS[j].length;i++)
            printf("%d ",total[i]);
        printf("\n");*/

        if(toMerge==NULL || total==NULL){
            printf("Error in malloc\n");
            exit(1);
        }
        
        toMerge=tS[j].array;

        totalN=merge(tS[j],total,totalN,toMerge);

        free(toMerge);
       // free(tS[j].array);

#if DEBUG
        printf("merge : %d total N: %d\n",j,totalN);
        /*for(int z=0;z<newLength;z++)
            printf("%d ",newVect[z]);*/
        printf("\n");
#endif
        /*
        for(int z=0;z<newLength;z++)
            printf("%d ",newVect[z]);*/
        printf("\n");
        
    
        //free(tS[j].array);
        //free(tS[j].file);

    }


    int fout;

    fout=open(argv[argc-1],O_WRONLY,O_CREAT,O_TRUNC);
    if(fout == -1){
        fprintf(stderr,"Error in opening fout\n");
        exit(1);
    }

    int i=0;
    while(i<totalN){
       // printf("writing: %d",total[i]);
        write(fout,&total[i],sizeof(int));
        i++;
    }

   
    fout=open(argv[argc-1],O_RDONLY);
    if(fout == -1){
        fprintf(stderr,"Error in opening fout\n");
        exit(1);
    }
    
    //print
    int num;
    while((read(fout,&num,sizeof(int)))>0){
        fprintf(stdout,"%d ",num);
    }
    printf("\n");
    
    free(tS);
    free(t);

}



void* tF(void* arg){
    thread_t *tS;
    tS = (thread_t*) arg;
    int num_elements;

    int fp;

    pthread_detach(pthread_self());

#if DEBUG
    //printf("Executing t%d\n",tS->id);
   // fprintf(stdout,"file=%s\n",tS->file);
#endif
    fp=open(tS->file,O_RDONLY);
    if(fp==-1){
        fprintf(stderr,"Error in open %d\n",tS->id);
        exit(1);
    }

    if(read(fp,&num_elements,sizeof(int))>0){
#if DEBUG
        fprintf(stdout,"t : %d num element to read: %d\n ",tS->id,num_elements);
#endif
        tS->length=num_elements;
        //allocate the array
        tS->array=(int*)malloc(num_elements*sizeof(int));
        if(tS->array==NULL){
            fprintf(stderr,"error in allocation t %d\n",tS->id);
            exit(1);
        }
    }
    //read the number and store them in the array
    int i=0,val;
    while((read(fp,&val,sizeof(int)))>0){
        tS->array[i]=val;
        i++;
    }
    
    //order the number
    sort(tS->array,num_elements);

#if DEBUG
   /* fprintf(stdout,"thread: %d\n",tS->id);
    for(int i=0;i<num_elements;i++){
        fprintf(stdout,"%d ",tS->array[i]);
    }
    fprintf(stdout,"\n");*/
#endif

    close(fp);
    //lock the sorter to write the variable finish
    pthread_mutex_lock(&mxtSorter);
    finish=tS->id -1;
    //unlock the merger
    pthread_mutex_unlock(&mtxMerger);

    pthread_exit(NULL);
}


void sort(int *arr,int num_elements){
    for (int i = 0; i < num_elements - 1; i++){

        for (int j = 0; j < num_elements - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}


int merge(thread_t tS,int *total,int length,int *toMerge){
   
   
    int newSize= length + tS.length;
    int n1=0,n2=0,*v;

    v=(int*)malloc(newSize*sizeof(int));
    if(v==NULL){
        printf("allocation error\n");
        exit(1);
    }


    int i=0;
    while(n1<length && n2<tS.length){
        if(total[n1]<toMerge[n2]){
            v[i++]=total[n1++];
            
        }else{
            v[i++]=toMerge[n2++];
        }
       
    }

    while(n1<length){
        v[i++]=total[n1++];
    }
    while(n2<tS.length)
        v[i++]=toMerge[n2++];

    
    printf("V: ");
    for(int i=0;i<newSize;i++){
        total[i]=v[i];
        printf("%d ",v[i]);
    }
   
    printf("\nmerge result: ");
    for(int i =0; i<newSize;i++){
        printf("%d ",total[i]);
    }
    printf("\n");
   

    return newSize;

    
}