#include <stdio.h>
#include <stdlib.h>

int main(void){

    int *age_pointer;
    age_pointer = malloc(sizeof(int));
    *age_pointer = 22;

    printf("Age: %d\n", *age_pointer);
    *age_pointer = 23;
    printf("Age: %d\n", *age_pointer);
    free(age_pointer);

    age_pointer = NULL; /* Set the pointer to NULL after freeing it */
    return 0;
}
