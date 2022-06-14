#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

#define BUFFERSIZE 1000
#define N 50

void visitDirRecur(char *pathname1, char *pathname2, int level);
void myCopy(char *sourceName,char *destName, char *name);


int main(int argc, char* argv[]){
    
    if(argc!=3){
        fprintf(stderr,"Wrong parameters\n");
        exit(1);
    }

    visitDirRecur(argv[1],argv[2],0);


}

void visitDirRecur(char *pathname1, char *pathname2, int level){
    struct dirent *dirp;
    struct DIR *dp;
    struct stat sb;
    char new_name1[N],new_name2[N];


    if(lstat(pathname1,&sb)!=0){
        fprintf(stderr,"error in pathname1\n");
        exit(1);
    }

    if(S_ISDIR(sb.st_mode)==0){
        fprintf(stderr,"Expected dir but it's something else\n");
        exit(1);
    }

    //create the directory
    mkdir(pathname2,sb.st_mode);

    if((dp=opendir(pathname1))!=0){
        fprintf(stderr,"Error in opendir\n");
        exit(1);
    }

    while((dirp=readdir(dp))!=NULL){
        sprintf(new_name1,"%s/%s",pathname1,dirp->d_name);
        sprintf(new_name2,"%s/%s",pathname2,dirp->d_name);
        if(lstat(new_name1,&sb)!=0){
        fprintf(stderr,"error in pathname1\n");
        exit(1);
        }if(S_ISDIR(sb.st_mode)==0){
            //fiile
            myCopy(new_name1,new_name2,dirp->d_name);
        }else{
            //directory
            if(strcmp(dirp->d_name,".")==0 || strcmp(dirp->d_name,"..")==0){
                continue;
            }
            visitDirRecur(new_name1,new_name2,level+1);
        }

    }

    if(closedir(dp)!=0){
        fprintf(stderr,"Error in closedir\n");
        exit(1);
    }
}

void myCopy(char *sourceName,char *destName,char *name){
    int f1,f2;
    struct stat st;
    char buf[BUFFERSIZE];

    if((f1=open(sourceName,O_RDONLY))!=0){
        fprintf(stderr,"Error in open f1\n");
        exit(1);
    }

    if((f2=open(destName,O_WRONLY))!=0){
        fprintf(stderr,"Error in open f1\n");
        exit(1);
    }

    if(write(f2,name,strlen(name))<0){
        fprintf(stderr,"Error in write\n");
        exit(1);
    }

    if(lstat(sourceName,&st)!=0){
        fprintf(stderr,"error in lstat\n");
        exit(1);
    }

    if(write(f2,(void*)&st.st_size,sizeof(int))<0){
        fprintf(stderr,"Error in write\n");
        exit(1);
    }

    while(read(f1,&buf,BUFFERSIZE*sizeof(char))>0){
        write(f2,&buf,BUFFERSIZE*sizeof(char));
    }

    close(f1);
    close(f2);

}