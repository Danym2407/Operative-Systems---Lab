/**
 * @file task3.c
 * @author Daniela Mendez Ramirez
 * @date 2026-10-04
 * @version 1.0
 *
 * @details This program is part of Operating Systems Lab 01 - Task 3.
 * It demonstrates direct low-level file I/O operations through POSIX system calls:
 * open(), write(), read(), and close(). Unlike high-level library functions (such as
 * fopen or fprintf), these system calls interact directly with the operating system
 * kernel using integer file descriptors.
 *
 * Header breakdown:
 * - <fcntl.h>: Provides file control options and flags (e.g., O_WRONLY, O_CREAT).
 * - <unistd.h>: Provides access to POSIX operating system API (write, read, close).
 * - <stdio.h>: Provides standard diagnostic printing (perror, printf).
 * - <string.h>: Provides string inspection utilities (strlen).
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    const char *filename = "lab01.txt";
    const char *message = "Hello from a COMP2040 system call lab!\n";
    
    /* Buffer to hold bytes retrieved from the file.
     * Initialized to zero to prevent uninitialized memory issues. */
    char buffer[128] = {0};

    /*
     * STEP 1: Request the kernel to open or create the file for writing.
     * Flags:
     *   - O_WRONLY: Open for writing only.
     *   - O_CREAT:  Create the file if it does not already exist.
     *   - O_TRUNC:  If the file already exists, wipe its content (length becomes 0).
     * Mode 0644 (Octal permission bitmask):
     *   - Owner (6 = 4+2): Read and Write permissions.
     *   - Group (4): Read-only permission.
     *   - Others (4): Read-only permission.
     */
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    /* Check if open() failed. System calls return -1 on error and set errno. */
    if (fd == -1) {
        perror("Error opening file for writing");
        return 1;
    }

    /*
     * STEP 2: Transfer raw bytes from the user-space buffer into the file.
     * write() returns the signed number of bytes successfully written (ssize_t).
     */
    ssize_t bytes_written = write(fd, message, strlen(message));
    if (bytes_written == -1) {
        perror("Error writing to file");
        close(fd); /* Clean up allocated descriptor before exiting */
        return 1;
    }

    printf("Wrote %zd bytes to %s\n", bytes_written, filename);

    /*
     * STEP 3: Close the file descriptor after writing.
     * Closing flushes kernel structures and frees the descriptor table entry.
     */
    if (close(fd) == -1) {
        perror("Error closing file after writing");
        return 1;
    }

    /*
     * STEP 4: Re-open the file in read-only mode (O_RDONLY).
     * The kernel assigns a new file descriptor index.
     */
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return 1;
    }

    /*
     * STEP 5: Read raw bytes from the file into the user-space buffer.
     * We pass 'sizeof(buffer) - 1' (127 bytes) so that we always leave room
     * for the null terminator byte ('\0') at the end.
     */
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
        perror("Error reading from file");
        close(fd);
        return 1;
    }

    /* Explicitly null-terminate the received stream so printf treats it as a string */
    buffer[bytes_read] = '\0';

    /*
     * STEP 6: Close the read descriptor and present the retrieved content.
     */
    if (close(fd) == -1) {
        perror("Error closing file after reading");
        return 1;
    }

    printf("Read %zd bytes from %s:\n%s", bytes_read, filename, buffer);

    return 0;
}