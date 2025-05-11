#include "Unity/src/unity.h"
#include "ext/uthash/src/uthash.h"

#include "doubly_linked_list.h"
#include "scheduler_round_robin.h"
#include "scheduler_sjf.h"

typedef struct task      task;
typedef struct run_queue run_queue;

static const size_t TASKBUFFSIZE = 20;

static void task_tostring(const task* t, char* out) { snprintf(out, TASKBUFFSIZE, "%d", t->pid); }

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
    t->runtime = 5;
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

static void validate_rq(const run_queue* rq) {
    size_t len = 0;
    // string buffers for error messages
    static const size_t ERRBUFFSIZE = 200;
    char                errbuff[ERRBUFFSIZE];
    char                taskbuff1[TASKBUFFSIZE];
    char                taskbuff2[TASKBUFFSIZE];
    char                taskbuff3[TASKBUFFSIZE];

    if (rq->head) {
        ++len;
        TEST_ASSERT_NULL_MESSAGE(rq->head->prev, "head->prev != NULL");
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
            task_tostring(curr, taskbuff1);
            snprintf(errbuff, ERRBUFFSIZE, "Looped element '%s' at pos %ld (second occurence)",
                     taskbuff1, len);
            TEST_ASSERT_NULL_MESSAGE(res, errbuff);

            // check curr->prev isn't null
            snprintf(errbuff, ERRBUFFSIZE, "Element '%s' at pos %ld previous is NULL", taskbuff1,
                     len);
            TEST_ASSERT_NOT_NULL_MESSAGE(curr->prev, errbuff);
            // check curr->prev isn't wrong
            task_tostring(prev, taskbuff2);
            task_tostring(curr->prev, taskbuff3);
            snprintf(errbuff, ERRBUFFSIZE,
                     "Element '%s' at pos %ld previous expected '%s' got '%s'", taskbuff1, len,
                     taskbuff2, taskbuff3);
            TEST_ASSERT_EQUAL_PTR_MESSAGE(prev, curr->prev, errbuff);

            // add to hashtable and iterate
            ht = hshable_taskptr_new(curr);
            HASH_ADD_PTR(task_table, t, ht);
            prev = curr;
            curr = curr->next;
        }
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(len, rq->n_tasks, "length of dll != n_tasks");
    task_table_clear();
}

// globals to be used in tests
run_queue* rq_empty;
run_queue* rq_5;

void setUp(void) {
    rq_empty = run_queue_new(0);
    rq_5     = run_queue_new(5);
}

void tearDown(void) {
    // free hashtable if still has contents due to test fail
    task_table_clear();
    run_queue_free(rq_empty);
    run_queue_free(rq_5);
}

void test_1_RunQueue_validation_empty(void) { validate_rq(rq_empty); }

void test_2_RunQueue_validation_5(void) { validate_rq(rq_5); }

void test_3_RunQueue_validation_5_broken_n_tasks(void) {
    --rq_5->n_tasks;
    validate_rq(rq_5);
}

void test_4_RunQueue_validation_5_broken_prev(void) {
    rq_5->head->next->next->prev = NULL;
    validate_rq(rq_5);
}

void test_5_RunQueue_validation_5_broken_loop(void) {
    rq_5->head->next->next->next = rq_5->head->next;
    validate_rq(rq_5);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_1_RunQueue_validation_empty);
    RUN_TEST(test_2_RunQueue_validation_5);
    RUN_TEST(test_3_RunQueue_validation_5_broken_n_tasks);
    RUN_TEST(test_4_RunQueue_validation_5_broken_prev);
    RUN_TEST(test_5_RunQueue_validation_5_broken_loop);
    return UNITY_END();
}