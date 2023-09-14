#include <app.h>

int main(int argc, char * argv[], char * envp[]) {
    
    // Disable buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    char * files[argc - 1];
    int countFiles = 0;

    struct stat fileData;
    for (int i = 1; i < argc; i++) {
        stat(argv[i], &fileData);
        
        // We check if is a common file
        if (S_ISREG(fileData.st_mode)) {
            files[countFiles++] = argv[i];
        }
    }

    /* printf("Cant Files: %d\n", countFiles);
    for (int i = 0; i < countFiles; i++) {
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

    Slave slaves[SLAVE_AMOUNT];

    for (int i = 0 ; i < SLAVE_AMOUNT ; i++) {

        int readfds[2];
        int writefds[2];

        createPipe(readfds);
        createPipe(writefds);

        int pid = createFork();
        if (pid == 0) {
            // Child
            close(STDIN_FILENO);
            dup(writefds[READ_END]); // read end of where app writes
            close(STDOUT_FILENO);
            dup(readfds[WRITE_END]); // write end of where app reads
            close(writefds[READ_END]);
            close(readfds[WRITE_END]);

            execl("slave", "slave", (char *) NULL);
        } else {
            // Father
            close(writefds[READ_END]);
            close(readfds[WRITE_END]);

            slaves[i].readfd = readfds[READ_END];
            slaves[i].writefd = writefds[WRITE_END];
            slaves[i].pid = pid;
            slaves[i].filesProcessed = 0;
            slaves[i].filesSent = 0;

            if (filesSent < filesToSlaves) { // We check this in case filesPerSlave was 0
                for (int j = 0; j < filesPerSlave; j++) {
                    write(slaves[i].writefd, files[filesSent++], sizeof (char *));
                    slaves[i].filesSent++;
                }
            }
        }
    }

    fd_set readfds;

    while (filesProcessed < countFiles) {
        printf("Files processed: %d\n", filesProcessed);

        FD_ZERO(&readfds);
        int nfds = 0;

        for (int i = 0; i < SLAVE_AMOUNT; i++) {
            FD_SET(slaves[i].readfd, &readfds);

            if (slaves[i].readfd >= nfds) nfds = slaves[i].readfd + 1;
        }

        rSelect(nfds, &readfds);

        for (int i = 0; i < SLAVE_AMOUNT; i++) {
            if (FD_ISSET(slaves[i].readfd, &readfds)) {
                // There is new hash to add to output
                char buff[1024];
                int len = readFd(slaves[i].readfd, buff, 1023);
                int fp = analyzeRead(buff, len);
                filesProcessed += fp;
                slaves[i].filesProcessed += fp;

                writeFd(outputFile, buff, len);

                if (filesSent < countFiles && slaves[i].filesSent <= slaves[i].filesProcessed) {
                    writeFd(slaves[i].writefd, files[filesSent++], sizeof (char *));
                    slaves[i].filesSent++;
                }
            }
        }
    }

    for (int i = 0; i < SLAVE_AMOUNT; i++) {
        close(slaves[i].readfd);
        close(slaves[i].writefd);
    }

    close(outputFile);
}

int analyzeRead(char * buff, int len) {
    int files = 0;
    for (int i = 0; i < len; i++) {
        if (buff[i] == '\n')
            files++;
    }
    return files;
}