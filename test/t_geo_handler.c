#include "geo_handler.h"

#include "extensible_hash_file.h"
#include "quadra.h"
#include "unity.h"

#include <stdlib.h>

#include <stdio.h>
#include <string.h>

static const char *HF_PATH  = "/tmp/geo_handler_test.hf";
static const char *GEO_PATH = "/tmp/geo_handler_test.geo";

static void write_geo(const char *contents) {
    FILE *f = fopen(GEO_PATH, "w");
    if (f != NULL) {
        fputs(contents, f);
        fclose(f);
    }
}

void setUp(void) {
    remove(HF_PATH);
    remove(GEO_PATH);
}

void tearDown(void) {
    remove(HF_PATH);
    remove(GEO_PATH);
}

void test_geo_handler_insere_quadras_no_hashfile(void) {
    extensible_hash_file_t hf =
        ehf_create(HF_PATH, 4u, quadra_registro_size());
    geo_handler_resultado_t res;
    quadra_registro_t *reg = malloc(quadra_registro_size());

    TEST_ASSERT_NOT_NULL(hf);
    write_geo("cq 2 #FF9966 #8B4513\n"
              "q cep1 0 0 100 100\n"
              "q cep2 100 0 100 100\n");

    res = geo_handler_processar(GEO_PATH, hf, NULL);

    TEST_ASSERT_EQUAL_INT(2, geo_handler_resultado_inseridas(res));
    TEST_ASSERT_EQUAL_INT(0, geo_handler_resultado_erros(res));
    TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hf, "cep1", reg, quadra_registro_size()));
    TEST_ASSERT_EQUAL_INT(0, (int)quadra_registro_x(reg));
    TEST_ASSERT_EQUAL_INT(100, (int)quadra_registro_largura(reg));
    TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hf, "cep2", reg, quadra_registro_size()));
    TEST_ASSERT_EQUAL_INT(100, (int)quadra_registro_x(reg));

    geo_handler_resultado_destruir(res);
    free(reg);
    ehf_close(hf);
}

void test_geo_handler_arquivo_vazio_zero_quadras(void) {
    extensible_hash_file_t hf =
        ehf_create(HF_PATH, 4u, quadra_registro_size());
    geo_handler_resultado_t res;

    TEST_ASSERT_NOT_NULL(hf);
    write_geo("");

    res = geo_handler_processar(GEO_PATH, hf, NULL);
    TEST_ASSERT_EQUAL_INT(0, geo_handler_resultado_inseridas(res));
    TEST_ASSERT_EQUAL_INT(0, geo_handler_resultado_erros(res));

    geo_handler_resultado_destruir(res);
    ehf_close(hf);
}

void test_geo_handler_linha_malformada_conta_erro(void) {
    extensible_hash_file_t hf =
        ehf_create(HF_PATH, 4u, quadra_registro_size());
    geo_handler_resultado_t res;

    TEST_ASSERT_NOT_NULL(hf);
    write_geo("cq 2 #FF9966 #8B4513\n"
              "q cep1 0 0 100\n");  /* falta altura */

    res = geo_handler_processar(GEO_PATH, hf, NULL);
    TEST_ASSERT_EQUAL_INT(0, geo_handler_resultado_inseridas(res));
    TEST_ASSERT_EQUAL_INT(1, geo_handler_resultado_erros(res));

    geo_handler_resultado_destruir(res);
    ehf_close(hf);
}

void test_geo_handler_sem_cq_conta_erro(void) {
    extensible_hash_file_t hf =
        ehf_create(HF_PATH, 4u, quadra_registro_size());
    geo_handler_resultado_t res;

    TEST_ASSERT_NOT_NULL(hf);
    /* 'q' sem 'cq' prévio deve falhar */
    write_geo("q cep1 0 0 100 100\n");

    res = geo_handler_processar(GEO_PATH, hf, NULL);
    TEST_ASSERT_EQUAL_INT(0, geo_handler_resultado_inseridas(res));
    TEST_ASSERT_EQUAL_INT(1, geo_handler_resultado_erros(res));

    geo_handler_resultado_destruir(res);
    ehf_close(hf);
}

void test_geo_handler_arquivo_nao_existe_conta_erro(void) {
    extensible_hash_file_t hf =
        ehf_create(HF_PATH, 4u, quadra_registro_size());
    geo_handler_resultado_t res;

    TEST_ASSERT_NOT_NULL(hf);
    res = geo_handler_processar("/tmp/nao_existe_xyz.geo", hf, NULL);
    TEST_ASSERT_EQUAL_INT(0, geo_handler_resultado_inseridas(res));
    TEST_ASSERT_EQUAL_INT(1, geo_handler_resultado_erros(res));

    geo_handler_resultado_destruir(res);
    ehf_close(hf);
}

void test_geo_handler_null_path_conta_erro(void) {
    extensible_hash_file_t hf =
        ehf_create(HF_PATH, 4u, quadra_registro_size());
    geo_handler_resultado_t res;

    TEST_ASSERT_NOT_NULL(hf);
    res = geo_handler_processar(NULL, hf, NULL);
    TEST_ASSERT_TRUE(geo_handler_resultado_erros(res) >= 1);

    geo_handler_resultado_destruir(res);
    ehf_close(hf);
}

void test_geo_handler_quadra_gera_svg(void) {
    static const char *SVG_PATH = "/tmp/geo_handler_test.svg";
    extensible_hash_file_t hf =
        ehf_create(HF_PATH, 4u, quadra_registro_size());
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 400.0, 400.0);
    geo_handler_resultado_t res;
    FILE *f;
    char buf[4096];
    size_t n;

    TEST_ASSERT_NOT_NULL(hf);
    TEST_ASSERT_NOT_NULL(svg);
    write_geo("cq 2 #FF9966 #8B4513\nq cep1 0 0 100 100\n");

    res = geo_handler_processar(GEO_PATH, hf, svg);
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    TEST_ASSERT_EQUAL_INT(1, geo_handler_resultado_inseridas(res));

    f = fopen(SVG_PATH, "r");
    TEST_ASSERT_NOT_NULL(f);
    n = fread(buf, 1, sizeof(buf) - 1u, f);
    buf[n] = '\0';
    fclose(f);
    remove(SVG_PATH);

    TEST_ASSERT_NOT_NULL(strstr(buf, "<rect"));

    geo_handler_resultado_destruir(res);
    ehf_close(hf);
}

void test_geo_handler_aceita_cq_com_px(void) {
    extensible_hash_file_t hf =
        ehf_create(HF_PATH, 4u, quadra_registro_size());
    geo_handler_resultado_t res;
    quadra_registro_t *reg = malloc(quadra_registro_size());

    TEST_ASSERT_NOT_NULL(hf);
    write_geo("cq 1.0px Olive Moccasin\n"
              "q b01.1 0 0 100 100\n");

    res = geo_handler_processar(GEO_PATH, hf, NULL);

    TEST_ASSERT_EQUAL_INT(1, geo_handler_resultado_inseridas(res));
    TEST_ASSERT_EQUAL_INT(0, geo_handler_resultado_erros(res));
    TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hf, "b01.1", reg, quadra_registro_size()));

    geo_handler_resultado_destruir(res);
    free(reg);
    ehf_close(hf);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_geo_handler_insere_quadras_no_hashfile);
    RUN_TEST(test_geo_handler_arquivo_vazio_zero_quadras);
    RUN_TEST(test_geo_handler_linha_malformada_conta_erro);
    RUN_TEST(test_geo_handler_sem_cq_conta_erro);
    RUN_TEST(test_geo_handler_arquivo_nao_existe_conta_erro);
    RUN_TEST(test_geo_handler_null_path_conta_erro);
    RUN_TEST(test_geo_handler_quadra_gera_svg);
    RUN_TEST(test_geo_handler_aceita_cq_com_px);
    return UNITY_END();
}
