/* COMP2040 - Lab 02, Task 4: private versus explicitly shared memory */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON /* macOS */
#endif

int main(void) {
    int private_value = 10;
    int *shared_value = mmap(NULL, sizeof *shared_value,
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_value == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    *shared_value = 10;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        munmap(shared_value, sizeof *shared_value);
        return 1;
    }
    if (pid == 0) {
        /* TODO 1: Set private_value and *shared_value to 99. */
        (void)private_value;
        _exit(0);
    }

    /* TODO 2: Wait for the child with waitpid() before printing. */
    printf("parent: private=%d shared=%d\n", private_value, *shared_value);
    if (munmap(shared_value, sizeof *shared_value) < 0) {
        perror("munmap");
        return 1;
    }
    return 0;
}
