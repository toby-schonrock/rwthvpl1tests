#include "test_helpers.h"

void test_1_RunQueue_empty_check(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    TEST_ASSERT_MESSAGE(stud_rq_empty(rq_stud_empty),
                        errfmt("stud_rq_empty on emptyqueue expected true returned false"));
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3. ");
    TEST_ASSERT_MESSAGE(!stud_rq_empty(rq_stud_3),
                        errfmt("stud_rq_empty on runqueue3 expected false returned true"));
    compare_rq(rq_3, rq_stud_3);
}

void test_2_RunQueue_head(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    TEST_ASSERT_NULL_MESSAGE(stud_rq_head(rq_stud_empty),
                             errfmt("stud_rq_head on emptyqueue expected NULL returned !NULL"));
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3. ");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(rq_stud_3->head, stud_rq_head(rq_stud_3),
                                  errfmt("stud_rq_head on runqueue3"));
    compare_rq(rq_3, rq_stud_3);
}

void test_3_RunQueue_tail(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    TEST_ASSERT_NULL_MESSAGE(stud_rq_tail(rq_stud_empty),
                             errfmt("stud_rq_tail on emptyqueue expected NULL returned !NULL"));
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3. ");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(rq_stud_3->head->next->next, stud_rq_tail(rq_stud_3),
                                  errfmt("stud_rq_tail on runqueue3"));
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
                        errfmt("stud_rq_enque on emptyqueue expected true returned false"));

    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3. ");
    // model
    ++rq_3->n_tasks;
    enq_task             = task_new(3, TERMINATED);
    enq_task->prev       = rq_3->head->next->next;
    enq_task->prev->next = enq_task;
    // stud
    enq_task = task_new(3, TERMINATED);
    TEST_ASSERT_MESSAGE(stud_rq_enqueue(rq_stud_3, enq_task),
                        errfmt("stud_rq_enque on runqueue3 expected true returned false"));

    compare_rq(rq_3, rq_stud_3);
}

void test_5_RunQueue_enque_sorted(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    // model
    task* enq_task    = task_new(0, READY);
    enq_task->runtime = 1;
    rq_empty->head    = enq_task;
    ++rq_empty->n_tasks;
    // stud
    enq_task          = task_new(0, READY);
    enq_task->runtime = 1;
    TEST_ASSERT_MESSAGE(stud_rq_enqueue_sorted(rq_stud_empty, enq_task),
                        errfmt("stud_rq_enque_sorted on emptyqueue expected true returned false"));
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 sortenqueue to back. ");

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 sortenqueue to back. ");
    // model
    enq_task             = task_new(3, READY);
    enq_task->runtime    = 12;
    enq_task->prev       = rq_3->head->next->next;
    enq_task->prev->next = enq_task;
    ++rq_3->n_tasks;
    // stud
    enq_task          = task_new(3, READY);
    enq_task->runtime = 12;
    TEST_ASSERT_MESSAGE(stud_rq_enqueue_sorted(rq_stud_3, enq_task),
                        errfmt("stud_rq_enque_sorted on runqueue3 expected true returned false"));
    compare_rq(rq_3, rq_stud_3);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 sortenqueue to second last. ");
    // model
    enq_task             = task_new(4, BLOCKED);
    enq_task->runtime    = 11;
    enq_task->next       = rq_3->head->next->next->next;
    enq_task->next->prev = enq_task;
    enq_task->prev       = rq_3->head->next->next;
    enq_task->prev->next = enq_task;
    ++rq_3->n_tasks;
    // stud
    enq_task          = task_new(4, BLOCKED);
    enq_task->runtime = 11;
    TEST_ASSERT_MESSAGE(stud_rq_enqueue_sorted(rq_stud_3, enq_task),
                        errfmt("stud_rq_enque_sorted on runqueue3 expected true returned false"));
    compare_rq(rq_3, rq_stud_3);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 sortenqueue to second pos due to running. (note "
                            "running tasks should be skipped)"); // see moodle
    // model
    enq_task             = task_new(5, READY);
    enq_task->runtime    = 1;
    enq_task->next       = rq_3->head->next;
    enq_task->next->prev = enq_task;
    enq_task->prev       = rq_3->head;
    enq_task->prev->next = enq_task;
    ++rq_3->n_tasks;
    // stud
    enq_task          = task_new(5, READY);
    enq_task->runtime = 1;
    TEST_ASSERT_MESSAGE(stud_rq_enqueue_sorted(rq_stud_3, enq_task),
                        errfmt("stud_rq_enque_sorted on runqueue3 expected true returned false"));
    compare_rq(rq_3, rq_stud_3);
}

void test_6_RunQueue_prepend(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    // model
    task* enq_task = task_new(0, READY);
    rq_empty->head = enq_task;
    ++rq_empty->n_tasks;
    // stud
    enq_task = task_new(0, READY);
    TEST_ASSERT_MESSAGE(stud_rq_prepend(rq_stud_empty, enq_task),
                        errfmt("stud_rq_prepend on emptyqueue expected true returned false"));

    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3. ");
    // model
    ++rq_3->n_tasks;
    enq_task         = task_new(3, TERMINATED);
    enq_task->next   = rq_3->head;
    rq_3->head->prev = enq_task;
    rq_3->head       = enq_task;
    // stud
    enq_task = task_new(3, TERMINATED);
    TEST_ASSERT_MESSAGE(stud_rq_prepend(rq_stud_3, enq_task),
                        errfmt("stud_rq_prepend on runqueue3 expected true returned false"));

    compare_rq(rq_3, rq_stud_3);
}

void test_7_RunQueue_find(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    TEST_ASSERT_NULL_MESSAGE(stud_rq_find(rq_stud_empty, 0),
                             errfmt("stud_rq_find on emptyqueue expected NULL"));
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 find 0. ");
    task* res = stud_rq_find(rq_stud_3, 0);
    compare_rq(rq_3, rq_stud_3);
    TEST_ASSERT_NOT_NULL_MESSAGE(res, errfmt("stud_rq_find 0 on runqueue3 expected !NULL"));
    TEST_ASSERT_EQUAL_PTR_MESSAGE(rq_stud_3->head, res,
                                  errfmt("stud_rq_find 0 on runqueue3 expected %s was %s",
                                         task_tostring(rq_3->head, taskstr1),
                                         task_tostring(res, taskstr2)));

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 find 2. ");
    res = stud_rq_find(rq_stud_3, 2);
    compare_rq(rq_3, rq_stud_3);
    TEST_ASSERT_NOT_NULL_MESSAGE(res, errfmt("stud_rq_find 2 on runqueue3 expected !NULL"));
    TEST_ASSERT_EQUAL_PTR_MESSAGE(rq_stud_3->head->next->next, res,
                                  errfmt("stud_rq_find 2 on runqueue3 expected %s was %s",
                                         task_tostring(rq_3->head->next->next, taskstr1),
                                         task_tostring(res, taskstr2)));

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 find 42. ");
    TEST_ASSERT_NULL_MESSAGE(stud_rq_find(rq_stud_3, 42),
                             errfmt("stud_rq_find 42 on runqueue3 expected NULL"));
}

void test_8_RunQueue_length(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    TEST_ASSERT_EQUAL_INT_MESSAGE(stud_rq_length(rq_stud_empty), 0,
                                  errfmt("stud_rq_length on emptyqueue expected 0"));
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3. ");
    TEST_ASSERT_EQUAL_INT_MESSAGE(stud_rq_length(rq_stud_3), 3,
                                  errfmt("stud_rq_length on runqueue3 expected 3"));
    compare_rq(rq_3, rq_stud_3);
}

void test_9_runqueue_destroy(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    stud_rq_destroy(rq_stud_empty);
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3. ");
    stud_rq_destroy(rq_stud_3);
    compare_rq(rq_empty, rq_stud_3);
}

void test_10_SJF_start(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    // model
    rq_empty->head          = task_new(0, READY);
    rq_empty->head->runtime = 0;
    ++rq_empty->n_tasks;
    // stud
    stud_SJF_start(rq_stud_empty, 0);
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 start 3 t0. ");
    // model
    task* t       = task_new(3, READY);
    t->runtime    = 0;
    t->next       = rq_3->head->next;
    t->next->prev = t;
    t->prev       = rq_3->head;
    t->prev->next = t;
    ++rq_3->n_tasks;
    // stud
    stud_SJF_start(rq_stud_3, 3);
    compare_rq(rq_3, rq_stud_3);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 duplicate element(do nothing). ");
    // model do nothing lol
    // stud
    stud_SJF_start(rq_stud_3, 3);
    compare_rq(rq_3, rq_stud_3);
}

void test_12_13_SJF_elect(void) {
    utstring_printf(logstr, "Testing runqueue3 alreay running(do nothing). ");
    // model do nothing lol
    stud_SJF_elect(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 only last task READY. ");
    // setup
    rq_3->head->state            = BLOCKED;
    rq_3->head->next->state      = TERMINATED;
    rq_stud_3->head->state       = BLOCKED;
    rq_stud_3->head->next->state = TERMINATED;
    // model
    task* old_tail       = rq_3->head->next->next;
    old_tail->state      = RUNNING;
    old_tail->prev->next = NULL;
    old_tail->prev       = NULL;
    old_tail->next       = rq_3->head;
    rq_3->head->prev     = old_tail;
    rq_3->head           = old_tail;
    // stud
    stud_SJF_elect(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 middle task READY. ");
    // setup
    rq_3->head->state            = BLOCKED;
    rq_3->head->next->state      = READY;
    rq_stud_3->head->state       = BLOCKED;
    rq_stud_3->head->next->state = READY;
    // model
    task* middle       = rq_3->head->next;
    middle->state      = RUNNING;
    middle->prev->next = middle->next;
    middle->prev       = NULL;
    middle->next->prev = rq_3->head;
    middle->next       = rq_3->head;
    rq_3->head->prev   = middle;
    rq_3->head         = middle;
    // stud
    stud_SJF_elect(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 front task READY. ");
    // setup
    rq_3->head->state      = READY;
    rq_stud_3->head->state = READY;
    // model
    rq_3->head->state = RUNNING;
    // stud
    stud_SJF_elect(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);
}

void test_14_SJF_terminate(void) {
    utstring_printf(logstr, "Testing runqueue3 terminate. ");
    // model
    task* old_head         = rq_3->head;
    rq_3->head             = rq_3->head->next; // set new head
    rq_3->head->prev       = NULL;
    old_head->prev         = rq_3->head->next;
    rq_3->head->next->next = old_head;
    old_head->next         = NULL;
    old_head->state        = TERMINATED;
    rq_3->head->state      = RUNNING;
    // stud
    stud_SJF_terminate(rq_stud_3);
    // CHECKRUNTIME = false;
    compare_rq(rq_3, rq_stud_3);
    // CHECKRUNTIME = true;
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_1_RunQueue_empty_check);
    RUN_TEST(test_2_RunQueue_head);
    RUN_TEST(test_3_RunQueue_tail);
    RUN_TEST(test_4_RunQueue_enque);
    RUN_TEST(test_5_RunQueue_enque_sorted);
    RUN_TEST(test_6_RunQueue_prepend);
    RUN_TEST(test_7_RunQueue_find);
    RUN_TEST(test_8_RunQueue_length);
    RUN_TEST(test_9_runqueue_destroy);
    RUN_TEST(test_10_SJF_start);
    RUN_TEST(test_12_13_SJF_elect);
    RUN_TEST(test_14_SJF_terminate);
    return UNITY_END();
}