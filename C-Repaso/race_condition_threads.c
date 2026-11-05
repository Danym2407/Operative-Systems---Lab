/**
 * @file race_condition_threads.c
 * @author: Daniela Mendez Ramirez
 * @date: 2026-11-02
 * @version 1.0
 * @brief Demonstrates a race condition in a multithreaded environment.
 * @details This code demonstrates a race condition that can occur when multiple threads access and modify shared
 * @note Compilation command:
 *       gcc -Wall -Wextra -pthread race_condition_threads.c -o race_condition_threads
 *       -Wall: Enables common compiler warnings
 *       -Wextra: Enables extra warning flags.
 *       -o: Specifies the output executable name
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h> /* Library for pthreads */

 /* Function Prototypes */
 void *modify_balance(void *arg); /* Function to be executed by the thread */

 /* Main Function */
 int main(void){

    int balance = 0; /* Shared variable */
    pthread_t first_thread, second_thread; /* Thread identifiers */

    /* Create the threads */
    pthread_create(&first_thread, NULL, modify_balance, &balance);
    pthread_create(&second_thread, NULL, modify_balance, &balance);
    /* Wait for the threads to complete */
    pthread_join(first_thread, NULL);
    pthread_join(second_thread, NULL);
    printf("Final balance: %d\n", balance);
    return 0;
 }

 /* Function Definitions */
void *modify_balance(void *arg){
    int *balance_ptr = (int *)arg; /* Cast the argument to an integer pointer */
    for (int i = 0; i < 100000; i++){
        (*balance_ptr)++; /* Increment the balance */
    }
    return NULL;
}
