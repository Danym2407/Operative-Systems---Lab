/*
 * COMP2040 - Lab 01
 * Task 5: File permissions with stat()
 */

#include <stdio.h>
#include <sys/stat.h>

static char permission_char(mode_t mode, mode_t mask, char when_set) {
    return (mode & mask) ? when_set : '-';
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    struct stat file_info;

    /* TODO 1: Call stat(filename, &file_info) and handle errors. */

    /*
     * TODO 2: Use file_info.st_mode with the permission masks below to print
     * a nine-character permission string such as rw-r--r--.
     *
     * Owner: S_IRUSR S_IWUSR S_IXUSR
     * Group: S_IRGRP S_IWGRP S_IXGRP
     * Other: S_IROTH S_IWOTH S_IXOTH
     *
     * The helper permission_char() is provided for you.
     */

    printf("Starter file: report permissions for %s.\n", filename);
    (void)file_info;
    (void)permission_char;

    return 0;
}
