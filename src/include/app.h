#include <stdio.h>
#include <string.h>
#include <utils.h>

typedef struct Slave {
    pid_t pid;
    int readfd;
    int writefd;
    int filesProcessed;
    int filesSent;
} Slave;

#define SLAVE_AMOUNT 5

int analyzeRead(char * buff, int len);