/**
 * @file task2_fork_exec.c
 * @author: Daniela Mendez Ramirez
 * @date: 2026-10-11
 * @version 1.0
 * 
 * @details This file is part of Opeating Systems Lab02.
 * 
 * @note Compilation command:
 *       gcc -Wall -Wextra task2_fork_exec.c -o task2_fork_exec
 *       -Wall: Enables common compiler warnings
 *       -Wextra: Enables extra warning flags.
 *       -o: Specifies the output executable name
 */

 #define _POSIX_C_SOURCE 200809L
 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/wait.h>
 #include <unistd.h>

 int main(void){
    /*TODO 1: Replace -1 with a call to fork().*/
    pid_t pid = fork();
    if (pid < 0){
        perror("fork");
        return 1;
    }

    if (pid == 0){
        printf("before exec: PID=%ld\n", (long)getpid());
        fflush(stdout);

        /*TODO 2: Use execl() to run ./task2_worker with argument COMP2040
        * First argument: path; second: argv[0]; third: word; final: (char *)NULL.
        */
        execl("./task2_worker", "task2_worker", "COMP2040", (char *)NULL);
        perror("execl"); /* Reached only if exec fails. */
        _exit(127);
    }

    int status = 0;
    /*TODO 3: Use waitpid(pid, &status, 0). Check for errors.*/
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return 1;
    }
    /* TODO 4: If WIFEXITED(status), print WEXITSTATUS(status). */
    if (WIFEXITED(status)) {
        printf("parent: child exited with status %d\n", WEXITSTATUS(status));
    }
    return 0;

 }