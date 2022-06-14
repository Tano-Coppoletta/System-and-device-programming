#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX 31

struct student{
    int id;
    long reg_num;
    char surname[MAX];
    char name[MAX];
    int mark;
};


int main(int argc, char* argv[]){
    struct student s;

    if(argc!=4){
        fprintf(stderr,"Wrong parameters, usage: %s file1 file2 file3\n",argv[0]);
        exit(1);
    }

    FILE *f1,*f3;
    int f2;

    if((f1=fopen(argv[1],"r"))==NULL){
        fprintf(stderr,"Error in fopen\n");
        exit(1);
    }
    if((f2=open(argv[2],O_WRONLY))!=0){
        fprintf(stderr,"Error in open\n");
        exit(1);
    }

    while((fscanf(f1,"%d%l%s%s%d",&s.id,&s.reg_num,s.surname,s.name,&s.mark))!=EOF){
        write(f2,&s,sizeof(s));
    }
    fclose(f1);
    close(f2);

    if((f2=open(argv[2],O_RDONLY))!=0){
        fprintf(stderr,"Error in open\n");
        exit(1);
    }

    if((f3=fopen(argv[3],"w"))==NULL){
        fprintf(stderr,"Error in fopen\n");
        exit(1);
    }

    while((read(f2,&s,sizeof(struct student)))>0){
        fprintf(f3,"%d%l%s%s%d",s.id,s.reg_num,s.surname,s.name,s.mark);
    }

}