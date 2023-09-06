#include <stdio.h>

int main(int argc, char * argv[]) {
    printf("ESCLAVO: Cant args: %d\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("ESCLAVO: Arg %d: %s\n", i, argv[i]);
    }
}