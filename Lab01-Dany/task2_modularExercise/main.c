/**
 * @file main.c
 * @author Daniela Mendez Ramirez
 * @date 2026-10-04
 * @version 1.0
 *
 * @details This file is part of Operating Systems Lab 01 - Task 2.
 * It demonstrates how to build a modular C program, declare functions,
 * and call them across multiple source files.
 *
 * @note Structure of a modular C program:
 *       - Header (.h): In this case 'math_operations.h', which contains only
 *         function prototypes, return types, and parameters (the public interface).
 *       - Implementation (.c): In this case 'math_operations.c', where the actual
 *         function definitions and logic are written (e.g., addition, multiplication).
 *       - Main program / Client (main.c): Integrates everything by including the header
 *         and calling the declared functions.
 */

 #include <stdio.h>
 #include "math_operations.h"

 int main(){
    
 }