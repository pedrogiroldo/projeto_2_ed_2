#include "quadra.h"

#include "unity.h"

#include <math.h>

static void assert_double_igual(double esperado, double atual) {
  TEST_ASSERT_TRUE(fabs(esperado - atual) < 0.000001);
}

void setUp(void) {}

void tearDown(void) {}

void test_quadra_criar_guarda_propriedades(void) {
  quadra_t *quadra =
      quadra_criar("01001", 10.5, 20.25, 30.0, 40.0, "blue", "black", 2.0);

  TEST_ASSERT_NOT_NULL(quadra);
  TEST_ASSERT_EQUAL_STRING("01001", quadra_obter_cep(quadra));
  assert_double_igual(10.5, quadra_obter_x(quadra));
  assert_double_igual(20.25, quadra_obter_y(quadra));
  assert_double_igual(30.0, quadra_obter_largura(quadra));
  assert_double_igual(40.0, quadra_obter_altura(quadra));
  TEST_ASSERT_EQUAL_STRING("blue", quadra_obter_cor_preenchimento(quadra));
  TEST_ASSERT_EQUAL_STRING("black", quadra_obter_cor_borda(quadra));
  assert_double_igual(2.0, quadra_obter_espessura_borda(quadra));

  quadra_destruir(quadra);
}

void test_quadra_criar_rejeita_argumentos_invalidos(void) {
  TEST_ASSERT_NULL(quadra_criar(NULL, 0.0, 0.0, 10.0, 10.0, "red", "black",
                                1.0));
  TEST_ASSERT_NULL(quadra_criar("", 0.0, 0.0, 10.0, 10.0, "red", "black",
                                1.0));
  TEST_ASSERT_NULL(quadra_criar("01001", 0.0, 0.0, 0.0, 10.0, "red",
                                "black", 1.0));
  TEST_ASSERT_NULL(quadra_criar("01001", 0.0, 0.0, 10.0, -1.0, "red",
                                "black", 1.0));
  TEST_ASSERT_NULL(quadra_criar("01001", 0.0, 0.0, 10.0, 10.0, NULL,
                                "black", 1.0));
  TEST_ASSERT_NULL(quadra_criar("01001", 0.0, 0.0, 10.0, 10.0, "red", "",
                                1.0));
  TEST_ASSERT_NULL(quadra_criar("01001", 0.0, 0.0, 10.0, 10.0, "red",
                                "black", -0.1));
}

void test_quadra_getters_aceitam_null(void) {
  TEST_ASSERT_NULL(quadra_obter_cep(NULL));
  assert_double_igual(0.0, quadra_obter_x(NULL));
  assert_double_igual(0.0, quadra_obter_y(NULL));
  assert_double_igual(0.0, quadra_obter_largura(NULL));
  assert_double_igual(0.0, quadra_obter_altura(NULL));
  TEST_ASSERT_NULL(quadra_obter_cor_preenchimento(NULL));
  TEST_ASSERT_NULL(quadra_obter_cor_borda(NULL));
  assert_double_igual(0.0, quadra_obter_espessura_borda(NULL));
}

void test_quadra_converte_para_registro_e_recria_quadra(void) {
  quadra_t *original =
      quadra_criar("01001", 10.5, 20.25, 30.0, 40.0, "blue", "black", 2.0);
  quadra_t *recriada;
  quadra_registro_t registro;

  TEST_ASSERT_NOT_NULL(original);
  TEST_ASSERT_EQUAL_INT(1, quadra_para_registro(original, &registro));
  TEST_ASSERT_EQUAL_STRING("01001", registro.cep);
  TEST_ASSERT_EQUAL_STRING("blue", registro.cor_preenchimento);
  TEST_ASSERT_EQUAL_STRING("black", registro.cor_borda);
  assert_double_igual(10.5, registro.x);
  assert_double_igual(20.25, registro.y);
  assert_double_igual(30.0, registro.largura);
  assert_double_igual(40.0, registro.altura);
  assert_double_igual(2.0, registro.espessura_borda);

  recriada = quadra_criar_de_registro(&registro);
  TEST_ASSERT_NOT_NULL(recriada);
  TEST_ASSERT_EQUAL_STRING(quadra_obter_cep(original), quadra_obter_cep(recriada));
  assert_double_igual(quadra_obter_x(original), quadra_obter_x(recriada));
  assert_double_igual(quadra_obter_y(original), quadra_obter_y(recriada));
  assert_double_igual(quadra_obter_largura(original),
                      quadra_obter_largura(recriada));
  assert_double_igual(quadra_obter_altura(original),
                      quadra_obter_altura(recriada));
  TEST_ASSERT_EQUAL_STRING(quadra_obter_cor_preenchimento(original),
                           quadra_obter_cor_preenchimento(recriada));
  TEST_ASSERT_EQUAL_STRING(quadra_obter_cor_borda(original),
                           quadra_obter_cor_borda(recriada));
  assert_double_igual(quadra_obter_espessura_borda(original),
                      quadra_obter_espessura_borda(recriada));

  quadra_destruir(recriada);
  quadra_destruir(original);
}

void test_quadra_conversao_rejeita_argumentos_invalidos(void) {
  quadra_registro_t registro;

  TEST_ASSERT_EQUAL_INT(0, quadra_para_registro(NULL, &registro));
  TEST_ASSERT_NULL(quadra_criar_de_registro(NULL));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_quadra_criar_guarda_propriedades);
  RUN_TEST(test_quadra_criar_rejeita_argumentos_invalidos);
  RUN_TEST(test_quadra_getters_aceitam_null);
  RUN_TEST(test_quadra_converte_para_registro_e_recria_quadra);
  RUN_TEST(test_quadra_conversao_rejeita_argumentos_invalidos);
  return UNITY_END();
}
