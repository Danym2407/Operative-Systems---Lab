/*
 * COMP2040 - Lab 01
 * Task 4: File size with stat()
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
     *
     * TODO 2: Print the file size using file_info.st_size.
     */

    printf("Starter file: use stat() to report the size of %s.\n", filename);
    (void)file_info;

    return 0;
}
