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
    char command;
    int n;

    if(argc<2){
        fprintf(stdout,"Wrong parameters!\n");
        exit(1);
    }

    int file=open(argv[1],O_RDWR);
    if(file<0){
        fprintf(stdout,"Error opening file\n");
        exit(1);
    }

    fprintf(stdout,"-R n to get information abount student n\n-W n to insert a new student\n-E to end program\n");

    fscanf(stdin,"%c",&command);

    while(command!='E'){
        
        if(command=='R'){
            fscanf(stdin,"%d",&n);
            lseek(file,((n-1)*sizeof(student_t)),SEEK_SET);
            read(file,(void*) &s,sizeof(student_t));
            fprintf(stdout,"%d %ld %s %s %d\n",s.id,s.reg,s.surname,s.name,s.mark);
                
        }else if(command=='W'){
            fscanf(stdin,"%d",&n);
            fprintf(stdout,"Insert id reg surname name mark\n");
            fscanf(stdin,"%d %ld %s %s %d",&s.id,&s.reg,&s.surname,&s.name,&s.mark);
            //move on the file
            lseek(file,(n-1)*sizeof(student_t),SEEK_SET);
            write(file,(void*)&s,sizeof(student_t));
            fprintf(stdout,"Write executed!\n");
        }
        //read new command
        fprintf(stdout,"-R n to get information abount student n\n-W n to insert a new student\n-E to end program\n");
        fscanf(stdin,"%c",&command);
    }
}
