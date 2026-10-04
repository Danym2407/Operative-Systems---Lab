/**
 * @file math_operations.c
 * @author Daniela Mendez Ramirez
 * @date 2026-10-04
 * @version 1.0
 */

#include <stdio.h>
#include "math_operations.h"

int add(int a, int b) {
    return a + b;
}

int substract(int a, int b) {
    return a - b;
}
int multiply(int a, int b) {
    return a * b;
}
double divide_numbers(int a, int b) {
    if (b == 0) {
        printf("Error: Division by zero is not allowed.\n");
        return 0.0; // Return 0.0 or handle the error as needed
    }
    return (double)a / b;
}       