#include <stdio.h>
#include <stdlib.h>

int main(void){

    int *numbers;
    int *temporary_pointer;
    numbers = calloc(5, sizeof(int)); /* Allocate memory for 5 integers and initialize to zero */

    /* See what does calloc initialize the memory to */
    for (int i = 0; i < 5; i++){
        printf("Number %d: %d\n", i + 1, numbers[i]);
    }


    numbers[0] = 10; 
    numbers[1] = 20;
    numbers[2] = 30;
    numbers[3] = 40;
    numbers[4] = 50;

    for (int i = 0; i < 5; i++){
        printf("Number %d: %d\n", i + 1, numbers[i]);
    }

    temporary_pointer = realloc(numbers, 8 * sizeof(int)); /* Reallocate memory for 8 integers */
    if (temporary_pointer != NULL){
        numbers = temporary_pointer; /* Update the pointer to the new memory block */
    }

    for (int i = 0; i < 8; i++){
        printf("Number %d: %d\n", i + 1, numbers[i]);
    }
    numbers[5] = 60;
    numbers[6] = 70;
    numbers[7] = 80;

    for (int i = 0; i < 8; i++){
        printf("Number %d: %d\n", i + 1, numbers[i]);
    }

    free(numbers);

    numbers = NULL; /* Set the pointer to NULL after freeing it */

    return 0;
}
