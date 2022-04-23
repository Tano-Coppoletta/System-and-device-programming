#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc,char* argv[]){

    if(argc!=3){
        fprintf(stderr,"Invalid argument, use: toBin file.txt fileo");
        exit(1);
    }

    int fout;
    FILE *fin;

    fin=fopen(argv[1],"r");
    fout=open(argv[2],O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR,S_IWUSR);
    if(fin==NULL || fout<0){
        fprintf(stderr,"error in open or fopen\n");
        exit(1);
    }

    int num;

    while(fscanf(fin,"%d",&num)!=EOF){
        write(fout,&num,sizeof(int));
    }



    fclose(fin);
    close(fout);

    return 0;
}