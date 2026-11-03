
#include <stdio.h>
#include <string.h>

/* Structure to store book information */
typedef struct {
    char title[100];
    int pages;
    float price;
} Book;

/* Function prototypes */
void display_books(Book books[], int number_of_books);
float calculate_total_prices(Book books[], int number_of_books);

int main(void) {

    Book books[3];

    /* Assign information to the first book */
    strcpy(books[0].title, "Mujercitas");
    books[0].pages = 200;
    books[0].price = 150.0f;

    /* Assign information to the second book */
    strcpy(books[1].title, "Cien años de soledad");
    books[1].pages = 400;
    books[1].price = 200.0f;

    /* Assign information to the third book */
    strcpy(books[2].title, "El principito");
    books[2].pages = 150;
    books[2].price = 100.0f;

    /* Display all books */
    display_books(books, 3);

    /* Calculate and display the total price */
    float total = calculate_total_prices(books, 3);

    printf("\nTotal price: %.2f\n", total);

    return 0;
}

/**
 * Displays the information of all books.
 *
 * @param books Array of books.
 * @param number_of_books Number of books in the array.
 * @return None.
 */
void display_books(Book books[], int number_of_books) {

    for (int index = 0; index < number_of_books; index++) {

        printf("\nBook %d:\n", index + 1);
        printf("Title: %s\n", books[index].title);
        printf("Pages: %d\n", books[index].pages);
        printf("Price: %.2f\n", books[index].price);
    }
}

/**
 * Calculates the total price of all books.
 *
 * @param books Array of books.
 * @param number_of_books Number of books in the array.
 * @return Total price of all books.
 */
float calculate_total_prices(Book books[], int number_of_books) {

    float total = 0.0f;

    for (int index = 0; index < number_of_books; index++) {
        total += books[index].price;
    }

    return total;
}
