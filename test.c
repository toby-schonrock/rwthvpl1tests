#include "Unity/src/unity.h"


void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_test1(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(4,4, "4=4 :)");
}

void test_test2(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(4,5, "4=5 !?");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_test1);
    RUN_TEST(test_test2);
    return UNITY_END();
}