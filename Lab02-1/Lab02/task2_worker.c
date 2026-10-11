/* COMP2040 - Lab 02, Task 2: program run after exec */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <word>\n", argv[0]);
        return 1;
    }
    printf("worker: PID=%ld word=%s\n", (long)getpid(), argv[1]);
    return 0;
}
