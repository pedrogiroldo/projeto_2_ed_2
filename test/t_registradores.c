#include "registradores.h"
#include "unity.h"

#include <math.h>

void setUp(void) {}
void tearDown(void) {}

static void assert_double_igual(double esperado, double atual) {
    TEST_ASSERT_TRUE(fabs(esperado - atual) < 1e-9);
}

/*
 * Quadra de referência: canto superior esquerdo (10, 20), largura 100, altura 50.
 * Canto sudeste (ancoragem) = (110, 70).
 */

static void test_coordenada_face_norte(void) {
    double x, y;
    /* N: y = qy (topo); x = (qx+qw) - num */
    TEST_ASSERT_TRUE(reg_coordenada_endereco(10, 20, 100, 50, 'N', 30, &x, &y));
    assert_double_igual(80.0, x);   /* 110 - 30 */
    assert_double_igual(20.0, y);
}

static void test_coordenada_face_sul(void) {
    double x, y;
    /* S: y = qy+qh (base); x = (qx+qw) - num */
    TEST_ASSERT_TRUE(reg_coordenada_endereco(10, 20, 100, 50, 'S', 30, &x, &y));
    assert_double_igual(80.0, x);   /* 110 - 30 */
    assert_double_igual(70.0, y);   /* 20 + 50 */
}

static void test_coordenada_face_leste(void) {
    double x, y;
    /* L: x = qx+qw (direita); y = (qy+qh) - num */
    TEST_ASSERT_TRUE(reg_coordenada_endereco(10, 20, 100, 50, 'L', 20, &x, &y));
    assert_double_igual(110.0, x);  /* 10 + 100 */
    assert_double_igual(50.0, y);   /* 70 - 20 */
}

static void test_coordenada_face_oeste(void) {
    double x, y;
    /* O: x = qx (esquerda); y = (qy+qh) - num */
    TEST_ASSERT_TRUE(reg_coordenada_endereco(10, 20, 100, 50, 'O', 20, &x, &y));
    assert_double_igual(10.0, x);
    assert_double_igual(50.0, y);   /* 70 - 20 */
}

static void test_coordenada_minuscula_e_invalida(void) {
    double x, y;
    /* face minúscula deve funcionar */
    TEST_ASSERT_TRUE(reg_coordenada_endereco(10, 20, 100, 50, 's', 0, &x, &y));
    assert_double_igual(110.0, x);  /* num=0 → canto sudeste projetado */
    assert_double_igual(70.0, y);

    /* face inválida → false */
    TEST_ASSERT_FALSE(reg_coordenada_endereco(10, 20, 100, 50, 'X', 5, &x, &y));
    /* ponteiros NULL → false */
    TEST_ASSERT_FALSE(reg_coordenada_endereco(10, 20, 100, 50, 'N', 5, NULL, &y));
}

static void test_indice_registrador(void) {
    TEST_ASSERT_EQUAL_INT(0, reg_indice("R0"));
    TEST_ASSERT_EQUAL_INT(10, reg_indice("R10"));
    TEST_ASSERT_EQUAL_INT(3, reg_indice("R3"));
    TEST_ASSERT_EQUAL_INT(-1, reg_indice("R11"));
    TEST_ASSERT_EQUAL_INT(-1, reg_indice("X2"));
    TEST_ASSERT_EQUAL_INT(-1, reg_indice(NULL));
}

static void test_armazenar_e_obter(void) {
    Registradores regs = registradores_criar();
    TEST_ASSERT_NOT_NULL(regs);

    /* recém-criado: nada ocupado */
    TEST_ASSERT_FALSE(reg_ocupado(regs, 5));

    TEST_ASSERT_TRUE(reg_armazenar(regs, 5, 12.5, -3.0));
    TEST_ASSERT_TRUE(reg_ocupado(regs, 5));

    double x = 0, y = 0;
    TEST_ASSERT_TRUE(reg_obter(regs, 5, &x, &y));
    assert_double_igual(12.5, x);
    assert_double_igual(-3.0, y);

    /* registrador vazio → obter retorna false */
    TEST_ASSERT_FALSE(reg_obter(regs, 2, &x, &y));

    /* índices fora do intervalo */
    TEST_ASSERT_FALSE(reg_armazenar(regs, 11, 0, 0));
    TEST_ASSERT_FALSE(reg_armazenar(regs, -1, 0, 0));
    TEST_ASSERT_FALSE(reg_obter(regs, 11, &x, &y));

    /* NULL safety */
    TEST_ASSERT_FALSE(reg_armazenar(NULL, 0, 0, 0));
    TEST_ASSERT_FALSE(reg_obter(NULL, 0, &x, &y));
    TEST_ASSERT_FALSE(reg_ocupado(NULL, 0));
    registradores_destruir(NULL);

    registradores_destruir(regs);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_coordenada_face_norte);
    RUN_TEST(test_coordenada_face_sul);
    RUN_TEST(test_coordenada_face_leste);
    RUN_TEST(test_coordenada_face_oeste);
    RUN_TEST(test_coordenada_minuscula_e_invalida);
    RUN_TEST(test_indice_registrador);
    RUN_TEST(test_armazenar_e_obter);
    return UNITY_END();
}
