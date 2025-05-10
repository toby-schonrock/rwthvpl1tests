#include "doubly_linked_list.h"
#include "scheduler_round_robin.h"
#include "scheduler_sjf.h"

int main() {
	printf("Test 1 passed blah blah blah\n");
    struct run_queue* rq = malloc(sizeof(struct run_queue));
    stud_SJF_start(rq, 1);
    free(rq);
	return 0;
}