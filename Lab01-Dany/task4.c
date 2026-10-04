/**
 * @file task4.c
 * @author Daniela Mendez Ramirez
 * @date 2026-10-04
 * @version 1.0
 *
 * @details This file is part of Operating Systems Lab 01 - Task 4.
 * It demonstrates how to inspect file metadata using the stat() system call
 * to report a file's size in bytes without opening or reading it.
 */

#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    struct stat file_info;

    /*
     * TODO 1: Call stat(filename, &file_info).
     * If stat() returns -1, print an error with perror("stat") and return 1.
     */
    if (stat(filename, &file_info) == -1) {
        perror("stat");
        return 1;
    }

    /*
     * TODO 2: Print the file size using file_info.st_size.
     */
    printf("File: %s\n", filename);
    printf("Size: %lld bytes\n", (long long)file_info.st_size);

    return 0;
}