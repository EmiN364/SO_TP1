#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <string.h>
#include <utils.h>
#include <shmAdt.h>

typedef struct Slave {
    pid_t pid;
    int readfd;
    int writefd;
    int filesProcessed;
    int filesSent;
} Slave;

#define SLAVE_MAX_AMOUNT 5
#define FD_AMOUNT 2
#define BUFF_SIZE 1024

int analyzeRead(char * buff, int len);

#endif