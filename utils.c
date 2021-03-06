// server.c 
// Developed by: 
// João Araujo - 201705577
// Leonor Sousa - 201705377
// Afonso Mendonça - 201706708
// SOPE - MIEIC2 - 2018/2019

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h>
#include "constants.h"
#include <time.h>

//function that produces an hash/sha256sum
void produceSha(const char* toEncrypt, char* encrypted)
{
    int fd[2];
    pipe(fd);
    int pid=fork();
    if (pid<0)
    {
        perror("Error ocured in fork.\n");
        exit(50);
    }
    if (pid==0)
    {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);

        int fd2[2];
        pipe(fd2);
        pid=fork();
        if (pid<0)
        {
            perror("Error ocurred in fork.\n");
            exit(50);
        }
        if (pid==0)
        {
            close(fd2[0]);
            dup2(fd2[1], STDOUT_FILENO);
            execlp("echo", "echo", "-n",toEncrypt, NULL);
            perror("Error on call to echo.\n");
            exit(50);
        }
        
        close(fd2[1]);
        wait(NULL);
        dup2(fd2[0], STDIN_FILENO);
        execlp("sha256sum", "sha256sum", NULL);
        perror("Error on call to sha256sum.\n");
        exit(50);
    }
    close(fd[1]);
    int n;
    n=read(fd[0], encrypted, HASH_LEN);
    encrypted[n]='\0';
    //printf("%s\n",encrypted);
    wait(NULL);
}

//produces a random salt based on current time and pid of the process
void produceSalt(char* salt)
{
    char temp[20];
    sprintf(salt,"%lx",clock());
    sprintf(temp,"%x",getpid());
    strcat(salt, temp);
    while (strlen(salt)<64){
        sprintf(temp,"%x",(rand() % 16));
        strcat(salt, temp);}
    //printf("%s\n",salt);
}

//creates a fifo with the name fifo_name
void createFifo(char* fifo_name)
{
    if (mkfifo(fifo_name,0660)<0 && errno!=EEXIST)
    {
        printf("Can't create FIFO %s\n", fifo_name);
        exit(2);
    }
}

//opens a fifo in read mode
int openReadFifo(char* fifo_name)
{
    int fd;    
    if ((fd=open(fifo_name, O_RDONLY | O_NONBLOCK)) <0)
    {
        printf("Can't open FIFO %s\n", fifo_name);
        return -1;
    }
    return fd;
}

//opens a fifo in write mode
int openWriteFifo(char* fifo_name)
{
    int fd;    
    if ((fd=open(fifo_name, O_WRONLY | O_APPEND | O_NONBLOCK)) <0)
    {
        printf("Can't open FIFO %s. Server offline.\n", fifo_name);
        return -1;
    }
    return fd;
}

//closes and unlinks a fifo
void closeUnlinkFifo(char* fifo_name, int fd)
{
    close(fd);
    if (unlink(fifo_name)<0)
    {
        printf("Error when destroying FIFO %s\n", fifo_name);
        exit(0); 
    }
}
