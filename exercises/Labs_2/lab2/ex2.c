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


int main(int argc,char *argv[]){
    char cmd;
    int n;
    int finish=0;

    int fd;
    struct student s;
    struct flock fl;

    if((fd=open(argv[1],O_RDWR))!=0){
        fprintf(stderr,"Error in open\n");
        exit(1);
    }

    while(!finish){
        scanf("%c",&cmd);
        

        switch (cmd)
        {
        case 'R':
            scanf("%d",&n);
            fl.l_type=F_RDLCK;
            fl.l_whence=SEEK_SET;
            fl.l_len=sizeof(s);
            fl.l_start=(n-1)*sizeof(s);
            fcntl(fd,F_SETLK,&fl);
            if(lseek(fd,(n-1)*sizeof(s),SEEK_SET)!=0){
                fprintf(stderr,"Error in lseek\n");
                exit(1);
            }
            if(read(fd,&s,sizeof(s))<0){
                fprintf(stderr,"Error in read\n");
                exit(1);
            }
            fl.l_type=F_UNLCK;
            fcntl(fd,F_SETFL,&fl);
            printf("%d %ld %s %s %d",s.id,s.reg_num,s.surname,s.name,s.mark);
            break;

        case 'W':
            scanf("%d",&n);
            fl.l_type=F_WRLCK;
            fl.l_len=sizeof(s);
            fl.l_start=(n-1)*sizeof(s);
            fl.l_whence=SEEK_SET;
            fcntl(fd,F_SETLK,&fl);
            if(lseek(fd,(n-1)*sizeof(s),SEEK_SET)!=0){
                fprintf(stderr,"Error in lseek\n");
                exit(1);
            }
            scanf("%d %ld %s %s %d",&s.id,&s.reg_num,s.surname,s.name,&s.mark);
            if(write(fd,&s,sizeof(s))<0){
                fprintf(stderr,"Error in write\n");
                exit(1);
            }
            fl.l_type=F_UNLCK;
            fcntl(fd,F_SETLK,&fl);
            printf("Write done");
            break;
        case 'E':
            finish=1;
            break;
        
        default:
            break;
        }
    }
}