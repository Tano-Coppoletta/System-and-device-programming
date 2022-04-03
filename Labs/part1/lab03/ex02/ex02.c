#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

#define N 30

typedef struct student{
    int id;
    long int reg;
    char surname[N];
    char name[N];
    int mark;

}student_t;


typedef struct tS{
    student_t *addr;
    int id;
    int len;
}tS_t;


void *tF1(void *args);
void *tF2(void *args);

int main(int argc,char* argv[]){
    int fp,fd;
    void *start_addr,*dest_addr;
    tS_t tS[2];
    pthread_t tid1,tid2;


    if(argc!=3){
        fprintf(stderr,"Wrong parameters, usage %s file\n",argv[0]);
        exit(1);
    }
    //open file 1
    fp=open(argv[1],O_RDONLY);
    if(fp<0){
        fprintf(stderr,"Error in open file 1\n");
        exit(1);
    }
    //retrieve file size
    struct stat statbuf;
    int err = fstat(fp, &statbuf);
    if(err < 0){
        printf("\n\"%s \" could not open\n",
                       argv[1]);
        exit(2);
    }

    fd=open(argv[2],O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR,S_IXUSR);
    if(fd<0){
        fprintf(stderr,"Error in open file 1\n");
        exit(1);
    }


    if(ftruncate(fd,statbuf.st_size)<0){
        fprintf(stderr,"Error in truncate \n");
        exit(1);
    }
    //param:
    //- address where to map=0
    //- nbytes to map
    // protection of the mapped region
    //- flag 
    // -file
    // -offset
    if((start_addr=mmap(0,statbuf.st_size,PROT_READ,MAP_SHARED,fp,0))==MAP_FAILED){
        fprintf(stderr,"Error in map function\n");
        exit(1);
    }

    if((dest_addr=mmap(0,statbuf.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0))==MAP_FAILED){
        fprintf(stderr,"Error in map function\n");
        exit(1);
    }
    //the files are no more needed
    close(fp);
    close(fd);
    //copy the bytes in the dest 
    memcpy(dest_addr,start_addr,statbuf.st_size);  
   

    //student=start_addr;
    //printf("%d %ld %s %s %d\n",student->id,student->reg,student->surname,student->name,student->mark);

    //create thread that modifies the register number
    tS[0].addr=dest_addr;
    tS[0].id=1;
    tS[0].len=statbuf.st_size;
    if(pthread_create(&tid1,NULL,tF1,(void*)&tS[0])!=0){
        fprintf(stderr,"Error in pthread_create1 \n");
        exit(1);
    }

    tS[1].addr=dest_addr+statbuf.st_size-sizeof(student_t);
    tS[1].id=2;
    tS[1].len=statbuf.st_size;
    if(pthread_create(&tid2,NULL,tF2,(void*)&tS[1])!=0){
        fprintf(stderr,"Error in pthread_create2 \n");
        exit(1);
    }


    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);

    student_t *s=dest_addr;
    //print to see if it worked
    for(int i=0;i<statbuf.st_size/sizeof(student_t);i++){
        printf("%d %ld %s %s %d\n",s->id,s->reg,s->surname,s->name,s->mark);
        s++;
    }

    err=munmap(start_addr,statbuf.st_size);
    if(err!=0){
        fprintf(stderr,"Error in unmap file1\n");
        exit(1);
    }

    err=munmap(dest_addr,statbuf.st_size);
    if(err!=0){
        fprintf(stderr,"Error in unmap file2\n");
        exit(1);
    }
    return 0;
}

void *tF1(void *args){
    tS_t *ts=(tS_t*)args;
    student_t *s=ts->addr;
    //for each student => change reg number and print
    for(int i=0;i<(ts->len/sizeof(student_t));i++){
        s->reg++;
        //printf("%d %ld %s %s %d\n",s->id,s->reg,s->surname,s->name,s->mark);
        //next pointer 
        s++;

    }
    
    pthread_exit((void*)1);

}

void *tF2(void *args){
    tS_t *ts=(tS_t*)args;
    student_t *s=ts->addr;
  
    for(int i=0;i<(ts->len/sizeof(student_t));i++){
        s->mark--;
       // printf("%d %ld %s %s %d\n",s->id,s->reg,s->surname,s->name,s->mark);
        //next pointer
        s--;
    }

    pthread_exit((void*)1);

}