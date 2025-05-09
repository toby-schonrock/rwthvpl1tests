#ifndef SCHEDULER_H__
#define SCHEDULER_H__

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define QUANTUM 10

enum states {
    BLOCKED = 0,
    RUNNING,
    READY,
    TERMINATED,
};

enum events {
    start = 0,
    wait,
    wake_up,
    terminate,
    clock_tick,
};

struct task {
    int pid;
    enum states state;
    struct task* prev;
    struct task* next;
    int runtime;
};

struct run_queue {
    struct task* head;
    size_t n_tasks;
    int time_counter;
};

#endif // SCHEDULER_H__
