#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>

#define BUF_SIZE 2048

typedef struct shmCdt * shmAdt;

shmAdt newShm(const char *name);

int unlinkShm(shmAdt shmp);

shmAdt connectShm(const char *shmpath);

int writeShm(shmAdt shmp, char * string, size_t len);