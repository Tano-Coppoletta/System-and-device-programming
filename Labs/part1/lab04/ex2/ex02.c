#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <pthread.h>
#include <fcntl.h>

#define DEBUG 1

typedef struct thread_s{
    char *file;
    int id;
    int *array;
    int length;
}thread_t;

void* tF(void* arg);
void sort(int *arr,int num_elements);
void merge(thread_t *tS,int n_threads,int fout);

int main(int argc,char *argv[]){
    thread_t *tS;
    int num_input_file;
    pthread_t *t;

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

    //create the threads
    for(int i=0;i<num_input_file;i++){
        tS[i].id=i;
        tS[i].file=argv[i+1];
        
        pthread_create(&t[i],NULL,tF,(void*)&tS[i]);


    }

    //wait all the threads
    for(int i=0;i<num_input_file;i++){
        pthread_join(t[i],NULL);
    }
    int fout;

    fout=open(argv[argc-1],O_WRONLY,O_CREAT,O_TRUNC);
    if(fout == -1){
        fprintf(stderr,"Error in opening fout\n");
        exit(1);
    }

    //merge
    merge(tS,num_input_file,fout);

    close(fout);

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

}


void* tF(void* arg){
    thread_t *tS;
    tS = (thread_t*) arg;
    int num_elements;

    int fp;
#if DEBUG
    fprintf(stdout,"file=%s\n",tS->file);
#endif
    fp=open(tS->file,O_RDONLY);
    if(fp==-1){
        fprintf(stderr,"Error in open %d\n",tS->id);
        exit(1);
    }

    if(read(fp,&num_elements,sizeof(int))>0){
#if DEBUG
        fprintf(stdout,"num element to read: %d\n ",num_elements);
#endif
        tS->length=num_elements;
        //allocate the array
        tS->array=malloc(num_elements*sizeof(int));
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
    fprintf(stdout,"thread: %d\n",tS->id);
    for(int i=0;i<num_elements;i++){
        fprintf(stdout,"%d ",tS->array[i]);
    }
    fprintf(stdout,"\n");
#endif

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

void merge(thread_t *tS,int n_threads,int fout){
    int *position;
    position=calloc(n_threads,sizeof(int));
    int num=1,tot_elem=0,min=TMP_MAX,min_position;
    //position stores the position of the last element inserted in the
    //big array for each thread
    //ex pos= 1 0 0 means that thread1 has used its first number so we don't have to insert that again
    if(position==NULL){
        fprintf(stderr,"Error in calloc\n");
        exit(1);
    }

    for(int i=0;i<n_threads;i++){
        tot_elem+=tS[i].length;
    }

#if DEBUG 
    printf("tot elem: %d\n",tot_elem);
#endif

    while(num!=tot_elem){
        //search the min
        for(int i=0;i<n_threads;i++){
            if(position[i]<tS[i].length){
                if(tS[i].array[position[i]]<min){
                    min=tS[i].array[position[i]];
                    min_position=i;
                }
            }
        }

#if DEBUG
    printf("min: %d min_position: %d\n",min,min_position);
#endif
        //write the min and increment position
        position[min_position]++;
     //   printf("position[%d]=%d\n",min_position,position[min_position]);
        if(write(fout,&min,sizeof(int))==-1){
            fprintf(stdout,"error in write\n");
            exit(1);
        }
        num++;
        min=TMP_MAX;

    }
    


}

