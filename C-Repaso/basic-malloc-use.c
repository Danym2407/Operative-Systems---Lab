/**
 * @file basic-malloc-use.c
 * @author: Daniela Mendez Ramirez
 * @date: 2026-11-02
 * @version 1.0
 * 
 * @details This code is part of a series of exercises to practice C programming and the use of structures, pointers, and memory management. 
 *          It demonstrates basic compilation with warning and debugging flags.
 * 
 * @note Compilation command:
 *       gcc -Wall -Wextra basic-malloc-use.c -o basic-malloc-use
 *       -Wall: Enables common compiler warnings
 *       -Wextra: Enables extra warning flags.
 *       -o: Specifies the output executable name
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {

    int *number_pointer;

    number_pointer = malloc(sizeof(int));

    *number_pointer = 50;

    printf("Number: %d\n", *number_pointer);

    free(number_pointer);

    return 0;
}
