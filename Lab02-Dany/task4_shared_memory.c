/**
 * @file task4_shared_memory.c
 * @author: Daniela Mendez Ramirez
 * @date: 2026-10-11
 * @version 1.0
 * 
 * @details This file is part of Opeating Systems Lab02. 
 * It demonstrates COMP2040 - Lab 02, Task 4: private versus explicitly shared memory
 * 
 * @note Compilation command:
 *       gcc -Wall -Wextra task4_shared_memory.c -o task4_shared_memory
 *       -Wall: Enables common compiler warnings
 *       -Wextra: Enables extra warning flags.
 *       -o: Specifies the output executable name
 */
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
        private_value = 99;
        *shared_value = 99;
        (void)private_value;
        _exit(0);
    }

    /* TODO 2: Wait for the child with waitpid() before printing. */
    if (waitpid(pid, NULL, 0) < 0) {
        perror("waitpid");
        munmap(shared_value, sizeof *shared_value);
        return 1;
    }

    printf("parent: private=%d shared=%d\n", private_value, *shared_value);

    if (munmap(shared_value, sizeof *shared_value) < 0) {
        perror("munmap");
        return 1;
    }
    return 0;
}
