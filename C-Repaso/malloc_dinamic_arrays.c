#include <stdio.h>
#include <stdlib.h>

int main(void){

    int *numbers;
    numbers = malloc(5 * sizeof(int));
    numbers[0] = 10; 
    numbers[1] = 20;
    numbers[2] = 30;
    numbers[3] = 40;
    numbers[4] = 50;

    for (int i = 0; i < 5; i++){
        printf("Number %d: %d\n", i + 1, numbers[i]);
    }

    free(numbers);

    numbers = NULL; /* Set the pointer to NULL after freeing it */

    return 0;
}
