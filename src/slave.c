#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
    // printf("ESCLAVO: Cant args: %d\n", argc);
    int myPid = getpid();

    char rtas[argc][128];

    int files = 0;

    for (int i = 0; i < argc; i++) {
        // printf("ESCLAVO: Arg %d: %s\n", i, argv[i]);
        if (*argv[i] != NULL) {
            processInitialFile(argv[i], rtas[i]);
            files++;
        }
    }

    for (int i = 0; i < files; i++) {
        printf("%d:\t%s", myPid, rtas[i]);
    }
}

void processInitialFile(char * file, char buff[]) {
    int pipefds[2];

    if (pipe(pipefds) != 0) {
        perror("Error while creating pipe in slave.");
        return 1;
    }

    int pid = fork();
    if (pid < 0) {
        perror("Error while fork in slave.");
        exit(1);
    } else if (pid == 0) {
        // Child
        close(1); // Close stdout
        dup(pipefds[1]); // Write end of pipe

        char * args[3];
        args[0] = file;
        args[1] = file;
        args[2] = NULL;

        execvp("md5sum", args);
    } else {
        int stat;
        waitpid(pid, &stat, 0);
        if (stat != 0) {
            perror("Error while doing md5 in slave");
            return 1;
        }
        int r = read(pipefds[0], buff, 128);
        buff[r] = 0;
    }
}