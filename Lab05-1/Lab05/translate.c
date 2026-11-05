/* COMP2040 Lab 05: address translation with a page table and a TLB
 *
 * Build: gcc -std=c11 -Wall -Wextra translate.c -o translate
 * Run:   ./translate checks | local | scattered
 */
#include <stdio.h>
#include <string.h>

#define PAGE_SIZE  256
#define NUM_PAGES  16
#define TLB_SIZE   4
#define ACCESSES   3000

// returned when there is no physical address
#define OK            0
#define INVALID      -1
#define NOT_RESIDENT -2
#define PROTECTION   -3

typedef struct {
    int mapped;     // part of the address space?
    int resident;   // in physical memory now?
    int writable;   // writes allowed?
    int frame;
} pte_t;

pte_t page_table[NUM_PAGES] = {
    // mapped resident writable frame
    {1, 1, 0, 5},    // page 0
    {1, 1, 0, 6},    // page 1
    {1, 1, 1, 1},    // page 2
    {1, 1, 1, 2},    // page 3
    {1, 1, 1, 9},    // page 4
    {1, 0, 1, 0},    // page 5
    {0, 0, 0, 0},    // page 6
    {0, 0, 0, 0},    // page 7
    {1, 1, 1, 3},    // page 8
    {1, 1, 1, 7},    // page 9
    {1, 1, 1, 8},    // page 10
    {1, 1, 1, 10},   // page 11
    {1, 0, 0, 0},    // page 12
    {0, 0, 0, 0},    // page 13
    {1, 1, 1, 11},   // page 14
    {1, 1, 1, 4},    // page 15
};

typedef struct {
    int page;
    int frame;
    int writable;
} tlb_entry_t;

tlb_entry_t tlb[TLB_SIZE];
int tlb_used;     // entries filled
int tlb_next;     // entry to overwrite when full
int tlb_hits;
int tlb_misses;

// TODO 1: return the page number of v
int page_number(int v) {
    return v;
}

// TODO 1: return the offset of v
int page_offset(int v) {
    return v;
}

// TODO 2: return INVALID, NOT_RESIDENT or OK for this page
int check_page_table(int page) {
    (void)page;   // delete this line
    return INVALID;
}

// TODO 3: search the filled entries for this page.
// Hit: count it and return the entry. Miss: count it and return NULL.
tlb_entry_t *tlb_lookup(int page) {
    (void)page;   // delete this line
    return NULL;
}

// add a translation; when full, overwrite the oldest entry
tlb_entry_t *tlb_insert(int page) {
    tlb_entry_t *e = &tlb[tlb_next];
    e->page = page;
    e->frame = page_table[page].frame;
    e->writable = page_table[page].writable;
    tlb_next = (tlb_next + 1) % TLB_SIZE;
    if (tlb_used < TLB_SIZE) {
        tlb_used++;
    }
    return e;
}

// op is 'R' or 'W'. Returns a physical address or an error value.
int translate(int v, char op) {
    int page = page_number(v);
    int offset = page_offset(v);

    tlb_entry_t *e = tlb_lookup(page);
    if (e == NULL) {
        int result = check_page_table(page);
        if (result != OK) {
            return result;
        }
        e = tlb_insert(page);
    }

    if (op == 'W' && !e->writable) {
        return PROTECTION;
    }
    return e->frame * PAGE_SIZE + offset;
}

void run_checks(void) {
    int addr[] = {0, 519, 519, 5, 1300, 1536, 4095, 4096};
    char op[]  = {'R', 'R', 'W', 'W', 'R', 'R', 'R', 'R'};
    int n = sizeof addr / sizeof addr[0];

    for (int i = 0; i < n; i++) {
        int pa = translate(addr[i], op[i]);
        printf("%c %4d  page %2d  offset %3d  ", op[i], addr[i],
               page_number(addr[i]), page_offset(addr[i]));
        if (pa == INVALID) {
            printf("invalid address\n");
        } else if (pa == NOT_RESIDENT) {
            printf("page not in memory\n");
        } else if (pa == PROTECTION) {
            printf("protection violation\n");
        } else {
            printf("physical %d\n", pa);
        }
    }
}

// read pages 2, 3 and 4 in order, over and over
void run_local(void) {
    for (int i = 0; i < ACCESSES; i++) {
        translate(2 * PAGE_SIZE + i % (3 * PAGE_SIZE), 'R');
    }
}

// read the resident pages in a scattered order (same sequence for everyone)
void run_scattered(void) {
    int pages[] = {0, 1, 2, 3, 4, 8, 9, 10, 11, 14, 15};
    int n = sizeof pages / sizeof pages[0];
    unsigned int x = 1;

    for (int i = 0; i < ACCESSES; i++) {
        x = (x * 1103515245u + 12345u) & 0x7FFFFFFFu;
        int r = (int)(x >> 8);
        int page = pages[(r >> 8) % n];
        translate(page * PAGE_SIZE + r % PAGE_SIZE, 'R');
    }
}

int main(int argc, char *argv[]) {
    const char *mode = (argc == 2) ? argv[1] : "";

    if (strcmp(mode, "checks") == 0) {
        run_checks();
    } else if (strcmp(mode, "local") == 0) {
        run_local();
    } else if (strcmp(mode, "scattered") == 0) {
        run_scattered();
    } else {
        fprintf(stderr, "Usage: %s checks|local|scattered\n", argv[0]);
        return 1;
    }

    int total = tlb_hits + tlb_misses;
    printf("TLB hits: %d  misses: %d  hit rate: %.2f%%\n", tlb_hits, tlb_misses,
           total > 0 ? 100.0 * tlb_hits / total : 0.0);
    return 0;
}
