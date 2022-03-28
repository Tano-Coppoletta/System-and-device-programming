
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <dirent.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <pthread.h>

#include  <limits.h>
#include  <errno.h>
#include  <stdarg.h>


typedef struct thread_s{
    int tid;
    char *path;

}thread_t;

void visitDir(char* path,int tid);

void *tF(void *arg){
    thread_t *tS;
    int id;
    char *path;

    tS=(thread_t*)arg;
    id=tS->tid;
    path=tS->path;

    fprintf(stdout,"thread:%d path:%s\n",id,path);

    visitDir(path,id);

    pthread_exit(NULL);
}

int main(int argc,char* argv[]){
    thread_t *tS;
    int i;
    pthread_t *t;

    if(argc<2){
        fprintf(stdout,"Insert at least one parameter\n");
        exit(1);
    }

    //fprintf(stdout,"parameters: %d %s %s\n",argc,argv[1],argv[2]);

    tS=malloc((argc-1)*sizeof(thread_t));
    t=malloc((argc-1)*sizeof(pthread_t));

    if(tS==NULL || t==NULL){
        fprintf(stdout,"Allocation error\n");
        exit(1);
    }
    
    for(i=1;i<argc;i++){
        tS[i-1].tid=i;
        tS[i-1].path=strdup(argv[i]);
        
        fprintf(stdout,"i=%d id=%d path=%s\n",i,tS[i-1].tid,tS[i-1].path);
        pthread_create(&t[i-1],NULL, tF,(void *) &tS[i-1]);

    }

    for(i=1;i<argc;i++){
        //wait for all the threads
        pthread_join(&t[i-1],NULL);
    }

    //free(tS);
    //free(t);
    return 0;
}

void visitDir(char* path,int tid){
    struct stat sb;
    DIR *dp;
    struct dirent *dirp;
    char new_name[10000];

    fprintf(stdout,"visiting %s tid %d\n",path,tid);
    if(stat(path,&sb)<0){
        fprintf(stdout,"Error tid=%d\n",tid);
        exit(1);
    }
    if(S_ISDIR(sb.st_mode)==0){
        //this is not a directory
        return;
    }
    if((dp=opendir(path))==NULL){
        fprintf(stdout,"Error opening %s by tid %d\n",path,tid);
        exit(1);
    }

    while((dirp=readdir(dp))!=NULL){
        //new_name = malloc ((strlen(path)+strlen(dirp->d_name)+2) * sizeof(char));
       // if(new_name==NULL){
        //    fprintf(stdout,"Allocation error new_name\n");
       // }
        sprintf(new_name,"%s/%s",path,dirp->d_name);
        if(stat(new_name,&sb)<0){
            fprintf(stdout,"Error\n");
            exit(1);
        }
        if(S_ISDIR(sb.st_mode)==0){
            //FILE
            fprintf(stdout,"tid: %d  FILE: %s\n",tid,new_name);
        }else{
            //directory
            if (strcmp(dirp->d_name, ".") ==0 || strcmp(dirp->d_name, "..") == 0)
                continue;
            fprintf(stdout,"tid: %d  directory: %s\n",tid,new_name);
            visitDir(new_name,tid);
        }
        //free(new_name);
    }

    if(closedir(dp)<0){
        fprintf(stdout,"Error in closedir\n");
        exit(1);
    }
}