/**
 * @file task1_fork.c
 * @author: Daniela Mendez Ramirez
 * @date: 2026-10-11
 * @version 1.0
 * 
 * @details This file is part of Opeating Systems Lab02.
 * It demonstrates basic use of fork() to create a child process, 
 * furthermore it shows how to use waitpid() to wait for the child process to finish and
 * how the it works the private process memory.
 * 
 * @note Compilation command:
 *       gcc -Wall -Wextra task1_fork.c -o task1_fork
 *       -Wall: Enables common compiler warnings
 *       -Wextra: Enables extra warning flags.
 *       -o: Specifies the output executable name
 */

 #define _POSIX_C_SOURCE 200809L

 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/wait.h>
 #include <unistd.h>
 

 int main(){
   int value = 10;

   /* TODO 1: Replace -1 with a call to fork(). */
    pid_t pid = fork();
    
    if (pid< 0){
        perror("fork");
        return 1;
    }
 /*TODO 2: Set value 99 and print this child's PID, PPID and value */
    if (pid==0){
       /*1. Asign 99 a 'local' variable value*/
        value = 99;
        /*2. Print the child's PID, PPID and value, using the appropriate format specifiers just as long as the types match*/
        printf("Child: PID=%ld PPID=%ld value=%d\n", (long)getpid(), (long)getppid(), value);
        /*3. We finish the child process*/
        return 0;
    }
/*TODO 3: Wait for this child using waitpid(pid, ...). Check for errors */

if (waitpid(pid, NULL, 0) < 0) {
        perror("waitpid");
        return 1;
    }


    printf("parent: PID=%ld value=%d\n", (long)getpid(), value);
    return 0;
 }
