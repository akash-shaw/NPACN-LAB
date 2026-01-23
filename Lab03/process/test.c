#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>

int main(){

    int mychild;;
    printf("parent process\n");
    mychild = fork();
    if(mychild){
        printf("Inside Parent\n");
        
    }
    else{
        printf("Inside child %d of parent %d\n",getpid(), getppid());
    }
    return 0;
}

