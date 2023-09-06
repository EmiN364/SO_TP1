#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
    // printf("ESCLAVO: Cant args: %d\n", argc);

    char * rtas[128];

    for (int i = 0; i < argc; i++) {
        // printf("ESCLAVO: Arg %d: %s\n", i, argv[i]);
        processInitialFile(argv[i]);
    }
}

int processInitialFile(char * file) {
    /* int pipefds[2];

    if (pipe(pipefds) != 0) {
        perror("Error while creating pipe in slave.");
        return 1;
    } */

    int pid = fork();
    if (pid < 0) {
        perror("Error while fork in slave.");
        return 1;
    } else if (pid == 0) {
        // Child
        /* close(1); // Close stdout
        dup(pipefds[1]); // Write end of pipe */

        char * args[2];
        args[0] = file;
        args[1] = NULL;

        execve("md5sum", args, NULL);
    } else {
        int stat;
        waitpid(pid, &stat, 0);
        if (stat != 0) {
            perror("Error while doing md5 in slave");
            return 1;
        }
    }
}
