/**
 * @file mutex_race_condition.c
 * @author: Daniela Mendez Ramirez
 * @date: 2026-11-02
 * @version 1.0
 * @brief Demonstrates the use of mutexes to prevent race conditions in a multithreaded environment.
 * @details This code demonstrates how to use mutexes to prevent race conditions when multiple threads access and modify shared data. It includes creating threads, using mutexes for synchronization, and joining threads.
 * @note Compilation command:
 *       gcc -Wall -Wextra -pthread mutex_race_condition.c -o mutex_race_condition
 *       -Wall: Enables common compiler warnings
 *       -Wextra: Enables extra warning flags.
 *       -o: Specifies the output executable name
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h> /* Library for pthreads */

 /* Structure Definitions */
 typedef struct{
    int *balance; /* Pointer to the shared balance variable */
    pthread_mutex_t *mutex; /* Pointer to the mutex for synchronization */
 }SharedData;


 /* Prototype Functions */
void *modify_balance(void *arg); /* Function to be executed by the thread */


 /* Main Function */
 int main(void){

    int balance = 0; /* Shared variable */
    pthread_mutex_t mutex; /* Mutex for synchronizing access to the shared balance variable */
    pthread_t first_thread, second_thread; /* Thread identifiers */

    /* Create a structure to hold the shared data */
    SharedData shared_data;
    shared_data.balance = &balance; /* Assign the address of the shared balance variable */
    shared_data.mutex = &mutex; /* Assign the address of the mutex */

    
    /* Initialize the mutex */
    /* Understand the pthread_mutex_init(&mutex, NULL); function
     * This function initializes the mutex object pointed to by mutex.
     * The second argument is a pointer to a pthread_mutexattr_t structure that specifies the attributes of the mutex.
     * If NULL is passed, default attributes are used.
     */
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&first_thread, NULL, modify_balance, &shared_data);
    pthread_create(&second_thread, NULL, modify_balance, &shared_data);

    /* Wait for both threads to complete */
    pthread_join(first_thread, NULL);
    pthread_join(second_thread, NULL);
    printf("Final balance: %d\n", *(shared_data.balance));
    
    /* Destroy the mutex */
    pthread_mutex_destroy(&mutex);

    return 0;
 }

 /* Function Definitions */
void *modify_balance(void *arg){

    SharedData *data = (SharedData *)arg; /* Cast the argument to a SharedData pointer */

    for (int i = 0; i < 100000; i++){
        
        /* Lock the mutex before modifying the shared balance variable */
        pthread_mutex_lock(data->mutex);
        (*(data->balance))++; /* Increment the balance */
        /* Unlock the mutex after modifying the shared balance variable */
        pthread_mutex_unlock(data->mutex);
    }
    return NULL;
}
