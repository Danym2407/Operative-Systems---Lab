/**
 * @file bank_exercise.c
 * @author: Daniela Mendez Ramirez
 * @date: 2026-11-02
 * @version 1.0
 * 
 * @details This code is part of a series of exercises to practice C programming and the use of structures, pointers, and memory management. 
 *          It demonstrates basic compilation with warning and debugging flags.
 * 
 * @note Compilation command:
 *       gcc -Wall -Wextra bank_exercise.c -o bank_exercise
 *       -Wall: Enables common compiler warnings
 *       -Wextra: Enables extra warning flags.
 *       -o: Specifies the output executable name
 */

 #include <stdio.h>
 #include <string.h>

 /* Structure definition for a bank account */ 
 typedef struct {
    int id;
    float balance;
    char owner[50];
 }Account;

/**** Prototypes ****/

void display_accounts(Account accounts[], int number_of_accounts);
void deposit_money(Account *account, float amount);
int withdraw_money(Account *account, float amount);
Account *find_account(Account accounts[], int number_of_accounts, int account_id);
void display_a_single_account(Account *account);

/**** Main function ****/
 int main (void){

    /* Create an array of three accounts */
    Account accounts[3];

    /* Declare variables for account initialization */
    Account *find_account_ptr;

    /* Initialize the accounts */
    /* Account 1 */
    accounts[0].id = 101;
    accounts[0].balance = 1500.0;
    strcpy(accounts[0].owner, "Daniela");

    /* Account 2 */
    accounts[1].id = 102;
    accounts[1].balance = 2500.0;
    strcpy(accounts[1].owner, "Eduardo");

    /* Account 3 */
    accounts[2].id = 103;
    accounts[2].balance = 800.0;
    strcpy(accounts[2].owner, "Andrea");

    /* Display account information */
    display_accounts(accounts, 3);

    /* Deposit money into Account 1 */
    deposit_money(&accounts[0], 500.0);

    /* Display updated account information */
    printf("\nDisplay updated account information from Daniela +500\n");
    display_a_single_account(&accounts[0]);

    /* Withdraw money from Account 2 */
    printf("\nDisplay updated account information from Eduardo -300\n");
    withdraw_money(&accounts[1], 300.0);
    /* Display updated account information */
    display_a_single_account(&accounts[1]);

    /* Withdraw 1000 from Account 3 */
    printf("\nDisplay updated account information from Andrea -1000\n");
    withdraw_money(&accounts[2], 1000.0);
    /* Display updated account information */
    display_a_single_account(&accounts[2]);
    
    /* Find account by ID 102 and display its information */
    printf("\nSearching for account with ID 102:\n");
    find_account_ptr = find_account(accounts, 3, 102);
    display_a_single_account(find_account_ptr);

    /* Try to find account 999*/
    printf("Searching for 999 id account");
    find_account_ptr = find_account(accounts, 3, 999);
    display_a_single_account(find_account_ptr);

    /* Display everything*/
    display_accounts(accounts, 3);


    return 0;
 }

 /**** Function definitions ****/

/**
 * Displays all bank accounts.
 *
 * @param accounts Array of bank accounts.
 * @param number_of_accounts Number of accounts.
 * @return None.
 */
void display_accounts(Account accounts[], int number_of_accounts) {

    for (int i = 0 ; i < number_of_accounts; i++){
        printf("\nAccount %d:\n", i + 1);
        printf("Account Id: %d\n", accounts[i].id);
        printf("Owner: %s\n", accounts[i].owner);
        printf("Balance: %.2f\n", accounts[i].balance);
    }
}


/**
 * Deposits money into a bank account.
 *
 * @param account Pointer to the bank account.
 * @param amount Amount of money to deposit.
 * @return None.
 */
void deposit_money(Account *account, float amount) {
    account->balance += amount;

}


/**
 * Withdraws money from a bank account.
 *
 * @param account Pointer to the bank account.
 * @param amount Amount of money to withdraw.
 * @return 1 if successful, 0 otherwise.
 */
int withdraw_money(Account *account, float amount) {

    if (amount > account->balance) {
        return 0; /* Withdrawal failed */
    }else {
        account->balance -= amount;
        return 1; /* Withdrawal successful */
    }
}


/**
 * Finds a bank account using its ID.
 *
 * @param accounts Array of bank accounts.
 * @param number_of_accounts Number of accounts.
 * @param account_id ID of the account to find.
 * @return Pointer to the account if found, NULL otherwise.
 */
Account *find_account(Account accounts[], int number_of_accounts, int account_id) {

    for (int i = 0; i < number_of_accounts; i++){
        if (accounts[i].id == account_id) {
            return &accounts[i];
        }
    }

    return NULL;
    
}


/**
 * Displays the information of a single bank account.
 * 
 * @param accounts Array of bank accounts
 * @param account_id ID of the account to display
 * @return None.
 */

void display_a_single_account(Account *account){

    if (account == NULL) {
        printf("\nAccount not found.\n");
        return;
    }
    printf("\nId account: %d\n", account->id);
    printf("Owner Name: %s\n", account->owner);
    printf("Balance: %.2f\n", account->balance);
}
