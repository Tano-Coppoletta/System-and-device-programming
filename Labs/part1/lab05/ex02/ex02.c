#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <pthread.h>
#include <fcntl.h>

typedef struct thread_s{
    char *file;
    int id;
    int *array;
    int length;
}thread_t;

int main(int argc, char* argv[]){
    
}