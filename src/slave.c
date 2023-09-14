#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <utils.h>

void processInitialFile(char * file, char buff[]);

int main(int argc, char * argv[]) {

    // Turning off print buffering
    setvbuf(stdout, NULL, _IONBF, 0);
    
    int myPid = getpid();

    while (1) {
        char fileName[256];
        int r = readFd(STDIN_FILENO, fileName, 255);
        if (r == 0) {
            exit(0);
        }
        fileName[r] = 0;
        
        char rta[128];
        // dprintf(2, "Processing: %s\n", fileName);
        processInitialFile(fileName, rta);
        printf("%d:\t%s", myPid, rta);
    }
}

void processInitialFile(char * file, char buff[]) {
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
        exit(1);        
    } else {
        int stat;
        waitpid(pid, &stat, 0);
        if (stat != 0) {
            perror("Error while doing md5 in slave");
            exit(1);
        }
        int r = readFd(pipefds[0], buff, 128);
        buff[r] = 0;
        close(pipefds[0]);
        close(pipefds[1]);
    }
}