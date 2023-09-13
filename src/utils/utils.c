#include <utils.h>

int createPipe(int fildes[2]) {
    int ret = pipe(fildes);
    if (ret != 0) {
        perror("Error while creating pipe");
        exit(EXIT_FAILURE);
    }
    return ret;
}

int createFork() {
    int pid = fork();
    if (pid < 0) {
        perror("Error while creating slave");
        exit(EXIT_FAILURE);
    }
    return pid;
}

int rSelect(int nfds, fd_set *restrict readfds) {
    int r = select(nfds, readfds, NULL, NULL, NULL);
    if (r < 0) {
        perror("Error while doing select in app");
        exit(1);
    }
    return r;            
}

ssize_t readFd(int fildes, void *buf, size_t nbyte) {
    int r = read(fildes, buf, nbyte);
    if (r < 0) {
        perror("Error while reading fd in app");
        exit(1);
    }
    return r;
}

ssize_t writeFd(int fildes, const void *buf, size_t nbyte) {
    int r = write(fildes, buf, nbyte);
    if (r < 0) {
        perror("Error while writing fd in app");
        exit(1);
    }
    return r;
}