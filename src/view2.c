// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <view.h>

int main(int argc, char * argv[]) {
    
    // Disable buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    int fifo = open("tpeFifo", O_RDONLY);

    char buff[SHM_BUFF_SIZE];

    while (TRUE) {
        read(fifo, buff, SHM_BUFF_SIZE - 1);
        if (buff[0] == '\0') break;
        printf("%s", buff);
    }

    return 0;
}