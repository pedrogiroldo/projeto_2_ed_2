#include "utils.h"

#include "unity.h"

#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_duplicate_string_copia_conteudo(void) {
    char *copy = duplicate_string("abc");

    TEST_ASSERT_NOT_NULL(copy);
    TEST_ASSERT_EQUAL_STRING("abc", copy);
    TEST_ASSERT_TRUE(copy != (char *)"abc");
    free(copy);
}

void test_duplicate_string_rejeita_null(void) {
    TEST_ASSERT_NULL(duplicate_string(NULL));
}

void test_invert_color_hex_nome_e_desconhecida(void) {
    char *hex = invert_color("#0000FF");
    char *named = invert_color("black");
    char *unknown = invert_color("moccasin");

    TEST_ASSERT_EQUAL_STRING("#FFFF00", hex);
    TEST_ASSERT_EQUAL_STRING("#FFFFFF", named);
    TEST_ASSERT_EQUAL_STRING("moccasin", unknown);

    free(hex);
    free(named);
    free(unknown);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_duplicate_string_copia_conteudo);
    RUN_TEST(test_duplicate_string_rejeita_null);
    RUN_TEST(test_invert_color_hex_nome_e_desconhecida);
    return UNITY_END();
}
