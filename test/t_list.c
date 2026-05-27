#include "list.h"

#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_list_insert_get_remove_and_clear(void) {
    int a = 1, b = 2, c = 3;
    List list = list_create();

    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_TRUE(list_is_empty(list));
    TEST_ASSERT_TRUE(list_insert_back(list, &b));
    TEST_ASSERT_TRUE(list_insert_front(list, &a));
    TEST_ASSERT_TRUE(list_insert_back(list, &c));
    TEST_ASSERT_EQUAL_INT(3, list_size(list));
    TEST_ASSERT_EQUAL_PTR(&a, list_get_first(list));
    TEST_ASSERT_EQUAL_PTR(&b, list_get(list, 1));
    TEST_ASSERT_EQUAL_PTR(&c, list_get_last(list));
    TEST_ASSERT_TRUE(list_remove(list, &b));
    TEST_ASSERT_EQUAL_INT(2, list_size(list));
    TEST_ASSERT_NULL(list_get(list, 2));

    list_clear(list);
    TEST_ASSERT_TRUE(list_is_empty(list));
    list_destroy(list);
}

void test_list_null_safety(void) {
    int value = 1;

    TEST_ASSERT_FALSE(list_insert_back(NULL, &value));
    TEST_ASSERT_FALSE(list_insert_front(NULL, &value));
    TEST_ASSERT_FALSE(list_remove(NULL, &value));
    TEST_ASSERT_NULL(list_get(NULL, 0));
    TEST_ASSERT_EQUAL_INT(0, list_size(NULL));
    TEST_ASSERT_TRUE(list_is_empty(NULL));
    list_destroy(NULL);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_list_insert_get_remove_and_clear);
    RUN_TEST(test_list_null_safety);
    return UNITY_END();
}
