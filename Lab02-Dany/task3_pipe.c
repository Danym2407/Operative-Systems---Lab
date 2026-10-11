/**
 * @file task3_pipe.c
 * @author: Daniela Mendez Ramirez
 * @date: 2026-10-11
 * @version 1.0
 * 
 * @details This file is part of Opeating Systems Lab02. 
 * It demonstrates a one-way pipe from child to parent
 * 
 * @note Compilation command:
 *       gcc -Wall -Wextra task3_pipe.c -o task3_pipe
 *       -Wall: Enables common compiler warnings
 *       -Wextra: Enables extra warning flags.
 *       -o: Specifies the output executable name
 */
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
        ssize_t bytes_written = write(fd[1], message, strlen(message));
        if (bytes_written < 0) {
            perror("write");
            close(fd[1]);
            _exit(1);
        }
        close(fd[1]);
        _exit(0);
    }

    close(fd[1]); /* Parent does not write. */
    char buffer[128] = {0};
    /* TODO 2: Read from fd[0] into buffer, leaving room for '\0'. */
    ssize_t bytes_read = read(fd[0], buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read");
        close(fd[0]);
        return 1;
    }
    buffer[bytes_read] = '\0'; /* Null-terminate the string. */
    
    /* TODO 3: Print "Parent received: " followed by the message. */
    printf("Parent received: %s", buffer);
    close(fd[0]);

    /* TODO 4: Collect the child with waitpid() and check its exit status. */
    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return 1;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Child process exited with an error\n");
        return 1;
    }
    return 0;
}
