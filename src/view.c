// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shmAdt.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[]) {
    // Disable buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    char shmName[SHM_NAME_SIZE];
    if (argc > 1) {
        strncpy(shmName, argv[1], SHM_NAME_SIZE - 1);
    } else {
        scanf("%9s", shmName);
    }

    shmAdt shm = connectShm(shmName);

    char buff[1024];

    while (1) {
        readShm(shm, buff);
        if (buff[0] == '\0') break;
        printf("%s", buff);
    }

    deleteShm(shm);
    return 0;
}