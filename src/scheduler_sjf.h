// WARNING this is just a header file if you want to test your code put in the .c version!

#include "doubly_linked_list.h"
#include "scheduler.h"

void stud_SJF_start(struct run_queue* rq, int pid);
void stud_SJF_elect(struct run_queue* rq);
void stud_SJF_terminate(struct run_queue* rq);
void stud_SJF_clock_tick(struct run_queue* rq);
void stud_SJF_wait(struct run_queue* rq);
void stud_SJF_wake_up(struct run_queue* rq, int pid);
void stud_SJF(struct run_queue* rq, enum events event, int pid);
