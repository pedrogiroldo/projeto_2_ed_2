#include "queue.h"

#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_queue_fifo_peek_size_and_clear(void) {
    int a = 1, b = 2;
    Queue q = queue_create();

    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_TRUE(queue_is_empty(q));
    TEST_ASSERT_TRUE(queue_enqueue(q, &a));
    TEST_ASSERT_TRUE(queue_enqueue(q, &b));
    TEST_ASSERT_EQUAL_INT(2, queue_size(q));
    TEST_ASSERT_EQUAL_PTR(&a, queue_peek(q));
    TEST_ASSERT_EQUAL_PTR(&a, queue_dequeue(q));
    TEST_ASSERT_EQUAL_PTR(&b, queue_dequeue(q));
    TEST_ASSERT_NULL(queue_dequeue(q));
    TEST_ASSERT_TRUE(queue_is_empty(q));

    TEST_ASSERT_TRUE(queue_enqueue(q, &a));
    queue_clear(q);
    TEST_ASSERT_EQUAL_INT(0, queue_size(q));
    queue_destroy(q);
}

void test_queue_null_safety(void) {
    int value = 1;

    TEST_ASSERT_FALSE(queue_enqueue(NULL, &value));
    TEST_ASSERT_NULL(queue_dequeue(NULL));
    TEST_ASSERT_NULL(queue_peek(NULL));
    TEST_ASSERT_TRUE(queue_is_empty(NULL));
    TEST_ASSERT_EQUAL_INT(0, queue_size(NULL));
    queue_clear(NULL);
    queue_destroy(NULL);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_queue_fifo_peek_size_and_clear);
    RUN_TEST(test_queue_null_safety);
    return UNITY_END();
}
