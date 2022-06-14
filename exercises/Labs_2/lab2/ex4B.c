#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <threads.h>
#include <dirent.h>

struct tS{
    int id;
    char file_name[50];
    char *pathname;
};

void *tF(void* args);
void visitDirRecur(char *pathname,int fp, int level,int id);

int main(int argc, char* argv){
    pthread_t *pid;
    struct tS *t_data;
    int n=argc-1;
    char filename[50],*command;

    pid=malloc(n*sizeof(pthread_t));
    t_data=malloc(n*sizeof(struct tS));

    if(pid==NULL || t_data==NULL){
        fprintf(stderr,"Error in malloc\n");
        exit(1);
    }

    for(int i=1;i<=n;i++){
        t_data[i-1].id=i;
        t_data[i-1].pathname=argv[i];
        sprintf(filename,"file_%d",i);
        strcpy(t_data[i-1].file_name,filename);
        pthread_create(&pid[i-1],NULL,tF,(void*)&t_data[i]);
    }

    for(int i=1;i<=n;i++){
        pthread_join(pid[i-1],NULL);
    }
    

    for (int i=1; i<argc; i++) {
      command = malloc (strlen(t_data[i-1].file_name+20) * sizeof (char));
      sprintf (command, "cat %s >> %d.txt", t_data[i-1].file_name, getpid());
      system (command);
      sprintf (command, "/bin/rm -rf %s", t_data[i-1].file_name);
      system (command);
      free (command);
    }
}

void *tF(void* args){
    struct tS *t_data=(struct tS*)args;
    int id=t_data->id;
    char *filename=t_data->file_name;
    char *pathname=t_data->pathname;
    int fp;
    if((fp=open(filename,O_RDWR))!=0){
        fprintf(stderr,"Error in open\n");
        exit(1);
    }

    visitDirRecur(pathname,fp,0,id);

    pthread_exit(NULL);
}

void visitDirRecur(char *pathname,int fp, int level,int id){
    struct stat sb;
    struct dirent *dirp;
    struct DIR *dp;
    char new_name[1000];

    if(lstat(pathname,&sb)!=0){
        fprintf(stderr,"Error in lstat\n");
        exit(1);
    }

    if(S_ISDIR(sb.st_mode)==0){
        //file
        return;
    }

    //directory

    if((dp=opendir(pathname))!=0){
        fprintf(stderr,"Error in opendir\n");
        exit(1);
    }

    while((dirp=readdir(dp))!=NULL){
        sprintf(new_name,"%s/%s",pathname,dirp->d_name);
        if(lstat(new_name,&sb)!=0){
            fprintf(stderr,"Error in lstat\n");
            exit(1);
        }
        if(S_ISDIR(sb.st_mode)==0){
            //file
            fprintf(fp,"FILE: %s THREAD: %d\n",new_name,id);
        }else{
            //directory
            fprintf(fp,"DIR: %s THREAD: %d\n",new_name,id);
            visitDirRecur(new_name,fp,level+1,id);
        }
    }
}