// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <slave.h>

void processFile(char * file, char buff[], int fifo);

int main(int argc, char * argv[]) {

    // Turning off print buffering
    setvbuf(stdout, NULL, _IONBF, 0);
    
    int myPid = getpid();

    int fifo = open("tpeFifo", O_WRONLY);

    while (TRUE) {
        char fileName[FILE_NAME_SIZE];
        int r = readFd(STDIN_FILENO, fileName, FILE_NAME_SIZE - 1);
        if (r == 0) {
            exit(EXIT_SUCCESS);
        }
/* 
        Some background on how FIFOs are opened
        Read-only, without O_NONBLOCK: open blocks until another process opens the FIFO for writing. This is the behavior when using fopen with mode argument "r".

        Write-only, without O_NONBLOCK: open blocks until another process opens the FIFO for reading. This is the behavior when using fopen with mode argument "w".

        Read-only, with O_NONBLOCK: open returns immediately.

        Write-only, with O_NONBLOCK: open returns an error with errno set to ENXIO unless another process has the FIFO open for reading. */

        int last = 0;
        for (int i = 0; i < r; i++) {
            if (fileName[i] == 0) {
                char rta[MD5_ANS_SIZE];
                processFile(fileName + last, rta, fifo);
                printf("%d:\t%s", myPid, rta); 

                // Write to fifo
                // dprintf(fifo, "%d:\t%s\n", myPid, rta);
                write(fifo, rta, MD5_ANS_SIZE);

                last = i + 1;
            }
        }
    }
}

void processFile(char * file, char buff[], int fifo) {
    int pipefds[2];

    createPipe(pipefds);

    int pid = createFork();
    if (pid == 0) {
        // Child
        close(STDOUT_FILENO); // Close stdout
        dup(pipefds[WRITE_END]); // Write end of pipe
        close(pipefds[READ_END]);
        close(pipefds[WRITE_END]);

        execlp("md5sum", "md5sum", file, NULL);
        perror("Error while calling md5sum");
        exit(EXIT_FAILURE);        
    } else {
        int stat;
        waitpid(pid, &stat, 0);
        if (stat != 0) {
            perror("Error while doing md5 in slave");
            exit(EXIT_FAILURE);
        }
        int r = readFd(pipefds[READ_END], buff, MD5_ANS_SIZE - 1);
        buff[r] = '\0';
        close(pipefds[READ_END]);
        close(pipefds[WRITE_END]);
    }
}