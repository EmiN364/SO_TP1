#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

#define SLAVE_AMOUNT 5

int stat(const char *restrict pathname, struct stat *restrict statbuf);
pid_t fork(void);
int execve(const char *pathname, char *const argv[], char *const envp[]);

int main(int argc, char * argv[], char * envp[]) {
    printf("Cant args: %d\n", argc);

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
    for (int i = 0; i < countFiles; i++) {
        printf("File %d: %s\n", i, files[i]);
    }

    if (countFiles < 1) {
        puts("No files to process");
        return 0;
    }

    int filesToSlaves = countFiles * 0.1;
    if (filesToSlaves == 0) filesToSlaves = 1;
    // printf("filesToSlaves: %d\n", filesToSlaves);

    int filesPerSlave = filesToSlaves / SLAVE_AMOUNT; 
    if (filesPerSlave == 0) filesPerSlave = 1;
    // printf("filesPerSlave: %d\n", filesPerSlave);
    // TODO: Ver caso si es 0

    int filesSent = 0;

    int slavePids[SLAVE_AMOUNT];

    for (int i = 0 ; i < SLAVE_AMOUNT ; i++) {
        char * slaveFiles[filesPerSlave + 1];
        if (filesSent < filesToSlaves) { // We check this in case filesPerSlave was 0
            for (int j = 0; j < filesPerSlave; j++)
                slaveFiles[j] = files[filesSent++];

            slaveFiles[filesPerSlave] = NULL;
        } else {
            slaveFiles[0] = NULL;
        }

        int pid = fork();
        if (pid < 0) {
            perror("Error while creating slave");
            return 1;
        } else if (pid == 0) {
            // Child
            execve("slave", slaveFiles, envp);
        } else {
            // Father
            slavePids[i] = pid;
        }
    }
}