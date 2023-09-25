// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

    if (countFiles < 1) {
        puts("No files to process");
        return 0;
    }

    shmAdt shm = newShm("shmTpe");
    puts("shmTpe");

    createFifo("tpeFifo", 0666);

    sleep(2); // Wait for view to appear

    int outputFile = openFile("output.txt", O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

    int filesToSlaves = countFiles * 0.1;
    if (filesToSlaves == 0) filesToSlaves = 1;

    int slaveAmount = filesToSlaves > SLAVE_MAX_AMOUNT ? SLAVE_MAX_AMOUNT : filesToSlaves;

    int filesPerSlave = filesToSlaves / slaveAmount; 
    if (filesPerSlave == 0) filesPerSlave = 1;

    int filesSent = 0, filesProcessed = 0;

    Slave slaves[slaveAmount];

    for (int i = 0 ; i < slaveAmount ; i++) {

        int readfds[FD_AMOUNT];
        int writefds[FD_AMOUNT];

        createPipe(readfds);
        createPipe(writefds);

        int pid = createFork();
        if (pid == 0) {
            // Child

            // Close fds of other slaves
            for (int j = 0 ; j < i ; j++) {
                close(slaves[j].readfd);
                close(slaves[j].writefd);
            }

            // Redirect stdin and stdout to pipes
            close(STDIN_FILENO);
            dup(writefds[READ_END]); // read end of where app writes
            close(STDOUT_FILENO);
            dup(readfds[WRITE_END]); // write end of where app reads

            close(readfds[READ_END]);
            close(readfds[WRITE_END]);
            close(writefds[READ_END]);
            close(writefds[WRITE_END]);

            execl("slave", "slave", (char *) NULL);
            perror("Error while creating slave");
            exit(EXIT_FAILURE);
        } else {
            // Father
            close(writefds[READ_END]);
            close(readfds[WRITE_END]);

            slaves[i].readfd = readfds[READ_END];
            slaves[i].writefd = writefds[WRITE_END];
            slaves[i].pid = pid;
            slaves[i].filesProcessed = 0;
            slaves[i].filesSent = 0;

            if (filesSent < filesToSlaves) {
                for (int j = 0; j < filesPerSlave; j++) {
                    write(slaves[i].writefd, files[filesSent], strlen(files[filesSent]) + 1);
                    filesSent++;
                    slaves[i].filesSent++;
                }
            }
        }
    }

    fd_set readfds;

    while (filesProcessed < countFiles) {

        FD_ZERO(&readfds);
        int nfds = 0;

        for (int i = 0; i < slaveAmount; i++) {
            FD_SET(slaves[i].readfd, &readfds);

            if (slaves[i].readfd >= nfds) nfds = slaves[i].readfd + 1;
        }

        rSelect(nfds, &readfds);

        for (int i = 0; i < slaveAmount; i++) {
            if (FD_ISSET(slaves[i].readfd, &readfds)) {
                // There is new hash to add to output
                char buff[BUFF_SIZE];
                int len = readFd(slaves[i].readfd, buff, BUFF_SIZE - 1);
                int fp = analyzeRead(buff, len);
                filesProcessed += fp;
                slaves[i].filesProcessed += fp;
                buff[len] = 0;

                writeFd(outputFile, buff, len);
                writeShm(shm, buff, len);

                if (filesSent < countFiles && slaves[i].filesSent <= slaves[i].filesProcessed) {
                    writeFd(slaves[i].writefd, files[filesSent], strlen(files[filesSent]) + 1);
                    filesSent++;
                    slaves[i].filesSent++;
                }
            }
        }
    }

    writeShm(shm, "", 0); // Tell view that there is no more data to read

    for (int i = 0; i < slaveAmount; i++) {
        close(slaves[i].readfd);
        close(slaves[i].writefd);
    }

    close(outputFile);
    closeShm(shm);
    return 0;
}

int analyzeRead(char * buff, int len) {
    int files = 0;
    for (int i = 0; i < len; i++) {
        if (buff[i] == '\n')
            files++;
    }
    return files;
}