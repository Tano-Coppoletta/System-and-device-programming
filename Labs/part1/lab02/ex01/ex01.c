#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define N 30

typedef struct student{
    int id;
    long int reg;
    char surname[N];
    char name[N];
    int mark;

}student_t;


int main(int argc,char* argv[]){
    student_t s;
    FILE *f1;
    int f2;
    FILE *f3;

    if(argc!=4){
        fprintf(stdout,"Wrong parameters!\n");
        exit(1);
    }   

    

    f1=fopen(argv[1],"r");
    if(f1==NULL){
        fprintf(stdout,"Error opening file1\n");
        exit(1);
    }

    f2=open(argv[2],O_WRONLY);
    if(f2<0){
        fprintf(stdout,"Error opening file2\n");
        exit(1);
    }

    while(fscanf(f1,"%d%ld%s%s%d",&s.id,&s.reg,s.surname,s.name,&s.mark)!=EOF){
        if(write(f2,(void*) &s,sizeof(s))<=0){
            fprintf(stdout,"Error in write\n");
        }
    }

    fclose(f1);
    close(f2);

    //open file 3 and write text from binary file file2

    f3=fopen(argv[3],"w");
    if(f3==NULL){
        fprintf(stdout,"Error opening file3\n");
        exit(1);
    }

    f2=open(argv[2],O_RDONLY);
    if(f2<0){
        fprintf(stdout,"Error opening file2\n");
        exit(1);
    }

    while(read(f2,(void *) &s,sizeof(s))>0){
        fprintf(f3,"%d %ld %s %s %d\n",s.id,s.reg,s.surname,s.name,s.mark);

    }


   

}