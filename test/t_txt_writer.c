#include "txt_writer.h"

#include "unity.h"

#include <stdio.h>
#include <string.h>

static const char *TXT_PATH = "/tmp/txt_writer_test.txt";

static char *read_txt(void) {
    static char buf[1024];
    FILE *f = fopen(TXT_PATH, "r");
    if (f == NULL) {
        buf[0] = '\0';
        return buf;
    }
    size_t n = fread(buf, 1u, sizeof(buf) - 1u, f);
    buf[n] = '\0';
    fclose(f);
    return buf;
}

void setUp(void) { remove(TXT_PATH); }
void tearDown(void) { remove(TXT_PATH); }

void test_txt_writer_escreve_linhas_formatadas(void) {
    txt_writer_t *txt = txt_writer_criar(TXT_PATH);
    char *contents;

    TEST_ASSERT_NOT_NULL(txt);
    txt_writer_linha(txt, "linha %d", 1);
    txt_writer_linha(txt, "%s", "linha 2");
    txt_writer_destruir(txt);

    contents = read_txt();
    TEST_ASSERT_EQUAL_STRING("linha 1\nlinha 2\n", contents);
}

void test_txt_writer_rejeita_path_null_e_ignora_null(void) {
    TEST_ASSERT_NULL(txt_writer_criar(NULL));
    txt_writer_linha(NULL, "ignorado");
    txt_writer_destruir(NULL);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_txt_writer_escreve_linhas_formatadas);
    RUN_TEST(test_txt_writer_rejeita_path_null_e_ignora_null);
    return UNITY_END();
}
