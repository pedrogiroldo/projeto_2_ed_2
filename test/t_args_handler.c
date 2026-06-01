#include "args_handler.h"

#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_get_option_value_retorna_valor(void) {
    char *argv[] = {"ted", "-e", "entrada", "-f", "cidade.geo", "-o", "saida"};

    TEST_ASSERT_EQUAL_STRING("entrada", get_option_value(7, argv, "e"));
    TEST_ASSERT_EQUAL_STRING("cidade.geo", get_option_value(7, argv, "f"));
    TEST_ASSERT_EQUAL_STRING("saida", get_option_value(7, argv, "o"));
}

void test_get_option_value_retorna_null_para_ausente_ou_sem_valor(void) {
    char *argv[] = {"ted", "-e", "entrada", "-q"};

    TEST_ASSERT_NULL(get_option_value(4, argv, "pm"));
    TEST_ASSERT_NULL(get_option_value(4, argv, "q"));
}

void test_get_command_suffix_retorna_sufixo_unico(void) {
    char *argv[] = {"cmd", "-a", "1", "sufixo"};

    TEST_ASSERT_EQUAL_STRING("sufixo", get_command_suffix(4, argv));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_get_option_value_retorna_valor);
    RUN_TEST(test_get_option_value_retorna_null_para_ausente_ou_sem_valor);
    RUN_TEST(test_get_command_suffix_retorna_sufixo_unico);
    return UNITY_END();
}
