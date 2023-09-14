#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
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