#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void processInitialFile(char * file, char buff[]);

int main(int argc, char * argv[]) {

    // Turning off print buffering
    setvbuf(stdout, NULL, _IONBF, 0);
    
    // printf("ESCLAVO: Cant args: %d\n", argc);
    int myPid = getpid();

    char rtas[argc-1][128];

    int files = 0;

    for (int i = 1; i < argc; i++) {
        // printf("ESCLAVO: Arg %d: %s\n", i, argv[i]);
        if (*argv[i] != 0) {
            processInitialFile(argv[i], rtas[i-1]);
            files++;
        }
    }

    for (int i = 0; i < files; i++) {
        printf("%d:\t%s", myPid, rtas[i]);
    }

    while (1) {
        char fileName[128];
        int r = read(0, fileName, 128);
        fileName[r] = 0;
        if (r < 0) {
            perror("Error while reading");
            exit(1);
        }
        if (r == 0) {
            printf("Slave ended");
            exit(0);
        }
        if (fileName != NULL) {
            char rta[128];
            dprintf(2, "Processing: %s\n", fileName);
            processInitialFile(fileName, rta);
            printf("%d:\t%s", myPid, rta);
            // write(1, rta, 128);
        }
    }
}

void processInitialFile(char * file, char buff[]) {
    int pipefds[2];

    if (pipe(pipefds) != 0) {
        perror("Error while creating pipe in slave.");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        perror("Error while fork in slave.");
        exit(1);
    } else if (pid == 0) {
        // Child
        close(1); // Close stdout
        dup(pipefds[1]); // Write end of pipe
        close(pipefds[0]);
        close(pipefds[1]);

        char * args[3];
        args[0] = "md5sum";
        args[1] = file;
        args[2] = NULL;

        execvp("md5sum", args);
        perror("Errir while calling md5sum");
        exit(1);        
    } else {
        int stat;
        waitpid(pid, &stat, 0);
        if (stat != 0) {
            perror("Error while doing md5 in slave");
            exit(1);
        }
        int r = read(pipefds[0], buff, 128);
        buff[r] = 0;
        close(pipefds[0]);
        close(pipefds[1]);
    }
}