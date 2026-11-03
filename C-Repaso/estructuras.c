
#include <stdio.h>
#include <string.h>

/* Structure to store book information */
typedef struct {
    char title[100];
    int pages;
    float price;
} tipo_libro;

int main(void) {

    tipo_libro my_book;

    strcpy(my_book.title, "Mujercitas");
    my_book.pages = 300;
    my_book.price = 199.99;

    printf("Title: %s\n", my_book.title);
    printf("Pages: %d\n", my_book.pages);
    printf("Price: %.2f\n", my_book.price);

    return 0;
}
