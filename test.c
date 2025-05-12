#include "Unity/src/unity.h"
#include "ext/uthash/src/uthash.h"
#include "ext/uthash/src/utstring.h"

#include "doubly_linked_list.h"
#include "scheduler_round_robin.h"
#include "scheduler_sjf.h"

typedef struct task      task;
typedef struct run_queue run_queue;

// never change or free return value only to be used for printf'ing
// free utbuffer instead
static char* task_tostring(const task* t, UT_string* utbuffer) {
    utstring_clear(utbuffer);
    utstring_printf(utbuffer, "(%d)", t->pid);
    return utstring_body(utbuffer);
}

// structure used to store tasks* in a hash table
typedef struct hshable_taskptr {
    task*          t;
    UT_hash_handle hh;
} hshable_taskptr;

static hshable_taskptr* hshable_taskptr_new(task* t) {
    hshable_taskptr* h = malloc(sizeof(hshable_taskptr));
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

static task* task_new(int pid, int state) {
    task* t    = malloc(sizeof(task));
    t->pid     = pid;
    t->state   = state;
    t->prev    = NULL;
    t->next    = NULL;
    t->runtime = 3;
    return t;
}

// builds a run queue of length len
static run_queue* run_queue_new(size_t len) {
    run_queue* rq = malloc(sizeof(run_queue));
    rq->head      = NULL;
    rq->n_tasks   = 0;
    if (len == 0) return rq;
    // add first
    int pid  = 0;
    rq->head = task_new(pid, READY);
    ++rq->n_tasks;
    --len;
    task* prev = rq->head;
    while (len > 0) {
        ++rq->n_tasks;
        --len;
        ++pid;
        task* curr = task_new(pid, READY);
        prev->next = curr;
        curr->prev = prev;
        prev       = curr;
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

void setUp(void) {
    utstring_new(logstr);
    utstring_new(errstr);
    utstring_new(taskstr1);
    utstring_new(taskstr2);
    utstring_new(taskstr3);
    utstring_printf(logstr, "LOG: ");
    rq_empty      = run_queue_new(0);
    rq_3          = run_queue_new(3);
    rq_stud_empty = run_queue_new(0);
    rq_stud_3     = run_queue_new(3);
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
        utstring_clear(errstr);
        utstring_printf(errstr, "rq->head->prev expected NULL was !NULL %s", utstring_body(logstr));
        TEST_ASSERT_NULL_MESSAGE(rq->head->prev, utstring_body(errstr));
        utstring_printf(logstr, "%s<->", task_tostring(rq->head, taskstr1));

        // add head to seen tasks
        hshable_taskptr* ht = hshable_taskptr_new(rq->head);
        HASH_ADD_PTR(task_table, t, ht);
        task* curr = rq->head->next;
        task* prev = rq->head;
        while (curr) { // loop through rq and check ptr validity
            ++len;
            // check for repeat
            hshable_taskptr* res = NULL;
            HASH_FIND_PTR(task_table, &curr, res);
            // if loop present
            utstring_clear(errstr);
            utstring_printf(errstr, "Looped element %s at pos %ld (second occurence) %s",
                            task_tostring(curr, taskstr1), len - 1, utstring_body(logstr));
            TEST_ASSERT_NULL_MESSAGE(res, utstring_body(errstr));

            // check curr->prev isn't null
            utstring_clear(errstr);
            utstring_printf(errstr, "Element %s at pos %ld previous was NULL %s",
                            utstring_body(taskstr1), len, utstring_body(logstr));
            TEST_ASSERT_NOT_NULL_MESSAGE(curr->prev, utstring_body(errstr));
            // check curr->prev isn't wrong
            utstring_clear(errstr);
            utstring_printf(errstr, "Element '%s' at pos %ld previous expected '%s' was '%s' %s",
                            utstring_body(taskstr1), len, task_tostring(prev, taskstr2),
                            task_tostring(curr->prev, taskstr3), utstring_body(logstr));
            TEST_ASSERT_EQUAL_PTR_MESSAGE(prev, curr->prev, utstring_body(errstr));

            utstring_printf(logstr, "<->%s", utstring_body(taskstr1));
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
    utstring_clear(errstr);
    utstring_printf(errstr, "Runque length incorrect %s", utstring_body(logstr));
    TEST_ASSERT_EQUAL_INT_MESSAGE(rq->n_tasks, studrq->n_tasks, utstring_body(errstr));
    task* t     = rq->head;
    task* studt = studrq->head;
    while (t) {
        utstring_clear(errstr);
        utstring_printf(errstr, "Stud pid expected = model pid %s", utstring_body(logstr));
        TEST_ASSERT_EQUAL_INT_MESSAGE(t->pid, studt->pid, utstring_body(errstr));
        utstring_clear(errstr);
        utstring_printf(errstr, "Stud state expected = model state %s", utstring_body(logstr));
        TEST_ASSERT_EQUAL_INT_MESSAGE(t->state, studt->state, utstring_body(errstr));
        t     = t->next;
        studt = studt->next;
    }
    utstring_printf(logstr, "Runque = model ");
}

void test_1_RunQueue_empty_check(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    TEST_ASSERT_MESSAGE(stud_rq_empty(rq_stud_empty),
                        "stud_rq_empty on emptyqueue expected true returned false");
    validate_rq(rq_stud_empty);
    compare_rq(rq_empty, rq_stud_empty);

    utstring_printf(logstr, "Testing runqueue3. ");
    TEST_ASSERT_MESSAGE(!stud_rq_empty(rq_stud_3),
                        "stud_rq_empty on runqueue3 expected false returned true");
    validate_rq(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);
}

void test_2_RunQueue_head(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    TEST_ASSERT_NULL_MESSAGE(stud_rq_head(rq_stud_empty),
                             "stud_rq_head on emptyqueue expected NULL returned !NULL");
    validate_rq(rq_stud_empty);
    compare_rq(rq_empty, rq_stud_empty);

    utstring_printf(logstr, "Testing runqueue3. ");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(rq_stud_3->head, stud_rq_head(rq_stud_3),
                                  "stud_rq_head on runqueue3");
    validate_rq(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);
}

void test_3_RunQueue_tail(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    TEST_ASSERT_NULL_MESSAGE(stud_rq_tail(rq_stud_empty),
                             "stud_rq_tail on emptyqueue expected NULL returned !NULL");
    validate_rq(rq_stud_empty);
    compare_rq(rq_empty, rq_stud_empty);

    utstring_printf(logstr, "Testing runqueue3. ");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(rq_stud_3->head->next->next, stud_rq_tail(rq_stud_3),
                                  "stud_rq_tail on runqueue3");
    validate_rq(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);
}

void test_4_RunQueue_enque(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    // model
    task* enq_task = task_new(0, READY);
    rq_empty->head = enq_task;
    ++rq_empty->n_tasks;
    // stud
    enq_task = task_new(0, READY);
    TEST_ASSERT_MESSAGE(stud_rq_enqueue(rq_stud_empty, enq_task),
                        "stud_rq_enque on emptyqueue expected true returned false");

    validate_rq(rq_stud_empty);
    compare_rq(rq_empty, rq_stud_empty);

    utstring_printf(logstr, "Testing runqueue3. ");
    // model
    ++rq_3->n_tasks;
    enq_task             = task_new(3, TERMINATED);
    enq_task->prev       = rq_3->head->next->next;
    enq_task->prev->next = enq_task;
    // stud
    enq_task = task_new(3, TERMINATED);
    TEST_ASSERT_MESSAGE(stud_rq_enqueue(rq_stud_3, enq_task), "stud_rq_enque on runqueue3 expected true returned false");

    validate_rq(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_1_RunQueue_empty_check);
    RUN_TEST(test_2_RunQueue_head);
    RUN_TEST(test_3_RunQueue_tail);
    RUN_TEST(test_4_RunQueue_enque);
    return UNITY_END();
}