// WARNING this is just a header file if you want to test your code put in the .c version!

#include "scheduler.h"
#include <stdbool.h>

bool         stud_rq_empty(struct run_queue const* rq);
struct task* stud_task_create(int pid, enum states state);
void         stud_task_free(struct task* task);
void         stud_rq_destroy(struct run_queue* rq);
struct task* stud_rq_find(struct run_queue* rq, int pid);
struct task* stud_rq_head(struct run_queue* rq);
struct task* stud_rq_tail(struct run_queue* rq);
bool         stud_rq_enqueue(struct run_queue* rq, struct task* task);
bool         stud_rq_enqueue_sorted(struct run_queue* rq, struct task* task);
bool         stud_rq_prepend(struct run_queue* rq, struct task* task);
size_t       stud_rq_length(struct run_queue* rq);
