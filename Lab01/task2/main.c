/*
 * COMP2040 - Lab 01
 * Task 2: Header files and multiple source files
 */

#include <stdio.h>
#include "math_operations.h"

int main(void) {
    int a = 10;
    int b = 5;

    printf("%d + %d = %d\n", a, b, add(a, b));
    printf("%d - %d = %d\n", a, b, subtract(a, b));
    printf("%d * %d = %d\n", a, b, multiply(a, b));
    printf("%d / %d = %.2f\n", a, b, divide_numbers(a, b));

    return 0;
}
