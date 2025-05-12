#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "Unity/src/unity.h"
#include "ext/uthash/src/uthash.h"
#include "ext/uthash/src/utstring.h"

#include "doubly_linked_list.h"
#include "scheduler_round_robin.h"
#include "scheduler_sjf.h"

typedef struct task      task;
typedef struct run_queue run_queue;
typedef enum states      states;

// never change or free return value only to be used for printf'ing
// free utbuffer instead
static char* task_tostring(const task* t, UT_string* utbuffer) {
    utstring_clear(utbuffer);
    utstring_printf(utbuffer, "(%d t%d ", t->pid, t->runtime);
    switch (t->state) {
    case BLOCKED:
        utstring_printf(utbuffer, "BLOC)");
        break;
    case RUNNING:
        utstring_printf(utbuffer, "RUN)");
        break;
    case READY:
        utstring_printf(utbuffer, "RDY)");
        break;
    case TERMINATED:
        utstring_printf(utbuffer, "TERM)");
        break;
    }
    return utstring_body(utbuffer);
}

static task* task_new(int pid, states state) {
    task* t    = (task*)malloc(sizeof(task));
    t->pid     = pid;
    t->state   = state;
    t->prev    = NULL;
    t->next    = NULL;
    t->runtime = 3;
    return t;
}

// structure used to store tasks* in a hash table
typedef struct hshable_taskptr {
    task*          t;
    UT_hash_handle hh;
} hshable_taskptr;

static hshable_taskptr* hshable_taskptr_new(task* t) {
    hshable_taskptr* h = (hshable_taskptr*)malloc(sizeof(hshable_taskptr));
    h->t               = t;
    return h;
}

// hash table used to track task*
// must be global so can be setup and free'd by setUp() and tearDown()
hshable_taskptr* task_table = NULL;

void task_table_clear() {
    hshable_taskptr *curr, *tmp;
    HASH_ITER(hh, task_table, curr, tmp) {
        HASH_DEL(task_table, curr); /* delete; users advances to next */
        free(curr);                 /* optional- if you want to free  */
    }
}

// builds a run queue of length len
static run_queue* run_queue_new(size_t len) {
    run_queue* rq = (run_queue*)malloc(sizeof(run_queue));
    rq->head      = NULL;
    rq->n_tasks   = 0;
    if (len == 0) return rq;
    // add first
    rq->head = task_new(rq->n_tasks, RUNNING);
    ++rq->n_tasks;
    task* prev = rq->head;
    while (rq->n_tasks < len) {
        ++rq->n_tasks;
        task* curr    = task_new(rq->n_tasks, READY);
        curr->runtime = rq->n_tasks + 2;
        prev->next    = curr;
        curr->prev    = prev;
        prev          = curr;
    }
    return rq;
}

static void run_queue_free(run_queue* rq) {
    task*            t   = rq->head;
    hshable_taskptr* res = NULL;
    while (t && !res) {
        // add t to hashtable
        hshable_taskptr* ht = hshable_taskptr_new(t);
        HASH_ADD_PTR(task_table, t, ht);
        task* temp = t->next;
        free(t);
        t = temp;
        if (t) HASH_FIND_PTR(task_table, &t, res);
    }
    free(rq);
    task_table_clear();
}

// globals to be used in tests
UT_string* logstr;
UT_string* errstr;
UT_string* taskstr1;
UT_string* taskstr2;
UT_string* taskstr3;

run_queue* rq_empty;
run_queue* rq_3;
run_queue* rq_stud_empty;
run_queue* rq_stud_3;

// printf wrapper that appends log
char* errfmt(char* fmt, ...) {
    static char buff[200];
    va_list     args;
    va_start(args, fmt);
    vsnprintf(buff, 200, fmt, args);
    va_end(args);
    utstring_clear(errstr);
    utstring_printf(errstr, "%s %s", buff, utstring_body(logstr));
    return utstring_body(errstr);
}

void clear_log() {
    utstring_clear(logstr);
    utstring_printf(logstr, "LOG: ");
}

void setUp(void) {
    utstring_new(logstr);
    utstring_new(errstr);
    utstring_new(taskstr1);
    utstring_new(taskstr2);
    utstring_new(taskstr3);
    clear_log();
    rq_empty                             = run_queue_new(0);
    rq_3                                 = run_queue_new(3);
    rq_3->head->next->next->runtime      = 10; // make end runtime 10
    rq_stud_empty                        = run_queue_new(0);
    rq_stud_3                            = run_queue_new(3);
    rq_stud_3->head->next->next->runtime = 10; // make end runtime 10
}

void tearDown(void) {
    task_table_clear(); // free hashtable if still has contents due to test fail
    utstring_free(logstr);
    utstring_free(errstr);
    utstring_free(taskstr1);
    utstring_free(taskstr2);
    utstring_free(taskstr3);
    run_queue_free(rq_empty);
    run_queue_free(rq_3);
    run_queue_free(rq_stud_empty);
    run_queue_free(rq_stud_3);
}

// checks legnth, prev/next pointers, loops
static void validate_rq(const run_queue* rq) {
    size_t len = 0;
    utstring_printf(logstr, "Validating runqueue: ");

    if (rq->head) {
        ++len;
        TEST_ASSERT_NULL_MESSAGE(rq->head->prev, errfmt("rq->head->prev expected NULL was !NULL"));
        utstring_printf(logstr, "%s", task_tostring(rq->head, taskstr1));

        // add head to seen tasks
        hshable_taskptr* ht = hshable_taskptr_new(rq->head);
        HASH_ADD_PTR(task_table, t, ht);
        task* curr = rq->head->next;
        task* prev = rq->head;
        while (curr) { // loop through rq and check ptr validity
            ++len;
            utstring_printf(logstr, "<->%s", task_tostring(curr, taskstr1));

            // check for repeat
            hshable_taskptr* res = NULL;
            HASH_FIND_PTR(task_table, &curr, res);
            // if loop present
            TEST_ASSERT_NULL_MESSAGE(res, errfmt("Looped element %s at pos %ld (second occurence)",
                                                 task_tostring(curr, taskstr1), len - 1));

            // check curr->prev isn't null
            TEST_ASSERT_NOT_NULL_MESSAGE(
                curr->prev,
                errfmt("Element %s at pos %ld previous was NULL expected %s",
                       task_tostring(curr, taskstr1), len - 1, task_tostring(prev, taskstr2)));
            // check curr->prev isn't wrong
            TEST_ASSERT_EQUAL_PTR_MESSAGE(
                prev, curr->prev,
                errfmt("Element %s at pos %ld previous expected %s was %s", utstring_body(taskstr1),
                       len - 1, task_tostring(prev, taskstr2),
                       task_tostring(curr->prev, taskstr3)));

            // add to hashtable and iterate
            ht = hshable_taskptr_new(curr);
            HASH_ADD_PTR(task_table, t, ht);
            prev = curr;
            curr = curr->next;
        }
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(len, rq->n_tasks, "length of runqueue != n_tasks");
    utstring_printf(logstr, " Validation successful ");
    task_table_clear();
}

// compares PID's and states between model and stud rqs
// should call validate first does not do invalidity checking
static void compare_rq(const run_queue* rq, const run_queue* studrq) {
    utstring_printf(logstr, "Comparing to model runqueue: ");
    TEST_ASSERT_EQUAL_INT_MESSAGE(rq->n_tasks, studrq->n_tasks, errfmt("Runque length incorrect"));
    task* t     = rq->head;
    task* studt = studrq->head;
    if (t) utstring_printf(logstr, "%s", task_tostring(t, taskstr1));
    while (t) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(t->pid, studt->pid,
                                      errfmt("Stud pid differs from model pid"));
        TEST_ASSERT_EQUAL_INT_MESSAGE(t->state, studt->state,
                                      errfmt("Stud state differs from model state"));
        TEST_ASSERT_EQUAL_INT_MESSAGE(t->runtime, studt->runtime,
                                      errfmt("Stud runtime differs from model runtime"));
        t     = t->next;
        studt = studt->next;
        if (t) utstring_printf(logstr, "<->%s", task_tostring(t, taskstr1));
    }
    utstring_printf(logstr, " runque matches model! ");
}