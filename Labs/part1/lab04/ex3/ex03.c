#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

#define N 50

typedef struct thread_s{
    int id;
    char currentDir[N];
    char currentFile[N];
    int endOfDir;
    sem_t sem;
}reading_t;

typedef struct thread_c{
    reading_t *r; //pointer to the reading thread
    int nt; //number of thread
}comparison_t;

sem_t se; //semaphore for comparing t

void *thread_compare(void* arg);
void *thread_visit(void *arg);
void thread_visit_rec(reading_t *data);


int main(int argc,char* argv[]){
    reading_t *rt;
    comparison_t ct;
    pthread_t *pid;

    if(argc<=3){
        fprintf(stderr,"Wrong usage, use: prgm path1 path2\n");
        exit(1);
    }

    rt=malloc((argc-1)*sizeof(reading_t));
    pid=malloc((argc)*sizeof(pthread_t));
    if(rt==NULL || pid==NULL){
        fprintf(stderr,"Error in malloc\n");
        exit(1);
    }

    //initialize sem_t se
    sem_init(&se,0,0);
    ct.nt=argc-1;
    ct.r=rt;
    //create comparison thread
    pthread_create(&pid[0],NULL,thread_compare,&ct);

    //create reading threads
    for(int i=0;i<argc-1;i++){
        strcpy(rt[i].currentDir,argv[i+1]);
        rt[i].id=i+1;
        sem_init(&rt[i].sem,0,1);
        rt[i].endOfDir=0;

        pthread_create(&pid[i+1],NULL,thread_visit,&rt[i]);
    }

    for(int i=0;i<argc-1;i++){
        pthread_join(pid[i],NULL);
    }

    fprintf (stdout, "--> OK directories are equivalent.\n");

    return 0;


}

void *thread_compare(void* arg){
    comparison_t *data;
    data=(comparison_t*)arg;
    int finished=0;

    while(1){
        for(int i=0;i<data->nt;i++){
            sem_wait(&se);
        }
        fprintf (stdout, "---- Comparing Entry: %s ", data->r[0].currentFile);
        for(int i=0;i<data->nt;i++){
            if(data->r[i].endOfDir==1){
                finished++;
            }else{
                if(i>0 && strcmp(data->r[0].currentFile,data->r[i].currentFile)!=0){
                    fprintf(stdout,"DIFFER %s %s\n",data->r[0].currentFile,data->r[i].currentFile);
                    //end all
                    exit(1);
                }
            }
        }
        if(finished==0 || finished==data->nt){
            fprintf(stdout,"All ok\n");
        }else{
            fprintf(stdout,"terminate all, someone has finished\n");
            exit(1);
            //terminate all
        }
        // Release All Reading Threads
        for (int i = 0; i<data->nt; i++) {
            sem_post (&data->r[i].sem);
        }
    }
    pthread_exit(0);
}

void *thread_visit(void *arg){
    reading_t *rt;
    rt=(reading_t*)arg;

    thread_visit_rec(rt);
    rt->endOfDir=1;
    sem_post(&se);

    pthread_exit(NULL);

}

void thread_visit_rec(reading_t *data){
    struct stat sb;
    DIR *dp;
    struct dirent *dirp;
    char s1[N],s2[N];

    strcpy(s1,data->currentDir);
    if(lstat(s1,&sb)!=0){
        fprintf(stderr,"Error in stat\n");
        exit(1);
    }

    if(S_ISDIR(sb.st_mode)==0){
        //end of recursion
        //FILE
        fprintf(stderr,"End of recursion\n");
        exit(1);
    }

    if((dp=opendir(s1))==NULL){
        fprintf(stderr,"Error in dir open\n");
    }

    //for each entry
    while((dirp=readdir(dp))!=NULL){
        strcpy(data->currentFile,dirp->d_name);
        fprintf(stdout,"Thread: %d file: %s\n",data->id,data->currentFile);
        sem_post(&se);
        sem_wait(&data->sem);


        //dir
        if(S_ISDIR(sb.st_mode)){
            if (strcmp(dirp->d_name, ".") == 0  ||
            strcmp(dirp->d_name, "..") == 0)
            continue;
        }

        sprintf(s1,"%s",data->currentDir);
        sprintf(s2,"%s",data->currentDir);
        strcat(s2,"\\");
        strcat(s2,dirp->d_name);
        strcpy(data->currentDir,s2);

        thread_visit_rec(data);

        //restore the value od currentdir
        strcpy(data->currentDir,s1);
    }

    if (closedir(dp) < 0) {
        fprintf (stderr, "Error.\n");
        exit (1);
    }

    return ;

}