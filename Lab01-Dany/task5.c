/**
 * @file task5_permissions.c
 * @author Daniela Mendez Ramirez
 * @date 2026-10-04
 * @version 1.0
 *
 * @details This file is part of Operating Systems Lab 01 - Task 5.
 * It demonstrates how to inspect file permission bits from st_mode using stat()
 * and translate them into a human-readable 9-character string (e.g., rw-r--r--).
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
    if (stat(filename, &file_info) == -1) {
        perror("stat");
        return 1;
    }

    /*
     * TODO 2: Build and print the 9-character permission string using permission_char.
     * Evaluates Owner (USR), Group (GRP), and Others (OTH).
     */
    char perms[10];

    // Owner permissions
    perms[0] = permission_char(file_info.st_mode, S_IRUSR, 'r');
    perms[1] = permission_char(file_info.st_mode, S_IWUSR, 'w');
    perms[2] = permission_char(file_info.st_mode, S_IXUSR, 'x');

    // Group permissions
    perms[3] = permission_char(file_info.st_mode, S_IRGRP, 'r');
    perms[4] = permission_char(file_info.st_mode, S_IWGRP, 'w');
    perms[5] = permission_char(file_info.st_mode, S_IXGRP, 'x');

    // Others permissions
    perms[6] = permission_char(file_info.st_mode, S_IROTH, 'r');
    perms[7] = permission_char(file_info.st_mode, S_IWOTH, 'w');
    perms[8] = permission_char(file_info.st_mode, S_IXOTH, 'x');

    perms[9] = '\0'; // Null-terminator

    printf("File: %s\n", filename);
    printf("Permissions: %s\n", perms);

    return 0;
}