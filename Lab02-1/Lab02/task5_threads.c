/* COMP2040 - Lab 02, Task 5: one pthread per output row */
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
    (void)row;
    (void)a;
    (void)b;
    return NULL;
}

int main(void) {
    pthread_t threads[N];
    int rows[N] = {0, 1};

    /* TODO 2: Create two threads with pthread_create(). Pass &rows[i]. */
    /* TODO 3: Join both threads with pthread_join(). Check return values. */

    for (int i = 0; i < N; ++i) {
        printf("%d %d\n", result[i][0], result[i][1]);
    }
    (void)threads;
    (void)rows;
    (void)compute_row;
    return 0;
}
