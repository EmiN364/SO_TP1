// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <slave.h>

void processFile(char * file, char buff[]);

int main(int argc, char * argv[]) {
    // Turning off print buffering
    setvbuf(stdout, NULL, _IONBF, 0);
    
    int myPid = getpid();

    while (1) {
        char fileName[FILE_NAME_SIZE];
        int r = readFd(STDIN_FILENO, fileName, FILE_NAME_SIZE - 1);
        if (r == 0) {
            exit(EXIT_SUCCESS);
        }

        int last = 0;
        for (int i = 0; i < r; i++) {
            if (fileName[i] == 0) {
                char rta[128];
                // dprintf(2, "Processing: %s\n", fileName + last);
                processFile(fileName + last, rta);
                printf("%d:\t%s", myPid, rta);
                last = i + 1;
            }
        }
    }
}

void processFile(char * file, char buff[]) {
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
        int r = readFd(pipefds[READ_END], buff, 128);
        buff[r] = 0;
        close(pipefds[READ_END]);
        close(pipefds[WRITE_END]);
    }
}