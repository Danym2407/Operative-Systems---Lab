/* COMP2040 - Lab 02 warm-up: arrays and pointers in C
 *
 * Five tiny examples, from easy to hard. Each one is C you will see in Lab 02.
 * Read an example, guess what it prints, then run it.
 *
 * Build: gcc -std=c11 -Wall -Wextra c_concepts_arrays_pointers.c -o c_concepts
 * Run:   ./c_concepts        (all five examples)
 *        ./c_concepts 3      (only example 3)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Example 1 (easy): an array holds several values; the first index is 0. */
static void example1(void) {
    int scores[3] = {70, 85, 90};        /* scores[0], scores[1], scores[2] */

    int sum = 0;
    for (int i = 0; i < 3; i++) {        /* i = 0, 1, 2 */
        sum = sum + scores[i];
    }

    printf("Example 1: arrays           (Lab 02: int fd[2];)\n");
    printf("  first = scores[0] = %d, last = scores[2] = %d\n", scores[0], scores[2]);
    printf("  sum of all three = %d\n", sum);
}

/* Example 2: a pointer stores the ADDRESS of a variable.
 *   &value  = "address of value"
 *   *p      = "the value that p points to" */
static void example2(void) {
    int value = 10;
    int *p = &value;                     /* p points to value */

    printf("Example 2: pointers         (Lab 02: *shared_value = 99;)\n");
    printf("  *p = %d  (p points to value)\n", *p);
    *p = 99;                             /* change value THROUGH the pointer */
    printf("  after *p = 99, value = %d\n", value);
}

/* Example 3: give a function the ADDRESS of your variable,
 * so the function can write a result into it. */
static void set_to_99(int *x) {
    *x = 99;
}

static void example3(void) {
    int status = 0;

    printf("Example 3: & in a call      (Lab 02: waitpid(pid, &status, 0);)\n");
    printf("  before: status = %d\n", status);
    set_to_99(&status);                  /* waitpid() fills in status the same way */
    printf("  after set_to_99(&status): status = %d\n", status);
}

/* Example 4: a C string must end with '\0'.
 * read() gives raw bytes WITHOUT '\0', so we add it ourselves. */
static void example4(void) {
    char buffer[8];
    buffer[0] = 'O';                     /* pretend read() gave us 2 bytes */
    buffer[1] = 'K';
    buffer[2] = '\0';                    /* we add the end marker */

    printf("Example 4: strings and '\\0' (Lab 02: buffer[count] = '\\0';)\n");
    printf("  buffer = \"%s\"  (%zu letters, then '\\0')\n", buffer, strlen(buffer));
}

/* Example 5 (hard): a 2D array, and a worker that gets its row
 * through a void * - exactly how a thread gets its argument. */
static int grid[2][3] = {{1, 2, 3},
                         {4, 5, 6}};

static void *sum_row(void *arg) {
    int row = *(int *)arg;               /* void * -> int * -> int */
    int sum = grid[row][0] + grid[row][1] + grid[row][2];
    printf("  worker got row = %d, sum of that row = %d\n", row, sum);
    return NULL;
}

static void example5(void) {
    int rows[2] = {0, 1};

    printf("Example 5: 2D array, void * (Lab 02: int row = *(int *)arg;)\n");
    printf("  grid[1][2] = %d  (row 1, column 2)\n", grid[1][2]);
    sum_row(&rows[1]);                   /* pthread_create() passes &rows[i] like this */
}

int main(int argc, char *argv[]) {
    int which = (argc > 1) ? atoi(argv[1]) : 0;   /* 0 = run all */

    if (which == 0 || which == 1) example1();
    if (which == 0 || which == 2) example2();
    if (which == 0 || which == 3) example3();
    if (which == 0 || which == 4) example4();
    if (which == 0 || which == 5) example5();
    return 0;
}
