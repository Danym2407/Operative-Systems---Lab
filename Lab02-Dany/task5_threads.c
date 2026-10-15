/**
 * @file task5_threads.c
 * @author: Daniela Mendez Ramirez
 * @date: 2026-10-11
 * @version 1.0
 * 
 * @details This file is part of Opeating Systems Lab02. 
 * It demonstrates COMP2040 - Lab 02, Task 5: one pthread per output row
 * 
 * @note Compilation command:
 *       gcc -Wall -Wextra task5_threads.c -o task5_threads
 *       -Wall: Enables common compiler warnings
 *       -Wextra: Enables extra warning flags.
 *       -o: Specifies the output executable name
 */

 #define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <stdio.h>

enum { N = 2 };
static const int a[N][N] = {{1, 2}, {3, 4}};
static const int b[N][N] = {{5, 6}, {7, 8}};
static int result[N][N];

static void *compute_row(void *arg) {
    int row = *(int *)arg;
    /* TODO 1: Compute every result[row][column] using matrix multiplication.
     * Each worker writes only to its own row.
     */

    for (int column = 0; column < N; ++column) {
        int sum = 0;
        for (int k = 0; k < N; ++k) {
            sum += a[row][k] * b[k][column];
        }
        result[row][column] = sum;
    }
    return NULL;
}

int main(void) {
    pthread_t threads[N];
    int rows[N] = {0, 1};

    /* TODO 2: Create two threads with pthread_create(). Pass &rows[i]. */
    for (int i = 0; i < N; ++i) {
        if (pthread_create(&threads[i], NULL, compute_row, &rows[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }
    /* TODO 3: Join both threads with pthread_join(). Check return values. */

    for (int i = 0; i < N; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }
    for (int i = 0; i < N; ++i) {
        printf("%d %d\n", result[i][0], result[i][1]);
    }
    return 0;
}
