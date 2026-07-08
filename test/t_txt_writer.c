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

/* ---- Extensões do Projeto 2 ---- */

void test_txt_writer_endereco(void) {
    txt_writer_t *txt = txt_writer_criar(TXT_PATH);
    txt_writer_endereco(txt, "R3", 12.5, 30.0);
    txt_writer_destruir(txt);
    TEST_ASSERT_EQUAL_STRING("R3: (12.50, 30.00)\n", read_txt());
}

void test_txt_writer_num_componentes(void) {
    txt_writer_t *txt = txt_writer_criar(TXT_PATH);
    txt_writer_num_componentes(txt, 4);
    txt_writer_destruir(txt);
    TEST_ASSERT_EQUAL_STRING("Numero de componentes: 4\n", read_txt());
}

void test_txt_writer_passo(void) {
    txt_writer_t *txt = txt_writer_criar(TXT_PATH);
    txt_writer_passo(txt, "Norte", "Rua das Flores");
    txt_writer_destruir(txt);
    TEST_ASSERT_EQUAL_STRING("Siga na direcao Norte pela Rua das Flores\n", read_txt());
}

void test_txt_writer_destino_inacessivel(void) {
    txt_writer_t *txt = txt_writer_criar(TXT_PATH);
    txt_writer_destino_inacessivel(txt);
    txt_writer_destruir(txt);
    TEST_ASSERT_EQUAL_STRING("Destino inacessivel\n", read_txt());
}

void test_txt_writer_direcao_cardeal(void) {
    /* Y cresce para baixo */
    TEST_ASSERT_EQUAL_STRING("Leste", txt_writer_direcao_cardeal(10, 0));
    TEST_ASSERT_EQUAL_STRING("Oeste", txt_writer_direcao_cardeal(-10, 0));
    TEST_ASSERT_EQUAL_STRING("Norte", txt_writer_direcao_cardeal(0, -10));
    TEST_ASSERT_EQUAL_STRING("Sul", txt_writer_direcao_cardeal(0, 10));
    /* componente dominante */
    TEST_ASSERT_EQUAL_STRING("Sul", txt_writer_direcao_cardeal(3, 10));
    TEST_ASSERT_EQUAL_STRING("Leste", txt_writer_direcao_cardeal(10, 3));
    /* nulo */
    TEST_ASSERT_EQUAL_STRING("", txt_writer_direcao_cardeal(0, 0));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_txt_writer_escreve_linhas_formatadas);
    RUN_TEST(test_txt_writer_rejeita_path_null_e_ignora_null);
    RUN_TEST(test_txt_writer_endereco);
    RUN_TEST(test_txt_writer_num_componentes);
    RUN_TEST(test_txt_writer_passo);
    RUN_TEST(test_txt_writer_destino_inacessivel);
    RUN_TEST(test_txt_writer_direcao_cardeal);
    return UNITY_END();
}
