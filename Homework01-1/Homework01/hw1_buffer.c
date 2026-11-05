/* COMP2040 Homework 01, bonus: bounded buffer with a mutex and condition variables
 *
 * Build: gcc -std=c11 -Wall -Wextra -pthread hw1_buffer.c -o hw1_buffer
 * Run:   ./hw1_buffer        (should print OK)
 */
#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#define N      3        // buffer size
#define ITEMS  20000    // items per producer

int buffer[N];
int in;      // next slot to fill
int out;     // next slot to empty
int count;   // items in the buffer
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void put(int item) {
    // TODO 1: lock m, wait on not_full while the buffer is full,
    // insert the item, signal not_empty, unlock m
    buffer[in] = item;
    in = (in + 1) % N;
    count++;
}

int get(void) {
    // TODO 2: lock m, wait on not_empty while the buffer is empty,
    // remove the item, signal not_full, unlock m
    int item = buffer[out];
    out = (out + 1) % N;
    count--;
    return item;
}

// test code below - do not change

void *producer(void *arg) {
    (void)arg;
    for (int i = 1; i <= ITEMS; i++) {
        put(i);
    }
    return NULL;
}

void *consumer(void *arg) {
    long long *sum = arg;
    for (int i = 0; i < ITEMS; i++) {
        *sum += get();
    }
    return NULL;
}

void watchdog(int sig) {
    (void)sig;
    const char msg[] = "\nWatchdog: no result after 10 seconds - "
                       "a thread is probably waiting forever. Exiting.\n";
    write(STDERR_FILENO, msg, sizeof msg - 1);
    _exit(3);
}

int main(void) {
    signal(SIGALRM, watchdog);
    alarm(10);

    pthread_t p1, p2, c1, c2;
    long long sum1 = 0, sum2 = 0;
    pthread_create(&p1, NULL, producer, NULL);
    pthread_create(&p2, NULL, producer, NULL);
    pthread_create(&c1, NULL, consumer, &sum1);
    pthread_create(&c2, NULL, consumer, &sum2);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(c1, NULL);
    pthread_join(c2, NULL);

    long long expected = 2LL * ITEMS * (ITEMS + 1) / 2;
    long long consumed = sum1 + sum2;
    printf("Produced sum: %lld\n", expected);
    printf("Consumed sum: %lld\n", consumed);
    printf("Items left in buffer: %d\n", count);
    printf("%s\n", (consumed == expected && count == 0) ? "OK" : "WRONG");
    return (consumed == expected && count == 0) ? 0 : 1;
}
