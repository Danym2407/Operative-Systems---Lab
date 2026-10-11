/**
 * @file task2_fork_worker.c
 * @author: Daniela Mendez Ramirez
 * @date: 2026-10-11
 * @version 1.0
 * 
 * @details This file is part of Opeating Systems Lab02.
 * 
 * 
 * @note Compilation command:
 *       gcc -Wall -Wextra task1_fork.c -o task1_fork
 *       -Wall: Enables common compiler warnings
 *       -Wextra: Enables extra warning flags.
 *       -o: Specifies the output executable name
 */

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
