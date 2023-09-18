#ifndef SHMADT_H
#define SHMADT_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#define SHM_BUFF_SIZE 4096
#define SHM_NAME_SIZE 10

typedef struct shmCdt * shmAdt;

shmAdt newShm(const char *name);

void deleteShm(shmAdt shmp);

shmAdt connectShm(const char *shmpath);

int closeShm(shmAdt shmp);

void writeShm(shmAdt shmp, char * string, size_t len);

void readShm(shmAdt shmp, char * buff);

#endif