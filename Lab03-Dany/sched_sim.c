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
static int process_count;           /* number of processes in the workload */
static slice_t trace[MAX_SLICES];   /* who ran when, in order */
static int slice_count;
static int context_switches;
static int last_id;                 /* id of the process that ran last, 0 = none yet */

/* Round robin ready queue (FIFO ring buffer) */
static process_t *queue[MAX_PROCS];
static int q_head;                  /* index of the first element */
static int q_count;                 /* number of elements currently in queue */

/* Function Prototypes */
static int load_workload(const char *workload_name);
static void run_slice(process_t *process, int from, int to);
static int next_arrival(int current_time);
static process_t *pick_fcfs(int current_time);
static process_t *pick_sjf(int current_time);
static void schedule_nonpreemptive(int use_sjf);
static void queue_push(process_t *process);
static process_t *queue_pop(void);
static void admit_arrivals(int current_time);
static void schedule_rr(int quantum);
static void print_report(const char *policy, const char *workload_name, int quantum);
static int usage(const char *prog);

/* ---------------------------------------------------------------------------
 * Main
 * ------------------------------------------------------------------------- */
int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        return usage(argv[0]);
    }

    const char *policy = argv[1];
    const char *workload_name = argv[2];
    int quantum = (argc == 4) ? atoi(argv[3]) : 2;

    if (load_workload(workload_name) != 0) {
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

    print_report(policy, workload_name, quantum);
    return 0;
}

/* ---------------------------------------------------------------------------
 * Helper Functions
 * ------------------------------------------------------------------------- */

/* Print usage guide */
static int usage(const char *prog) {
    fprintf(stderr, "Usage: %s fcfs|sjf|rr lecture|lab [quantum]\n", prog);
    fprintf(stderr, "Example: %s rr lecture 2\n", prog);
    return 1;
}

/* Copy a workload table into procs[]. Returns -1 for an unknown name. */
static int load_workload(const char *workload_name) {
    const int (*table)[2];

    if (strcmp(workload_name, "lecture") == 0) {
        table = lecture_workload;
        process_count = (int)(sizeof lecture_workload / sizeof lecture_workload[0]);
    } else if (strcmp(workload_name, "lab") == 0) {
        table = lab_workload;
        process_count = (int)(sizeof lab_workload / sizeof lab_workload[0]);
    } else {
        return -1;
    }

    /* Reset global state */
    slice_count = 0;
    context_switches = 0;
    last_id = 0;
    q_head = 0;
    q_count = 0;

    for (int i = 0; i < process_count; i++) {
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

/* Run process on the CPU from time `from` to time `to`.
 * Counts context switches and records slices for the Gantt trace. */
static void run_slice(process_t *process, int from, int to) {
    /* Count a context switch whenever the CPU starts running a process
     * that is different from the one that ran last.
     * The very first dispatch is not counted as a switch. */
    if (last_id != 0 && process->id != last_id) {
        context_switches++;
    }

    last_id = process->id;                  /* remember who ran last */

    if (process->start < 0) {
        process->start = from;              /* first time on the CPU */
    }

    process->remaining -= to - from;
    if (process->remaining == 0) {
        process->finish = to;
    }

    /* Record the slice, or extend the last one if the same process continues */
    if (slice_count > 0 && trace[slice_count - 1].id == process->id &&
        trace[slice_count - 1].to == from) {
        trace[slice_count - 1].to = to;
    } else if (slice_count < MAX_SLICES) {
        trace[slice_count].id = process->id;
        trace[slice_count].from = from;
        trace[slice_count].to = to;
        slice_count++;
    }
}

/* Earliest arrival time after `current_time` of a process that still needs the CPU.
 * Used when nothing is ready: the CPU idles until then. */
static int next_arrival(int current_time) {
    int next = -1;

    for (int i = 0; i < process_count; i++) {
        if (procs[i].remaining > 0 && procs[i].arrival > current_time &&
            (next < 0 || procs[i].arrival < next)) {
            next = procs[i].arrival;
        }
    }

    return next;
}

/* ---------------------------------------------------------------------------
 * Non-preemptive policies: pick a READY process, run it to completion, repeat.
 * Ready = has arrived (arrival <= current_time) and still needs the CPU.
 * ------------------------------------------------------------------------- */

/* FCFS: the ready process that arrived first (ties: lower id).
 * Returns NULL if nothing is ready. */
static process_t *pick_fcfs(int current_time) {
    process_t *best = NULL;

    for (int i = 0; i < process_count; i++) {
        process_t *process = &procs[i];

        if (process->arrival > current_time || process->remaining == 0) {
            continue;                       /* not ready */
        }

        if (best == NULL || process->arrival < best->arrival) {
            best = process;
        }
    }

    return best;
}

/* SJF: the ready process with the shortest burst.
 * Ties: earlier arrival first, then lower id.
 * Returns NULL if nothing is ready. */
static process_t *pick_sjf(int current_time) {
    process_t *best = NULL;

    for (int i = 0; i < process_count; i++) {
        process_t *process = &procs[i];

        if (process->arrival > current_time || process->remaining == 0) {
            continue;                       /* not ready */
        }

        /* Choose the shortest burst.
         * If bursts are tied, choose the earlier arrival time. */
        if (best == NULL || process->burst < best->burst) {
            best = process;
        } else if (process->burst == best->burst) {
            if (process->arrival < best->arrival) {
                best = process;
            }
        }
    }

    return best;
}

static void schedule_nonpreemptive(int use_sjf) {
    int current_time = 0;
    int finished_count = 0;

    while (finished_count < process_count) {
        process_t *selected_process = use_sjf ? pick_sjf(current_time)
                                              : pick_fcfs(current_time);

        if (selected_process == NULL) {     /* nothing ready: idle until next arrival */
            current_time = next_arrival(current_time);
            continue;
        }

        int end_time = current_time + selected_process->remaining; /* run to completion */
        run_slice(selected_process, current_time, end_time);
        current_time = end_time;
        finished_count++;
    }
}

/* ---------------------------------------------------------------------------
 * Round robin queue functions
 * ------------------------------------------------------------------------- */

static void queue_push(process_t *process) {   /* add at the tail */
    queue[(q_head + q_count) % MAX_PROCS] = process;
    q_count++;
}

static process_t *queue_pop(void) {            /* remove from the head */
    if (q_count == 0) {
        return NULL;
    }
    process_t *process = queue[q_head];
    q_head = (q_head + 1) % MAX_PROCS;
    q_count--;
    return process;
}

/* Put every process that has arrived by `current_time` into the queue (once) */
static void admit_arrivals(int current_time) {
    for (int i = 0; i < process_count; i++) {
        if (!procs[i].queued && procs[i].arrival <= current_time) {
            procs[i].queued = 1;
            queue_push(&procs[i]);
        }
    }
}

/* Round robin scheduling implementation */
static void schedule_rr(int quantum) {
    int current_time = 0;
    int finished_count = 0;

    /* Admit processes that arrive at time 0 */
    admit_arrivals(current_time);

    while (finished_count < process_count) {
        if (q_count == 0) {
            /* 1. If the queue is empty, the CPU idles: jump to next arrival */
            int arrival_time = next_arrival(current_time);
            if (arrival_time < 0) {
                break;
            }

            current_time = arrival_time;
            admit_arrivals(current_time);
            continue;
        }

        /* 2. Pop the head process and run it for quantum or remaining */
        process_t *selected_process = queue_pop();
        if (selected_process == NULL) {
            continue;
        }

        int execution_time = selected_process->remaining < quantum
            ? selected_process->remaining
            : quantum;
        int end_time = current_time + execution_time;

        run_slice(selected_process, current_time, end_time);
        current_time = end_time;

        /* 3. New arrivals join the queue BEFORE putting the current process back */
        admit_arrivals(current_time);

        /* 4. If the process still needs CPU, push it to the tail; otherwise it finished */
        if (selected_process->remaining > 0) {
            queue_push(selected_process);
        } else {
            finished_count++;
        }
    }
}

/* ---------------------------------------------------------------------------
 * Report: trace, per-process metrics and averages
 * ------------------------------------------------------------------------- */
static void print_report(const char *policy, const char *workload_name, int quantum) {
    if (strcmp(policy, "rr") == 0) {
        printf("Policy: RR (q = %d)   Workload: %s\n", quantum, workload_name);
    } else {
        const char *policy_name = strcmp(policy, "sjf") == 0 ? "SJF" : "FCFS";
        printf("Policy: %s   Workload: %s\n", policy_name, workload_name);
    }

    printf("Trace:");
    {
        int previous_end = 0;

        for (int index = 0; index < slice_count; index++) {
            if (index > 0 && index % 8 == 0) {
                printf("\n      ");
            }

            if (trace[index].from > previous_end) {
                printf(" [%d-%d idle]", previous_end, trace[index].from);
            }

            printf(" [%d-%d P%d]", trace[index].from, trace[index].to, trace[index].id);
            previous_end = trace[index].to;
        }
    }
    printf("\n\n");

    printf("Process  Arrival  Burst  Start  Finish  Turnaround  Waiting  Response\n");

    {
        int total_turnaround = 0;
        int total_waiting = 0;
        int total_response = 0;
        int completed_count = 0;

        for (int index = 0; index < process_count; index++) {
            process_t *process = &procs[index];

            if (process->finish < 0) {
                printf("P%-7d %7d %6d   not finished\n",
                       process->id, process->arrival, process->burst);
                continue;
            }

            int turnaround_time = process->finish - process->arrival;  /* T = C - A */
            int waiting_time    = turnaround_time - process->burst;    /* W = T - B */
            int response_time   = process->start - process->arrival;   /* R = S - A */

            printf("P%-7d %7d %6d %6d %7d %11d %8d %9d\n",
                   process->id, process->arrival, process->burst,
                   process->start, process->finish, turnaround_time,
                   waiting_time, response_time);

            total_turnaround += turnaround_time;
            total_waiting += waiting_time;
            total_response += response_time;
            completed_count++;
        }

        if (completed_count > 0) {
            printf("Average %42.2f %8.2f %9.2f\n",
                   (double)total_turnaround / completed_count,
                   (double)total_waiting / completed_count,
                   (double)total_response / completed_count);
        }
    }

    printf("Context switches: %d\n", context_switches);
}