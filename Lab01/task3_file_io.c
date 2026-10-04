/*
 * COMP2040 - Lab 01
 * Task 3: File I/O with open(), write(), read(), and close()
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    const char *filename = "lab01.txt";
    const char *message = "Hello from a COMP2040 system call lab!\n";
    char buffer[128] = {0};

    /*
     * TODO 1: Open filename for writing.
     * - Create it if it does not exist.
     * - Truncate it if it already exists.
     * - Give the owner read/write permission and everyone else read permission.
     *
     * Hint: open() with O_WRONLY | O_CREAT | O_TRUNC and mode 0644.
     */
    int fd = -1;

    /* TODO 2: Check fd for an error, then write message with write(). */

    /* TODO 3: Close the write descriptor with close(). */

    /* TODO 4: Re-open the same file with O_RDONLY. */

    /* TODO 5: Read into buffer with read(). Keep space for a trailing '\0'. */

    /* TODO 6: Close the read descriptor and print what you read. */

    printf("Starter file: complete the TODO sections, then compile and run again.\n");
    printf("Target file: %s\n", filename);
    printf("Message length to write: %zu bytes\n", strlen(message));
    (void)buffer;
    (void)fd;

    return 0;
}
