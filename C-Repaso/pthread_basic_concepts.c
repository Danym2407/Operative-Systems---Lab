/**
 * @file pthread_basic_concepts.c
 * @author: Daniela Mendez Ramirez
 * @date: 2026-11-02
 * @version 1.0
 * @brief Basic concepts of pthreads in C
 * @details This code demonstrates the basic concepts of using pthreads in C. It includes creating threads, passing arguments to threads, and joining threads.
 * @note Compilation command:
 *       gcc -Wall -Wextra -pthread pthread_basic_concepts.c -o pthread_basic_concepts
 *       -Wall: Enables common compiler warnings
 *       -Wextra: Enables extra warning flags.
 *       -o: Specifies the output executable name
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> /* Library for pthreads */

/* Function Prototypes*/
void *worker_function(void *arg); /* Function to be executed by the thread */

/* Main function */
int main(void){

    /* Declare thread variables */
    pthread_t worker_thread;

    /** Understand the pthread_create function 
     * pthread_create(&thread_id, attributes, function, argument);
     * &thread_id: Pointer to the thread identifier
     * attributes: Thread attributes (NULL for default)
     * function: Function to be executed by the thread
     * argument: Argument to be passed to the thread function
     */ 
    pthread_create(&worker_thread, NULL, worker_function, NULL);    
    printf("Hello from main thread!\n");

    /* Use pthread_join to wait for the thread to complete */
    pthread_join(worker_thread, NULL);

    return 0;
}

/* Function definitions*/

void *worker_function(void *arg){
    printf("Hello from worker thread!\n");
    return NULL;
}
