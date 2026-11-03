/* COMP2040 Lab 04: Synchronization and Deadlocks
 *
 * Build: gcc -std=c11 -Wall -Wextra -pthread bank.c -o bank
 * Run:   ./bank race | mutex | deadlock | ordered
 */
#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DEPOSITS   100000
#define TRANSFERS  200
#define WATCHDOG   3        /* Seconds without progress before timeout */

/* ==========================================================================
 *  Type Definitions
 * ========================================================================== */

/**
 * Struct representing a single bank account with an embedded mutex lock.
 */
typedef struct {
    int id;               /* Unique numeric account identifier */
    long balance;         /* Current balance of the account */
    pthread_mutex_t lock; /* Mutex protecting balance access */
} account_t;

/**
 * Packaging struct to pass all required context to a transfer worker thread
 * via a single void pointer, eliminating the need for global state.
 */
typedef struct {
    account_t *from;      /* Source account losing money */
    account_t *to;        /* Destination account gaining money */
    bool use_order;       /* true = ordered lock acquisition; false = naive */
} transfer_job_t;

/* ==========================================================================
 *  Function Prototypes
 * ========================================================================== */

/* Deposit prototypes (Tasks 1 and 2) */
void deposit_unsafe(account_t *acc, long amount);
void deposit_safe(account_t *acc, long amount);
void *depositor_unsafe_worker(void *arg);
void *depositor_safe_worker(void *arg);
void run_deposits(account_t *acc, bool use_mutex);

/* Transfer and synchronization prototypes (Tasks 3 and 4) */
void pause_briefly(void);
void lock_both(account_t *first, account_t *second);
void unlock_both(account_t *first, account_t *second);
void move_money(account_t *from, account_t *to, long amount);
void transfer_naive(account_t *from, account_t *to, long amount);
void transfer_ordered(account_t *from, account_t *to, long amount);
void watchdog(int sig);
void *transferrer(void *arg);
void run_transfers(account_t *acc1, account_t *acc2, bool use_order);

/* ==========================================================================
 *  Main Function
 * ========================================================================== */

/**
 * Main entry point. Allocates local state, parses arguments, and executes tests.
 *
 * @param argc Count of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return 0 on success, 1 on invalid argument usage.
 */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s race|mutex|deadlock|ordered\n", argv[0]);
        return 1;
    }

    /* Local stack allocation of accounts: no global variables */
    account_t accounts[2];
    for (int i = 0; i < 2; i++) {
        accounts[i].id = i + 1;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    const char *mode = argv[1];

    if (strcmp(mode, "race") == 0) {
        run_deposits(&accounts[0], false);
    } else if (strcmp(mode, "mutex") == 0) {
        run_deposits(&accounts[0], true);
    } else if (strcmp(mode, "deadlock") == 0) {
        run_transfers(&accounts[0], &accounts[1], false);
    } else if (strcmp(mode, "ordered") == 0) {
        run_transfers(&accounts[0], &accounts[1], true);
    } else {
        fprintf(stderr, "Usage: %s race|mutex|deadlock|ordered\n", argv[0]);
        for (int i = 0; i < 2; i++) {
            pthread_mutex_destroy(&accounts[i].lock);
        }
        return 1;
    }

    /* Destroy mutex locks to prevent resource leaks */
    for (int i = 0; i < 2; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    return 0;
}

/* ==========================================================================
 *  Function Definitions: Deposit Operations (Tasks 1 and 2)
 * ========================================================================== */

/**
 * Performs an unsynchronized deposit into an account, causing race conditions.
 *
 * @param acc    Pointer to the bank account struct to modify.
 * @param amount Units of currency to add to the account.
 * @return void
 */
void deposit_unsafe(account_t *acc, long amount) {
    long tmp = acc->balance;   /* Read shared state */
    tmp = tmp + amount;        /* Modify local register */
    acc->balance = tmp;        /* Write shared state */
}

/**
 * Performs a synchronized deposit into an account protected by a mutex lock.
 * Resolves TODO 1 from the starter code.
 *
 * @param acc    Pointer to the bank account struct to modify.
 * @param amount Units of currency to add to the account.
 * @return void
 */
void deposit_safe(account_t *acc, long amount) {
    /* Lock the mutex before reading to establish a critical section */
    pthread_mutex_lock(&acc->lock);

    long tmp = acc->balance;
    tmp = tmp + amount;
    acc->balance = tmp;

    /* Unlock the mutex after writing to conclude the critical section */
    pthread_mutex_unlock(&acc->lock);
}

/**
 * Thread entry point for unsynchronized deposits.
 *
 * @param arg Pointer to the target account_t struct passed as a void pointer.
 * @return Always returns NULL upon completion.
 */
void *depositor_unsafe_worker(void *arg) {
    account_t *acc = (account_t *)arg;
    for (int i = 0; i < DEPOSITS; i++) {
        deposit_unsafe(acc, 1);
    }
    return NULL;
}

/**
 * Thread entry point for synchronized deposits.
 *
 * @param arg Pointer to the target account_t struct passed as a void pointer.
 * @return Always returns NULL upon completion.
 */
void *depositor_safe_worker(void *arg) {
    account_t *acc = (account_t *)arg;
    for (int i = 0; i < DEPOSITS; i++) {
        deposit_safe(acc, 1);
    }
    return NULL;
}

/**
 * Orchestrates two concurrent threads performing deposits on a shared account.
 *
 * @param acc       Pointer to the target account.
 * @param use_mutex Boolean flag indicating whether to use mutex synchronization.
 * @return void
 */
void run_deposits(account_t *acc, bool use_mutex) {
    pthread_t t1, t2;
    acc->balance = 0;

    /* Select the appropriate worker function according to mode */
    void *(*worker_func)(void *) = use_mutex ? depositor_safe_worker : depositor_unsafe_worker;

    pthread_create(&t1, NULL, worker_func, (void *)acc);
    pthread_create(&t2, NULL, worker_func, (void *)acc);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    long expected = 2L * DEPOSITS;
    printf("Two threads each deposited 1, %d times.\n", DEPOSITS);
    printf("Expected balance: %ld\n", expected);
    printf("Actual balance:   %ld", acc->balance);
    if (acc->balance == expected) {
        printf("   correct\n");
    } else {
        printf("   %ld deposits were lost\n", expected - acc->balance);
    }
}

/* ==========================================================================
 *  Function Definitions: Transfer and Concurrency Operations (Tasks 3 and 4)
 * ========================================================================== */

/**
 * Suspends execution for 1 ms to reliably trigger interleaving and race conditions.
 *
 * @return void
 */
void pause_briefly(void) {
    struct timespec t = {0, 1000000};
    nanosleep(&t, NULL);
}

/**
 * Acquires locks for two accounts sequentially with an artificial pause between them.
 *
 * @param first  Pointer to the account to lock first.
 * @param second Pointer to the account to lock second.
 * @return void
 */
void lock_both(account_t *first, account_t *second) {
    pthread_mutex_lock(&first->lock);
    pause_briefly();
    pthread_mutex_lock(&second->lock);
}

/**
 * Unlocks two accounts in reverse order of acquisition.
 *
 * @param first  Pointer to the account locked first.
 * @param second Pointer to the account locked second.
 * @return void
 */
void unlock_both(account_t *first, account_t *second) {
    pthread_mutex_unlock(&second->lock);
    pthread_mutex_unlock(&first->lock);
}

/**
 * Deducts amount from source balance and adds amount to target balance.
 * Assumes caller has already acquired locks on both accounts.
 *
 * @param from   Pointer to the source account debited.
 * @param to     Pointer to the destination account credited.
 * @param amount Units to transfer between balances.
 * @return void
 */
void move_money(account_t *from, account_t *to, long amount) {
    from->balance -= amount;
    to->balance += amount;
}

/**
 * Naively executes a transfer by locking the "from" account first.
 * Leads to deadlock when opposite-direction transfers run concurrently.
 *
 * @param from   Pointer to the source account debited.
 * @param to     Pointer to the destination account credited.
 * @param amount Units to transfer.
 * @return void
 */
void transfer_naive(account_t *from, account_t *to, long amount) {
    lock_both(from, to);
    move_money(from, to, amount);
    unlock_both(from, to);
}

/**
 * Executes a deadlock-free transfer by locking the account with lower id first.
 * Enforces a strict linear locking order, eliminating circular wait.
 * Resolves TODO 2 from the starter code.
 *
 * @param from   Pointer to the source account debited.
 * @param to     Pointer to the destination account credited.
 * @param amount Units to transfer.
 * @return void
 */
void transfer_ordered(account_t *from, account_t *to, long amount) {
    /* Determine lock order based on unique account id */
    account_t *first = (from->id < to->id) ? from : to;
    account_t *second = (from->id < to->id) ? to : from;

    lock_both(first, second);
    move_money(from, to, amount);
    unlock_both(first, second);
}

/**
 * Signal handler triggered when SIGALRM expires due to lack of progress.
 *
 * @param sig Signal number received.
 * @return void (aborts process).
 */
void watchdog(int sig) {
    (void)sig;
    const char msg[] =
        "\nWatchdog: no transfer has finished for 3 seconds.\n"
        "The threads are stuck - this looks like a deadlock. Exiting.\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
    _exit(3);
}

/**
 * Thread entry point for processing transfer jobs.
 *
 * @param arg Pointer to a transfer_job_t struct holding transfer parameters.
 * @return Always returns NULL upon completion.
 */
void *transferrer(void *arg) {
    transfer_job_t *job = (transfer_job_t *)arg;

    for (int i = 0; i < TRANSFERS; i++) {
        if (job->use_order) {
            transfer_ordered(job->from, job->to, 1);
        } else {
            transfer_naive(job->from, job->to, 1);
        }
        alarm(WATCHDOG);   /* Reset timeout counter when a transfer completes */
    }
    return NULL;
}

/**
 * Runs two concurrent threads performing bidirectional transfers.
 *
 * @param acc1      Pointer to the first local account.
 * @param acc2      Pointer to the second local account.
 * @param use_order Boolean flag: true uses ordered locking, false uses naive locking.
 * @return void
 */
void run_transfers(account_t *acc1, account_t *acc2, bool use_order) {
    acc1->balance = 1000;
    acc2->balance = 1000;

    /* Build local job configurations on the stack */
    transfer_job_t job_a = {
        .from = acc1,
        .to = acc2,
        .use_order = use_order
    };
    transfer_job_t job_b = {
        .from = acc2,
        .to = acc1,
        .use_order = use_order
    };

    printf("Thread A moves 1 from account %d to %d, %d times.\n", acc1->id, acc2->id, TRANSFERS);
    printf("Thread B moves 1 from account %d to %d, %d times.\n", acc2->id, acc1->id, TRANSFERS);
    fflush(stdout);

    signal(SIGALRM, watchdog);
    alarm(WATCHDOG);

    pthread_t ta, tb;
    pthread_create(&ta, NULL, transferrer, (void *)&job_a);
    pthread_create(&tb, NULL, transferrer, (void *)&job_b);

    pthread_join(ta, NULL);
    pthread_join(tb, NULL);

    alarm(0);  /* Cancel watchdog timer upon normal termination */

    long total = acc1->balance + acc2->balance;
    printf("All transfers finished. Account %d: %ld, account %d: %ld\n",
           acc1->id, acc1->balance, acc2->id, acc2->balance);
    printf("Total money: %ld (expected 2000)\n", total);
}
