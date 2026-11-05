/**
 * @file hw1_buffer.c
 * @author: Daniela Mendez Ramirez
 * @date: 2026-11-04
 * @version 1.0
 *
 * @details This file implements a bounded buffer for COMP2040 Homework 01.
 *          Two producer threads add items and two consumer threads remove
 *          items. A mutex protects the shared buffer, while condition
 *          variables coordinate threads when the buffer is full or empty.
 *
 * @note Compilation command:
 *       gcc -std=c11 -Wall -Wextra -pthread hw1_buffer.c -o hw1_buffer
 *       -std=c11: Uses the C11 language standard.
 *       -Wall: Enables common compiler warnings.
 *       -Wextra: Enables additional compiler warnings.
 *       -pthread: Enables POSIX thread support.
 *       -o: Sets the name of the output executable.
 *
 * @note Run command:
 *       ./hw1_buffer
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 3
#define ITEMS_PER_PRODUCER 20000

/**
 * @struct BoundedBuffer
 * @brief Stores the shared buffer data and synchronization objects.
 *
 * @param items Array that stores the buffer values.
 * @param next_in Index where the next item will be inserted.
 * @param next_out Index where the next item will be removed.
 * @param count Number of items currently stored in the buffer.
 * @param mutex Protects all shared buffer data.
 * @param not_full Signals that the buffer has an available slot.
 * @param not_empty Signals that the buffer contains an item.
 */
typedef struct {
    int items[BUFFER_SIZE];
    int next_in;
    int next_out;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} BoundedBuffer;

/**
 * @struct ConsumerArguments
 * @brief Stores the data required by each consumer thread.
 *
 * @param buffer Pointer to the shared bounded buffer.
 * @param sum Pointer to the consumer's private result sum.
 */
typedef struct {
    BoundedBuffer *buffer;
    long long *sum;
} ConsumerArguments;

/* Function prototypes */
int initialize_buffer(BoundedBuffer *buffer);
int destroy_buffer(BoundedBuffer *buffer);
int put(BoundedBuffer *buffer, int item);
int get(BoundedBuffer *buffer, int *item);
void *producer(void *arguments);
void *consumer(void *arguments);
void watchdog(int signal_number);
void print_thread_error(const char *operation, int error_code);

int main(void)
{
    BoundedBuffer shared_buffer = {0};
    ConsumerArguments consumer_one_arguments;
    ConsumerArguments consumer_two_arguments;

    pthread_t producer_one;
    pthread_t producer_two;
    pthread_t consumer_one;
    pthread_t consumer_two;

    long long consumer_one_sum = 0;
    long long consumer_two_sum = 0;
    long long expected_sum;
    long long consumed_sum;

    int error_code;

    if (initialize_buffer(&shared_buffer) != 0) {
        fprintf(stderr, "Error: Could not initialize the bounded buffer.\n");
        return EXIT_FAILURE;
    }

    consumer_one_arguments.buffer = &shared_buffer;
    consumer_one_arguments.sum = &consumer_one_sum;

    consumer_two_arguments.buffer = &shared_buffer;
    consumer_two_arguments.sum = &consumer_two_sum;

    if (signal(SIGALRM, watchdog) == SIG_ERR) {
        perror("signal");
        destroy_buffer(&shared_buffer);
        return EXIT_FAILURE;
    }

    alarm(10);

    error_code = pthread_create(&producer_one, NULL, producer, &shared_buffer);
    if (error_code != 0) {
        print_thread_error("pthread_create producer one", error_code);
        alarm(0);
        destroy_buffer(&shared_buffer);
        return EXIT_FAILURE;
    }

    error_code = pthread_create(&producer_two, NULL, producer, &shared_buffer);
    if (error_code != 0) {
        print_thread_error("pthread_create producer two", error_code);
        fprintf(stderr, "The first producer may still be running.\n");
        alarm(0);
        pthread_join(producer_one, NULL);
        destroy_buffer(&shared_buffer);
        return EXIT_FAILURE;
    }

    error_code = pthread_create(&consumer_one, NULL, consumer,
                                &consumer_one_arguments);
    if (error_code != 0) {
        print_thread_error("pthread_create consumer one", error_code);
        fprintf(stderr, "The producers may be waiting for buffer space.\n");
        alarm(0);
        return EXIT_FAILURE;
    }

    error_code = pthread_create(&consumer_two, NULL, consumer,
                                &consumer_two_arguments);
    if (error_code != 0) {
        print_thread_error("pthread_create consumer two", error_code);
        alarm(0);
        return EXIT_FAILURE;
    }

    pthread_join(producer_one, NULL);
    pthread_join(producer_two, NULL);
    pthread_join(consumer_one, NULL);
    pthread_join(consumer_two, NULL);

    alarm(0);

    expected_sum = 2LL * ITEMS_PER_PRODUCER *
                   (ITEMS_PER_PRODUCER + 1) / 2;
    consumed_sum = consumer_one_sum + consumer_two_sum;

    printf("Produced sum: %lld\n", expected_sum);
    printf("Consumed sum: %lld\n", consumed_sum);
    printf("Items left in buffer: %d\n", shared_buffer.count);
    printf("%s\n", (consumed_sum == expected_sum &&
                    shared_buffer.count == 0) ? "OK" : "WRONG");

    if (destroy_buffer(&shared_buffer) != 0) {
        fprintf(stderr, "Warning: Could not destroy all synchronization objects.\n");
        return EXIT_FAILURE;
    }

    return (consumed_sum == expected_sum && shared_buffer.count == 0)
               ? EXIT_SUCCESS
               : EXIT_FAILURE;
}

/**
 * @brief Initializes the buffer and its synchronization objects.
 * @param buffer Pointer to the buffer to initialize.
 * @return 0 on success, or a non-zero value if initialization fails.
 */
int initialize_buffer(BoundedBuffer *buffer)
{
    int error_code;

    buffer->next_in = 0;
    buffer->next_out = 0;
    buffer->count = 0;

    error_code = pthread_mutex_init(&buffer->mutex, NULL);
    if (error_code != 0) {
        print_thread_error("pthread_mutex_init", error_code);
        return error_code;
    }

    error_code = pthread_cond_init(&buffer->not_full, NULL);
    if (error_code != 0) {
        print_thread_error("pthread_cond_init not_full", error_code);
        pthread_mutex_destroy(&buffer->mutex);
        return error_code;
    }

    error_code = pthread_cond_init(&buffer->not_empty, NULL);
    if (error_code != 0) {
        print_thread_error("pthread_cond_init not_empty", error_code);
        pthread_cond_destroy(&buffer->not_full);
        pthread_mutex_destroy(&buffer->mutex);
        return error_code;
    }

    return 0;
}

/**
 * @brief Destroys the buffer's condition variables and mutex.
 * @param buffer Pointer to the buffer to clean up.
 * @return 0 if all objects are destroyed, or a non-zero error code.
 */
int destroy_buffer(BoundedBuffer *buffer)
{
    int first_error = 0;
    int error_code;

    error_code = pthread_cond_destroy(&buffer->not_empty);
    if (error_code != 0 && first_error == 0) {
        first_error = error_code;
    }

    error_code = pthread_cond_destroy(&buffer->not_full);
    if (error_code != 0 && first_error == 0) {
        first_error = error_code;
    }

    error_code = pthread_mutex_destroy(&buffer->mutex);
    if (error_code != 0 && first_error == 0) {
        first_error = error_code;
    }

    return first_error;
}

/**
 * @brief Adds one item to the bounded buffer.
 * @param buffer Pointer to the shared buffer.
 * @param item Value to insert.
 * @return 0 on success, or a non-zero pthread error code.
 */
int put(BoundedBuffer *buffer, int item)
{
    int error_code;

    error_code = pthread_mutex_lock(&buffer->mutex);
    if (error_code != 0) {
        return error_code;
    }

    while (buffer->count == BUFFER_SIZE) {
        error_code = pthread_cond_wait(&buffer->not_full, &buffer->mutex);
        if (error_code != 0) {
            pthread_mutex_unlock(&buffer->mutex);
            return error_code;
        }
    }

    buffer->items[buffer->next_in] = item;
    buffer->next_in = (buffer->next_in + 1) % BUFFER_SIZE;
    buffer->count++;

    error_code = pthread_cond_signal(&buffer->not_empty);
    if (error_code != 0) {
        pthread_mutex_unlock(&buffer->mutex);
        return error_code;
    }

    return pthread_mutex_unlock(&buffer->mutex);
}

/**
 * @brief Removes one item from the bounded buffer.
 * @param buffer Pointer to the shared buffer.
 * @param item Address where the removed value will be stored.
 * @return 0 on success, or a non-zero pthread error code.
 */
int get(BoundedBuffer *buffer, int *item)
{
    int error_code;

    error_code = pthread_mutex_lock(&buffer->mutex);
    if (error_code != 0) {
        return error_code;
    }

    while (buffer->count == 0) {
        error_code = pthread_cond_wait(&buffer->not_empty, &buffer->mutex);
        if (error_code != 0) {
            pthread_mutex_unlock(&buffer->mutex);
            return error_code;
        }
    }

    *item = buffer->items[buffer->next_out];
    buffer->next_out = (buffer->next_out + 1) % BUFFER_SIZE;
    buffer->count--;

    error_code = pthread_cond_signal(&buffer->not_full);
    if (error_code != 0) {
        pthread_mutex_unlock(&buffer->mutex);
        return error_code;
    }

    return pthread_mutex_unlock(&buffer->mutex);
}

/**
 * @brief Produces a fixed number of items and inserts them into the buffer.
 * @param arguments Pointer to the shared buffer.
 * @return NULL when the producer has finished.
 */
void *producer(void *arguments)
{
    BoundedBuffer *buffer = arguments;
    int item_number;
    int error_code;

    for (item_number = 1; item_number <= ITEMS_PER_PRODUCER; item_number++) {
        error_code = put(buffer, item_number);
        if (error_code != 0) {
            print_thread_error("put", error_code);
            return NULL;
        }
    }

    return NULL;
}

/**
 * @brief Removes items from the buffer and adds them to a private sum.
 * @param arguments Pointer to the consumer's arguments.
 * @return NULL when the consumer has finished.
 */
void *consumer(void *arguments)
{
    ConsumerArguments *consumer_arguments = arguments;
    int item_number;
    int item;
    int error_code;

    for (item_number = 0; item_number < ITEMS_PER_PRODUCER; item_number++) {
        error_code = get(consumer_arguments->buffer, &item);
        if (error_code != 0) {
            print_thread_error("get", error_code);
            return NULL;
        }

        *consumer_arguments->sum += item;
    }

    return NULL;
}

/**
 * @brief Stops the program if the threads do not finish in time.
 * @param signal_number Signal received by the handler.
 * @return This function does not return.
 */
void watchdog(int signal_number)
{
    const char message[] =
        "\nWatchdog: no result after 10 seconds - "
        "a thread may be waiting forever. Exiting.\n";

    (void)signal_number;
    write(STDERR_FILENO, message, sizeof(message) - 1);
    _exit(3);
}

/**
 * @brief Prints a readable message for a pthread error code.
 * @param operation Name of the operation that failed.
 * @param error_code Error code returned by the pthread function.
 * @return This function does not return a value.
 */
void print_thread_error(const char *operation, int error_code)
{
    fprintf(stderr, "%s: %s\n", operation, strerror(error_code));
}
