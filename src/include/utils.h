#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define READ_END 0
#define WRITE_END 1

int createPipe(int fildes[2]);
int createFork();
int rSelect(int nfds, fd_set *restrict readfds);

ssize_t readFd(int fildes, void *buf, size_t nbyte);
ssize_t writeFd(int fildes, const void *buf, size_t nbyte);
int openFile(const char *pathname, int flags, mode_t mode);