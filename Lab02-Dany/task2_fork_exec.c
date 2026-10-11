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

 #dfine _POSIX_C_SOURCE_200809L
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
        perror("execl"); /* Reached only if exec fails. */
        _exit(127);
    }

    int status = 0;
    /*TODO 3: Use waitpid(pid, &status, 0). Check for errors.*/
    /* TODO 4: If WIFEXITED(status), print WEXITSTATUS(status). */
    puts("Starter file: collect and report the child's real exit status.");
    (void)status;
    return 0;

 }