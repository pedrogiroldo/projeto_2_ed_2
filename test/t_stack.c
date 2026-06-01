#include "stack.h"

#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_stack_lifo_peek_at_size_and_clear(void) {
    int a = 1, b = 2, c = 3;
    Stack s = stack_create();

    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_TRUE(stack_is_empty(s));
    TEST_ASSERT_TRUE(stack_push(s, &a));
    TEST_ASSERT_TRUE(stack_push(s, &b));
    TEST_ASSERT_TRUE(stack_push(s, &c));
    TEST_ASSERT_EQUAL_INT(3, stack_size(s));
    TEST_ASSERT_EQUAL_PTR(&c, stack_peek(s));
    TEST_ASSERT_EQUAL_PTR(&b, stack_peek_at(s, 1));
    TEST_ASSERT_EQUAL_PTR(&c, stack_pop(s));
    TEST_ASSERT_EQUAL_PTR(&b, stack_pop(s));
    TEST_ASSERT_EQUAL_PTR(&a, stack_pop(s));
    TEST_ASSERT_NULL(stack_pop(s));

    TEST_ASSERT_TRUE(stack_push(s, &a));
    stack_clear(s);
    TEST_ASSERT_TRUE(stack_is_empty(s));
    stack_destroy(s);
}

void test_stack_null_safety(void) {
    int value = 1;

    TEST_ASSERT_FALSE(stack_push(NULL, &value));
    TEST_ASSERT_NULL(stack_pop(NULL));
    TEST_ASSERT_NULL(stack_peek(NULL));
    TEST_ASSERT_NULL(stack_peek_at(NULL, 0));
    TEST_ASSERT_TRUE(stack_is_empty(NULL));
    TEST_ASSERT_EQUAL_INT(0, stack_size(NULL));
    stack_clear(NULL);
    stack_destroy(NULL);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_stack_lifo_peek_at_size_and_clear);
    RUN_TEST(test_stack_null_safety);
    return UNITY_END();
}
