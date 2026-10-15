/* COMP2040 - Lab 03: CPU scheduling simulator
 *
 * Simulates FCFS, non-preemptive SJF and round robin on a fixed workload and
 * prints the execution trace and the per-process metrics from Lecture 4.
 *
 * Build: gcc -std=c11 -Wall -Wextra sched_sim.c -o sched_sim
 * Run:   ./sched_sim fcfs lecture
 *        ./sched_sim sjf lab
 *        ./sched_sim rr lecture 2        (the last number is the quantum)
 *
 * Model (same as the lecture): one CPU, one CPU burst per process, no I/O,
 * zero switching cost, all times in ms.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCS  10
#define MAX_SLICES 100

/* ---------------------------------------------------------------------------
 * Workloads: one row per process, {arrival, burst}, sorted by arrival time.
 * Row 0 is P1, row 1 is P2, and so on.
 * ------------------------------------------------------------------------- */
static const int lecture_workload[][2] = {   /* Lecture 4, slide 19 */
    {0, 8}, {1, 4}, {2, 2}, {3, 1},
};
static const int lab_workload[][2] = {       /* the workload of this lab */
    {0, 5}, {1, 3}, {3, 6}, {5, 1}, {16, 2},
};

typedef struct {
    int id;         /* 1, 2, 3, ... printed as P1, P2, P3, ... */
    int arrival;    /* A: arrival time */
    int burst;      /* B: total CPU time needed */
    int remaining;  /* CPU time still needed */
    int start;      /* S: first time on the CPU, -1 = not started yet */
    int finish;     /* C: completion time, -1 = not finished yet */
    int queued;     /* RR only: 1 once the process has entered the ready queue */
} process_t;

typedef struct {
    int id;         /* which process ran ... */
    int from;       /* ... from this time ... */
    int to;         /* ... until this time */
} slice_t;

static process_t procs[MAX_PROCS];
static int n;                       /* number of processes in the workload */
static slice_t trace[MAX_SLICES];   /* who ran when, in order */
static int n_slices;
static int context_switches;
static int last_id;                 /* id of the process that ran last, 0 = none yet */

/* Copy a workload table into procs[]. Returns -1 for an unknown name. */
static int load_workload(const char *name) {
    const int (*table)[2];
    if (strcmp(name, "lecture") == 0) {
        table = lecture_workload;
        n = (int)(sizeof lecture_workload / sizeof lecture_workload[0]);
    } else if (strcmp(name, "lab") == 0) {
        table = lab_workload;
        n = (int)(sizeof lab_workload / sizeof lab_workload[0]);
    } else {
        return -1;
    }
    for (int i = 0; i < n; i++) {
        procs[i].id = i + 1;
        procs[i].arrival = table[i][0];
        procs[i].burst = table[i][1];
        procs[i].remaining = table[i][1];
        procs[i].start = -1;
        procs[i].finish = -1;
        procs[i].queued = 0;
    }
    return 0;
}

/* Run process p on the CPU from time `from` to time `to`. Every policy calls
 * this, so it is the one place that records the trace and the metrics. */
static void run_slice(process_t *p, int from, int to) {
    /* TODO 3: count a context switch whenever the CPU starts running a
     * process that is different from the one that ran last (last_id).
     * The very first dispatch is not a switch. */

    last_id = p->id;                     /* remember who ran last */

    if (p->start < 0) {
        p->start = from;                 /* first time on the CPU */
    }
    p->remaining -= to - from;
    if (p->remaining == 0) {
        p->finish = to;
    }

    /* Record the slice, or extend the last one if the same process continues. */
    if (n_slices > 0 && trace[n_slices - 1].id == p->id && trace[n_slices - 1].to == from) {
        trace[n_slices - 1].to = to;
    } else if (n_slices < MAX_SLICES) {
        trace[n_slices].id = p->id;
        trace[n_slices].from = from;
        trace[n_slices].to = to;
        n_slices++;
    }
}

/* Earliest arrival time after `time` of a process that still needs the CPU.
 * Used when nothing is ready: the CPU idles until then. */
static int next_arrival(int time) {
    int next = -1;
    for (int i = 0; i < n; i++) {
        if (procs[i].remaining > 0 && procs[i].arrival > time &&
            (next < 0 || procs[i].arrival < next)) {
            next = procs[i].arrival;
        }
    }
    return next;
}

/* ---------------------------------------------------------------------------
 * Non-preemptive policies: pick a READY process, run it to completion, repeat.
 * Ready = has arrived (arrival <= time) and still needs the CPU.
 * ------------------------------------------------------------------------- */

/* FCFS: the ready process that arrived first (ties: lower id, because the
 * loop keeps the first one it finds). Returns NULL if nothing is ready. */
static process_t *pick_fcfs(int time) {
    process_t *best = NULL;
    for (int i = 0; i < n; i++) {
        process_t *p = &procs[i];
        if (p->arrival > time || p->remaining == 0) {
            continue;                    /* not ready */
        }
        if (best == NULL || p->arrival < best->arrival) {
            best = p;
        }
    }
    return best;
}

/* SJF: the ready process with the shortest burst.
 * Ties: earlier arrival first, then lower id. Returns NULL if nothing is ready. */
static process_t *pick_sjf(int time) {
    /* TODO 1: like pick_fcfs(), but choose the smallest burst instead of the
     * earliest arrival. Only READY processes may be chosen.
     * Until you write it, SJF simply behaves like FCFS. */
    return pick_fcfs(time);
}

static void schedule_nonpreemptive(int use_sjf) {
    int time = 0;
    int finished = 0;
    while (finished < n) {
        process_t *p = use_sjf ? pick_sjf(time) : pick_fcfs(time);
        if (p == NULL) {                 /* nothing ready: idle until next arrival */
            time = next_arrival(time);
            continue;
        }
        int end = time + p->remaining;   /* non-preemptive: run to completion */
        run_slice(p, time, end);
        time = end;
        finished++;
    }
}

/* ---------------------------------------------------------------------------
 * Round robin: a FIFO ready queue. Each process is in it at most once, so a
 * ring buffer of MAX_PROCS entries is enough.
 * ------------------------------------------------------------------------- */
static process_t *queue[MAX_PROCS];
static int q_head;                       /* index of the first element */
static int q_count;                      /* number of elements */

static void queue_push(process_t *p) {   /* add at the tail */
    queue[(q_head + q_count) % MAX_PROCS] = p;
    q_count++;
}

static process_t *queue_pop(void) {      /* remove from the head */
    process_t *p = queue[q_head];
    q_head = (q_head + 1) % MAX_PROCS;
    q_count--;
    return p;
}

/* Put every process that has arrived by `time` into the queue (once). */
static void admit_arrivals(int time) {
    for (int i = 0; i < n; i++) {
        if (!procs[i].queued && procs[i].arrival <= time) {
            procs[i].queued = 1;
            queue_push(&procs[i]);
        }
    }
}

static void schedule_rr(int quantum) {
    /* TODO 2: simulate round robin with the queue functions above.
     * Repeat until every process has finished:
     *   1. if the queue is empty, the CPU idles: jump to next_arrival(time)
     *      and admit the processes that have arrived;
     *   2. pop the head p and run it for quantum ms, or less if it needs less;
     *   3. advance time, then admit new arrivals BEFORE putting p back,
     *      so that new arrivals join the queue ahead of p (lecture rule);
     *   4. if p still needs the CPU, push it to the tail; otherwise it finished. */

    printf("schedule_rr() is not written yet (TODO 2).\n");
    /* Delete this line when you write your code: it only stops the compiler
     * warning about helpers that are not used yet. */
    (void)quantum; (void)queue_pop; (void)admit_arrivals;
}

/* ---------------------------------------------------------------------------
 * Report: trace, per-process metrics (Lecture 4, slide 15) and averages.
 * ------------------------------------------------------------------------- */
static void print_report(const char *policy, const char *workload, int quantum) {
    if (strcmp(policy, "rr") == 0) {
        printf("Policy: RR (q = %d)   Workload: %s\n", quantum, workload);
    } else {
        printf("Policy: %s   Workload: %s\n", strcmp(policy, "sjf") == 0 ? "SJF" : "FCFS", workload);
    }

    printf("Trace:");
    int end = 0;
    for (int i = 0; i < n_slices; i++) {
        if (i > 0 && i % 8 == 0) {
            printf("\n      ");            /* wrap long traces */
        }
        if (trace[i].from > end) {
            printf(" [%d-%d idle]", end, trace[i].from);
        }
        printf(" [%d-%d P%d]", trace[i].from, trace[i].to, trace[i].id);
        end = trace[i].to;
    }
    printf("\n\n");

    printf("Process  Arrival  Burst  Start  Finish  Turnaround  Waiting  Response\n");
    int total_t = 0, total_w = 0, total_r = 0, done = 0;
    for (int i = 0; i < n; i++) {
        process_t *p = &procs[i];
        if (p->finish < 0) {
            printf("P%-7d %7d %6d   not finished\n", p->id, p->arrival, p->burst);
            continue;
        }
        int t = p->finish - p->arrival;  /* turnaround T = C - A */
        int w = t - p->burst;            /* waiting    W = T - B (one burst, no I/O) */
        int r = p->start - p->arrival;   /* response   R = S - A */
        printf("P%-7d %7d %6d %6d %7d %11d %8d %9d\n",
               p->id, p->arrival, p->burst, p->start, p->finish, t, w, r);
        total_t += t;
        total_w += w;
        total_r += r;
        done++;
    }
    if (done > 0) {
        printf("Average %42.2f %8.2f %9.2f\n",
               (double)total_t / done, (double)total_w / done, (double)total_r / done);
    }
    printf("Context switches: %d\n", context_switches);
}

static int usage(const char *prog) {
    fprintf(stderr, "Usage: %s fcfs|sjf|rr lecture|lab [quantum]\n", prog);
    fprintf(stderr, "Example: %s rr lecture 2\n", prog);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        return usage(argv[0]);
    }
    const char *policy = argv[1];
    const char *workload = argv[2];
    int quantum = (argc == 4) ? atoi(argv[3]) : 2;

    if (load_workload(workload) != 0) {
        return usage(argv[0]);
    }
    if (strcmp(policy, "fcfs") == 0) {
        schedule_nonpreemptive(0);
    } else if (strcmp(policy, "sjf") == 0) {
        schedule_nonpreemptive(1);
    } else if (strcmp(policy, "rr") == 0) {
        if (quantum < 1) {
            fprintf(stderr, "The quantum must be at least 1.\n");
            return 1;
        }
        schedule_rr(quantum);
    } else {
        return usage(argv[0]);
    }
    print_report(policy, workload, quantum);
    return 0;
}
