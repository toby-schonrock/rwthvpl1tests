#include "test_helpers.h"

void test_1_RunQueue_empty_check(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    TEST_ASSERT_MESSAGE(stud_rq_empty(rq_stud_empty),
                        errfmt("stud_rq_empty on emptyqueue expected true returned false"));
    validate_rq(rq_stud_empty);
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3. ");
    TEST_ASSERT_MESSAGE(!stud_rq_empty(rq_stud_3),
                        errfmt("stud_rq_empty on runqueue3 expected false returned true"));
    validate_rq(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);
}

void test_2_RunQueue_head(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    TEST_ASSERT_NULL_MESSAGE(stud_rq_head(rq_stud_empty),
                             errfmt("stud_rq_head on emptyqueue expected NULL returned !NULL"));
    validate_rq(rq_stud_empty);
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3. ");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(rq_stud_3->head, stud_rq_head(rq_stud_3),
                                  errfmt("stud_rq_head on runqueue3"));
    validate_rq(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);
}

void test_3_RunQueue_tail(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    TEST_ASSERT_NULL_MESSAGE(stud_rq_tail(rq_stud_empty),
                             errfmt("stud_rq_tail on emptyqueue expected NULL returned !NULL"));
    validate_rq(rq_stud_empty);
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3. ");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(rq_stud_3->head->next->next, stud_rq_tail(rq_stud_3),
                                  errfmt("stud_rq_tail on runqueue3"));
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
                        errfmt("stud_rq_enque on emptyqueue expected true returned false"));

    validate_rq(rq_stud_empty);
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

    validate_rq(rq_stud_3);
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
    validate_rq(rq_stud_empty);
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
    validate_rq(rq_stud_3);
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
    validate_rq(rq_stud_3);
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
    validate_rq(rq_stud_3);
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

    validate_rq(rq_stud_empty);
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

    validate_rq(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);
}

void test_7_RunQueue_find(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    TEST_ASSERT_NULL_MESSAGE(stud_rq_find(rq_stud_empty, 0),
                             errfmt("stud_rq_find on emptyqueue expected NULL"));
    validate_rq(rq_stud_empty);
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 find 0. ");
    task* res = stud_rq_find(rq_stud_3, 0);
    validate_rq(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);
    TEST_ASSERT_NOT_NULL_MESSAGE(res, errfmt("stud_rq_find 0 on runqueue3 expected !NULL"));
    TEST_ASSERT_EQUAL_PTR_MESSAGE(rq_stud_3->head, res,
                                  errfmt("stud_rq_find 0 on runqueue3 expected %s was %s",
                                         task_tostring(rq_3->head, taskstr1),
                                         task_tostring(res, taskstr2)));

    clear_log();
    utstring_printf(logstr, "Testing runqueue3 find 2. ");
    res = stud_rq_find(rq_stud_3, 2);
    validate_rq(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);
    TEST_ASSERT_NOT_NULL_MESSAGE(res, errfmt("stud_rq_find 2 on runqueue3 expected !NULL"));
    TEST_ASSERT_EQUAL_PTR_MESSAGE(rq_stud_3->head->next, res,
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
    validate_rq(rq_stud_empty);
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3. ");
    TEST_ASSERT_EQUAL_INT_MESSAGE(stud_rq_length(rq_stud_3), 3,
                                  errfmt("stud_rq_find on emptyqueue expected 3"));
    validate_rq(rq_stud_3);
    compare_rq(rq_3, rq_stud_3);
}

void test_9_runqueue_destroy(void) {
    utstring_printf(logstr, "Testing emptyqueue. ");
    stud_rq_destroy(rq_stud_empty);
    validate_rq(rq_stud_empty);
    compare_rq(rq_empty, rq_stud_empty);

    clear_log();
    utstring_printf(logstr, "Testing runqueue3. ");
    stud_rq_destroy(rq_stud_3);
    validate_rq(rq_stud_3);
    compare_rq(rq_empty, rq_stud_3);
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
    return UNITY_END();
}