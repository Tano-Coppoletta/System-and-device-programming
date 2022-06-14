#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <threads.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

struct tS{
    int id;
    char *pathname;
};

void *tF(void *args);
void visidDirRecur(char *pathname, int level,int id);

int main(int argc, char* argv[]){
    int n=argc-1;

    setbuf(stdout,0);
    pthread_t *tid;
    struct tS *t_data;

    tid=malloc(n*sizeof(pthread_t));
    t_data=malloc(n*sizeof(struct tS));

    if(tid==NULL || t_data==NULL){
        fprintf(stderr,"Error in malloc\n");
        exit(1);
    }

    for(int i=1;i<=n;i++){
        t_data[i-1].id=i;
        t_data[i-1].pathname=argv[i];
        pthread_create(&tid[i-1],NULL,tF,(void*)&t_data[i-1]);
    }

    for(int i=1;i<=n;i++){
        pthread_join(tid[i-1],NULL);
    }

    free(tid);
    free(t_data);


}


void *tF(void *args){
    struct tS *t_data=(struct tS*)args;
    int id=(int)t_data->id;
    char pathname=(char*)t_data->pathname;

    visidDirRecur(pathname,0,id);

    pthread_exit(NULL);
}

void visidDirRecur(char *pathname, int level,int id){
    struct stat sb;
    struct dirent *dirp;
    struct DIR *dp;
    char new_name[1000];

    if(lstat(pathname,&sb)!=0){
        fprintf(stderr,"Error in lstat\n");
        exit(1);
    }
    if(S_ISDIR(sb.st_mode)==0){
        //expected dir but it's not
        return;
        
    }

    if((dp=opendir(pathname))!=0){
        fprintf(stderr,"Error in opendir\n");
        exit(1);
    }
    while((dirp=readdir(dp))!=NULL){
        lstat(dirp->d_name,&sb);
        sprintf(new_name,"%s/%s",pathname,dirp->d_name);
        if(S_ISDIR(sb.st_mode)==0){
            //file
            printf("FILE %s Thread: %d\n",new_name,id);
            return;
        }else{
            //directory
            if(strcmp(dirp->d_name,".")==0 || strcmp(dirp->d_name,"..")==0){
                continue;
            }
            printf("DIR: %s Thread: %d\n",new_name,id);
            visidDirRecur(new_name,level+1,id);
        }
    }


    if(closedir(dp)<0){
        fprintf(stderr,"error in closedir\n");
        exit(1);
    }

}   