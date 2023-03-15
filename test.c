#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{

    pid_t pid;
    char * message;

    char msg[64]={0,};
    int n;
    
    pid=fork();

    if(pid < 0){
        perror("fork failed");
        exit(1);
    }

    if(pid == 0){
        message="this is the child \n";
        n = 6;
    }else{
        message="this is the parent \n";
        n = 3;
    }

    for(;n>0;n--){
        sprintf(msg,"pid:%d ,msg:%s",pid,message);
        printf("%s",msg);
        sleep(1);
    }
    return 0;
}