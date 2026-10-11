/* COMP2040 - Lab 02, Task 3: one-way pipe from child to parent */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    const char *message = "Hello from the child\n";
    int fd[2];
    if (pipe(fd) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return 1;
    }

    if (pid == 0) {
        close(fd[0]); /* Child does not read. */
        /* TODO 1: Write message to fd[1]. Check the number of bytes written. */
        close(fd[1]);
        _exit(0);
    }

    close(fd[1]); /* Parent does not write. */
    char buffer[128] = {0};
    /* TODO 2: Read from fd[0] into buffer, leaving room for '\0'. */
    /* TODO 3: Print "Parent received: " followed by the message. */
    close(fd[0]);

    /* TODO 4: Collect the child with waitpid() and check its exit status. */
    puts("Starter file: send and receive the message.");
    (void)message;
    (void)buffer;
    return 0;
}
