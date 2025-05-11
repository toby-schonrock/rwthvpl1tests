#include "Unity/src/unity.h"
#include "ext/uthash/src/uthash.h"

#include "doubly_linked_list.h"
#include "scheduler_round_robin.h"
#include "scheduler_sjf.h"

typedef struct task      task;
typedef struct run_queue run_queue;

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

static char* task_tostring(const task* t) {
    char* str = malloc(sizeof(char) * 5);
    sprintf(str, "%d", t->pid);
    return str;
}

static void validate_rq(const run_queue* rq) {
    size_t len = 0;
    char   errstring[200];

    // create hash table to prevent inifnite looping
    hshable_taskptr* task_table = NULL;

    if (!rq->head) {
        ++len;
        if (rq->head->prev) {
            TEST_ASSERT_NULL_MESSAGE(rq->head->prev, "head->prev != NULL");
        }
        HASH_ADD_PTR(task_table, t, hshable_taskptr_new(rq->head));
        task* curr = rq->head->next;
        task* prev = rq->head;
        while (curr) { // loop through rq and check ptr validity
            ++len;
            // check for repeat
            hshable_taskptr* res;
            HASH_FIND_PTR(task_table, curr, res);
            sprintf(errstring, "Looped element '%s' at pos %ld (second occurence)",
                    task_tostring(curr), len);
            TEST_ASSERT_NULL_MESSAGE(res, errstring);

            // check curr->prev isn't null
            sprintf(errstring, "Element '%s' at pos %ld previous is NULL", task_tostring(curr),
                    len);
            TEST_ASSERT_NOT_NULL_MESSAGE(curr->prev, errstring);
            // check curr->prev isn't wrong
            sprintf(errstring, "Element '%s' at pos %ld previous expected '%s' got '%s'",
                    task_tostring(curr), len, task_tostring(prev), task_tostring(curr->prev));
            TEST_ASSERT_EQUAL_PTR(prev, curr->prev);

            // add to hashtable and iterate
            HASH_ADD_PTR(task_table, t, hshable_taskptr_new(curr));
            curr = curr->next;
        }
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(len, rq->n_tasks, "length of dll != n_tasks");
}

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_1_RunQueue_empty_check(void) {
    TEST_ASSERT_MESSAGE(false, "ahhh");
    printf("boo\n");
}

void test_test2(void) { TEST_ASSERT_EQUAL_INT_MESSAGE(4, 5, "4=5 !?"); }

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_1_RunQueue_empty_check);
    RUN_TEST(test_test2);
    return UNITY_END();
}