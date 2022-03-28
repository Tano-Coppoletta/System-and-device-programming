#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define N 1000

int main(int argc,char* argv[]){
   
    if(argc!=3){
        fprintf(stdout,"Wrong parameters!\n");
        exit(1);
    }
    visitDir(argv[1],argv[2]);

    return(0);
   
}

void visitDir(char *fullnameR,char *fullnameW){
    struct stat sb;
    DIR *dp;
    struct dirent *dirp;
    char new_name[N],new_nameW[N];

     if(lstat(fullnameR,&sb)<0){
        fprintf(stdout,"Error\n");
        exit(1);
    }
    fprintf(stdout,"visiting : %s \n",fullnameR);
    if(S_ISDIR(sb.st_mode)==0){
        //end of recursion;
        return;
    }

    
    //is a directory
    //create a new directory
    mkdir(fullnameW,sb.st_mode);
    fprintf(stdout,"create dir: %s\n",fullnameW);

    //visit the directory
    if((dp=opendir(fullnameR))==NULL){
        fprintf(stdout,"Error in opening the directory\n");
        exit(1);
    }
    while((dirp=readdir(dp))!=NULL){
        sprintf(new_name,"%s/%s",fullnameR,dirp->d_name);
        sprintf(new_nameW,"%s/%s",fullnameW,dirp->d_name);
      //  printf("\n%s/%s\n",fullnameR,dirp->d_name);
      //  printf("\n%s/%s\n",fullnameW,dirp->d_name);
        if(lstat(new_name,&sb)<0){
            fprintf(stdout,"Error\n");
            exit(1);
        }
        if(S_ISDIR(sb.st_mode)==0){
            //FILE
            copyFile(new_name,new_nameW,dirp->d_name);
        }else{
            //DIRECTORY
             if (strcmp(dirp->d_name, ".") == 0  ||
                strcmp(dirp->d_name, "..") == 0)
                continue;
            visitDir(new_name,new_nameW);
        }
    }

    if(closedir(dp)<0){
        fprintf(stdout,"Erron in closedir\n");
        exit(1);
    }


}

void copyFile(char *source,char *dest,char *name){
    int fp,fd,nR,nW;
    struct stat sbuf;
    char buffer[100];

    fp=open(source,O_RDONLY);

   // printf("\nfile : %s\n",dest);
    fd=open(dest,O_CREAT | O_WRONLY);
    
    if(fp < 0 ){
        fprintf(stdout,"Error in file open 1\n");
        exit(2);
    }

    if(fd < 0) {
         fprintf(stdout,"Error in file open 2\n");
        exit(2);
    }

    //write the name of the file in dest
    if(write(fd,name,strlen(name))!=strlen(name)){
        fprintf(stdout,"Error in writing name\n");
    }

    fstat(fp,&sbuf);
    //write size of source
    if(write(fd,&sbuf.st_size,sizeof(int))!=sizeof(int)){
        fprintf(stdout,"Error in writing size\n");
    }

    //copy
    while((nR=read(fp,buffer,100)) > 0 ){
        nW=write(fd,buffer,nR);
       // write(1,buffer,nR);
        if(nR!=nW){
            fprintf(stdout,"Error in copy\n");
            exit(1);
        }

    }

    close(fp);
    close(fd);

}