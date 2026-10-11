/* COMP2040 - Lab 02, Task 1: fork() and private process memory */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int value = 10;

    /* TODO 1: Replace -1 with a call to fork(). */
    pid_t pid = -1;
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* TODO 2: Set value to 99 and print this child's PID, PPID, and value. */
        return 0;
    }

    /* TODO 3: Wait for this child using waitpid(pid, ...). Check for errors. */
    printf("parent: PID=%ld value=%d\n", (long)getpid(), value);
    return 0;
}
