/*
 * COMP2040 - Lab 01
 * Task 2: Header files and multiple source files
 */

#include <stdio.h>
#include "math_operations.h"

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    /* TODO: implement this function. */
    (void)a;
    (void)b;
    return 0;
}

double divide_numbers(int a, int b) {
    /* TODO: handle division by zero and return a / b as a double. */
    (void)a;
    (void)b;
    return 0.0;
}
