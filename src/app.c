#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>

#define SLAVE_AMOUNT 5

int main(int argc, char * argv[], char * envp[]) {
    printf("Cant args: %d\n", argc);
    setvbuf(stdout, NULL, _IONBF, 0);

    char * files[argc - 1];
    int countFiles = 0;

    struct stat fileData;
    for (int i = 1; i < argc; i++) {
        //printf("Arg %d: %s\n", i, argv[i]);
        stat(argv[i], &fileData);
        //printf("Mode: %s \n", S_ISREG(fileData.st_mode) ? "File" : "Not file");
        
        if (S_ISREG(fileData.st_mode)) {
            files[countFiles++] = argv[i];
        }
    }

    printf("Cant Files: %d\n", countFiles);
/*     for (int i = 0; i < countFiles; i++) {
        printf("File %d: %s\n", i, files[i]);
    } */

    if (countFiles < 1) {
        puts("No files to process");
        return 0;
    }

    int outputFile = open("output.txt", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (outputFile < 0) {
        perror("Error while creating output file");
        return 1;
    }

    int filesToSlaves = countFiles * 0.1;
    if (filesToSlaves == 0) filesToSlaves = 1;
    // printf("filesToSlaves: %d\n", filesToSlaves);

    int filesPerSlave = filesToSlaves / SLAVE_AMOUNT; 
    if (filesPerSlave == 0) filesPerSlave = 1;
    // printf("filesPerSlave: %d\n", filesPerSlave);
    // TODO: Ver caso si es 0

    int filesSent = 0;
    int filesProcessed = 0;

    int slavePids[SLAVE_AMOUNT];

    int fds[SLAVE_AMOUNT * 2];

    for (int i = 0 ; i < SLAVE_AMOUNT ; i++) {
        char * slaveFiles[filesPerSlave + 2];
        slaveFiles[0] = "slave";
        if (filesSent < filesToSlaves) { // We check this in case filesPerSlave was 0
            for (int j = 1; j < filesPerSlave + 1; j++)
                slaveFiles[j] = files[filesSent++];

            slaveFiles[filesPerSlave + 1] = NULL;
        } else {
            slaveFiles[0] = NULL;
        }

        int readfd[2];
        if (pipe(readfd) != 0) {
            perror("Error while creating pipe");
            return 1;
        }
        int writefd[2];
        if (pipe(writefd) != 0) {
            perror("Error while creating pipe");
            return 1;
        }
        fds[2 * i] = readfd[0]; // read end of the pipe
        fds[2 * i + 1] = writefd[1]; // write end of the pipe

        int pid = fork();
        if (pid < 0) {
            perror("Error while creating slave");
            return 1;
        } else if (pid == 0) {
            // Child

            close(0); // stdin
            dup(writefd[0]); // read end of where app writes
            close(1); // stdout
            dup(readfd[1]); // write end of where app reads
            close(writefd[0]);
            close(readfd[1]);

            execve("slave", slaveFiles, envp);
        } else {
            // Father
            close(writefd[0]);
            close(readfd[1]);

            slavePids[i] = pid;
        }
    }

    fd_set readfds;

    while (filesProcessed < countFiles) {
        printf("Files processed: %d\n", filesProcessed);

        FD_ZERO(&readfds);
        int nfds = 0;

        for (int i = 0; i < SLAVE_AMOUNT; i++) {
            FD_SET(fds[i*2], &readfds);

            if (fds[i*2] >= nfds) nfds = fds[i*2] + 1;
        }

        if (select(nfds, &readfds, NULL, NULL, NULL) < 0) {
            perror("Error while doing select in app");
            return 1;
        }

        for (int i = 0; i < SLAVE_AMOUNT; i++) {
            if (FD_ISSET(fds[i*2], &readfds)) {
                // There is new hash to add to output
                char buff[128];
                int len = read(fds[i*2], buff, 128);
                if (len < 0) {
                    perror("Error while reading fd in app");
                    return 1;
                }

                printf("Writing 1\n");
                if (write(outputFile, buff, len) < 0) {
                    perror("Error while writing 1 in app");
                    return 1;
                }
                printf("Writing mid\n");
                if ((filesProcessed + 1) < countFiles && write(fds[2*i + 1], files[filesSent], sizeof (char *)) < 0) {
                    perror("Error while writing 2 in app");
                    return 1;
                }
                filesSent++;
                printf("Writing 2\n");
            }
        }

        filesProcessed++;
    }
}