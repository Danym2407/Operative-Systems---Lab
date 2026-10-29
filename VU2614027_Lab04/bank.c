/* COMP2040 - Lab 04: Synchronization and Deadlocks
 *
 * This lab covers Lectures 5 and 6 with one small program. You will watch two threads lose
 * updates to a shared bank account, fix it with a mutex, then watch two transfers that each look
 * correct freeze in a deadlock, and prevent it with a lock order. The story is the same in both
 * weeks: a lock fixes a race, and two locks can stop a program completely.
 * The starter file bank.c has four modes, one per task, and two TODO sections. Each account is
 * a struct (account_t) with a balance and its own pthread_mutex_t; the C Concepts for Lab 03
 * guide explains structs and ->, and Lab 02 Task 5 shows pthread_create() and pthread_join().
 * Build the program once, and again after each change:
 * 
 * gcc -std=c11 -Wall -Wextra -pthread bank.c -o bank
 *
 * Build and run the program with one of the four modes:
 * ./bank
 * ./bank mutex
 * ./bank ordered
 * ./bank race      
 * ./bank deadlock 
 *
 */


#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DEPOSITS   100000
#define TRANSFERS  200
#define WATCHDOG   3        // seconds without progress

typedef struct {
    int id;
    long balance;
    pthread_mutex_t lock;   // protects balance
} account_t;

account_t accounts[2];


/* ---------------------------------------------------------------------------
 * Function Prototypes
 * ------------------------------------------------------------------------- */
// Tasks 1 and 2
void deposit_unsafe(account_t *acc, long amount);
void deposit_safe(account_t *acc, long amount);
int use_mutex;   // 0 = race, 1 = mutex
void *depositor(void *arg);
void run_deposits(void);

// Tasks 3 and 4
// sleep 1 ms between the two locks so the problem shows up on every run
void pause_briefly(void);
void lock_both(account_t *first, account_t *second);
void unlock_both(account_t *first, account_t *second);
void move_money(account_t *from, account_t *to, long amount);

// Task 3: lock the "from" account first
void transfer_naive(account_t *from, account_t *to, long amount);


// Task 4: lock the account with the lower id first
void transfer_ordered(account_t *from, account_t *to, long amount);

// runs on SIGALRM: no transfer finished for WATCHDOG seconds
void watchdog(int sig);

typedef struct {
    account_t *from;
    account_t *to;
} job_t;

int use_order;   // 0 = deadlock, 1 = ordered

void *transferrer(void *arg);

void run_transfers(void);


/* ---------------------------------------------------------------------------
 * Main
 * ------------------------------------------------------------------------- */

int main(int argc, char *argv[]) {
    for (int i = 0; i < 2; i++) {
        accounts[i].id = i + 1;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    const char *mode = (argc == 2) ? argv[1] : "";
    if (strcmp(mode, "race") == 0) {
        use_mutex = 0;
        run_deposits();
    } else if (strcmp(mode, "mutex") == 0) {
        use_mutex = 1;
        run_deposits();
    } else if (strcmp(mode, "deadlock") == 0) {
        use_order = 0;
        run_transfers();
    } else if (strcmp(mode, "ordered") == 0) {
        use_order = 1;
        run_transfers();
    } else {
        fprintf(stderr, "Usage: %s race|mutex|deadlock|ordered\n", argv[0]);
        return 1;
    }
    return 0;
}


/* ---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------- */

void deposit_unsafe(account_t *acc, long amount) {
    long tmp = acc->balance;   // read
    tmp = tmp + amount;        // add
    acc->balance = tmp;        // write
}

void deposit_safe(account_t *acc, long amount) {
    // TODO 1: lock acc->lock before the read and unlock it after the write
    long tmp = acc->balance;
    tmp = tmp + amount;
    acc->balance = tmp;
}

void *depositor(void *arg) {
    account_t *acc = arg;
    for (int i = 0; i < DEPOSITS; i++) {
        if (use_mutex) {
            deposit_safe(acc, 1);
        } else {
            deposit_unsafe(acc, 1);
        }
    }
    return NULL;
}

void run_deposits(void) {
    pthread_t t1, t2;
    accounts[0].balance = 0;
    pthread_create(&t1, NULL, depositor, &accounts[0]);
    pthread_create(&t2, NULL, depositor, &accounts[0]);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    long expected = 2L * DEPOSITS;
    printf("Two threads each deposited 1, %d times.\n", DEPOSITS);
    printf("Expected balance: %ld\n", expected);
    printf("Actual balance:   %ld", accounts[0].balance);
    if (accounts[0].balance == expected) {
        printf("   correct\n");
    } else {
        printf("   %ld deposits were lost\n", expected - accounts[0].balance);
    }
}

void pause_briefly(void) {
    struct timespec t = {0, 1000000};
    nanosleep(&t, NULL);
}

void lock_both(account_t *first, account_t *second) {
    pthread_mutex_lock(&first->lock);
    pause_briefly();
    pthread_mutex_lock(&second->lock);
}

void unlock_both(account_t *first, account_t *second) {
    pthread_mutex_unlock(&second->lock);
    pthread_mutex_unlock(&first->lock);
}

void move_money(account_t *from, account_t *to, long amount) {
    from->balance -= amount;
    to->balance += amount;
}

void transfer_naive(account_t *from, account_t *to, long amount) {
    lock_both(from, to);
    move_money(from, to, amount);
    unlock_both(from, to);
}

void transfer_ordered(account_t *from, account_t *to, long amount) {
    // TODO 2: choose first (lower id) and second, then call lock_both(first, second).
    // As it is now, this does the same as transfer_naive().
    lock_both(from, to);
    move_money(from, to, amount);
    unlock_both(from, to);
}

void watchdog(int sig) {
    (void)sig;
    const char msg[] =
        "\nWatchdog: no transfer has finished for 3 seconds.\n"
        "The threads are stuck - this looks like a deadlock. Exiting.\n";
    write(STDERR_FILENO, msg, sizeof msg - 1);
    _exit(3);
}

void *transferrer(void *arg) {
    job_t *job = arg;
    for (int i = 0; i < TRANSFERS; i++) {
        if (use_order) {
            transfer_ordered(job->from, job->to, 1);
        } else {
            transfer_naive(job->from, job->to, 1);
        }
        alarm(WATCHDOG);   // a transfer finished: restart the countdown
    }
    return NULL;
}

void run_transfers(void) {
    accounts[0].balance = 1000;
    accounts[1].balance = 1000;
    job_t a_to_b = {&accounts[0], &accounts[1]};
    job_t b_to_a = {&accounts[1], &accounts[0]};

    printf("Thread A moves 1 from account 1 to 2, %d times.\n", TRANSFERS);
    printf("Thread B moves 1 from account 2 to 1, %d times.\n", TRANSFERS);
    fflush(stdout);

    signal(SIGALRM, watchdog);
    alarm(WATCHDOG);
    pthread_t ta, tb;
    pthread_create(&ta, NULL, transferrer, &a_to_b);
    pthread_create(&tb, NULL, transferrer, &b_to_a);
    pthread_join(ta, NULL);
    pthread_join(tb, NULL);
    alarm(0);

    long total = accounts[0].balance + accounts[1].balance;
    printf("All transfers finished. Account 1: %ld, account 2: %ld\n",
           accounts[0].balance, accounts[1].balance);
    printf("Total money: %ld (expected 2000)\n", total);
}