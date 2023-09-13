#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>

int createPipe(int fildes[2]);
int createFork();
int rSelect(int nfds, fd_set *restrict readfds);

ssize_t readFd(int fildes, void *buf, size_t nbyte);
ssize_t writeFd(int fildes, const void *buf, size_t nbyte);