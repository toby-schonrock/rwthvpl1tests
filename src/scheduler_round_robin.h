// WARNING this is just a header file if you want to test your code put in the .c version!

#include "doubly_linked_list.h"
#include "scheduler.h"

void stud_RR_start(struct run_queue* rq, int pid);
void stud_RR_elect(struct run_queue* rq);
void stud_RR_terminate(struct run_queue* rq);
void stud_RR_clock_tick(struct run_queue* rq);
void stud_RR_wait(struct run_queue* rq);
void stud_RR_wake_up(struct run_queue* rq, int pid);
void stud_RR(struct run_queue* rq, enum events event, int pid);
