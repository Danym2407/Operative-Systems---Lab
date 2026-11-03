#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){

    int age = 22;
    int *age_pointer = &age;
    
    printf("Age: %d\n", age);
    printf("Veremos el contenido de la variable a la que apunta age_pointer: %d\n", *age_pointer);
    printf("Dirección de memoria de age_pointer: %p\n", (void *)&age_pointer);
    printf("Dirección de memoria de age: %p\n", (void *)&age);
    printf("Dirección de memoria de age desde el apuntador: %p\n", (void *)age_pointer);
    return 0;

}
