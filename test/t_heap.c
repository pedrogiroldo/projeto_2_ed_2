#include "heap.h"
#include "unity.h"

#include <math.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static void assert_double_igual(double esperado, double atual) {
    TEST_ASSERT_TRUE(fabs(esperado - atual) < 1e-9);
}

void test_heap_criar_destruir(void) {
    Heap h = heap_criar(8);
    TEST_ASSERT_NOT_NULL(h);
    TEST_ASSERT_TRUE(heap_vazio(h));
    TEST_ASSERT_EQUAL_INT(0, heap_tamanho(h));
    heap_destruir(h);
}

void test_heap_extrair_ordem_crescente(void) {
    Heap h = heap_criar(16);

    /* inserir 10 elementos com prioridades fora de ordem */
    TEST_ASSERT_TRUE(heap_inserir(h, "e", 5.0));
    TEST_ASSERT_TRUE(heap_inserir(h, "c", 3.0));
    TEST_ASSERT_TRUE(heap_inserir(h, "j", 10.0));
    TEST_ASSERT_TRUE(heap_inserir(h, "a", 1.0));
    TEST_ASSERT_TRUE(heap_inserir(h, "h", 8.0));
    TEST_ASSERT_TRUE(heap_inserir(h, "b", 2.0));
    TEST_ASSERT_TRUE(heap_inserir(h, "g", 7.0));
    TEST_ASSERT_TRUE(heap_inserir(h, "d", 4.0));
    TEST_ASSERT_TRUE(heap_inserir(h, "f", 6.0));
    TEST_ASSERT_TRUE(heap_inserir(h, "i", 9.0));

    TEST_ASSERT_EQUAL_INT(10, heap_tamanho(h));

    double chave_anterior = -1.0;
    char id_out[64];
    double chave_out;
    int count = 0;

    while (!heap_vazio(h)) {
        TEST_ASSERT_TRUE(heap_extrair_min(h, id_out, &chave_out));
        TEST_ASSERT_TRUE(chave_out >= chave_anterior);
        chave_anterior = chave_out;
        count++;
    }
    TEST_ASSERT_EQUAL_INT(10, count);
    TEST_ASSERT_EQUAL_INT(0, heap_tamanho(h));

    heap_destruir(h);
}

void test_heap_diminuir_chave(void) {
    Heap h = heap_criar(8);

    heap_inserir(h, "x", 10.0);
    heap_inserir(h, "y", 20.0);
    heap_inserir(h, "z", 30.0);

    /* y era 20, diminuir para 1 — deve virar o mínimo */
    TEST_ASSERT_TRUE(heap_diminuir_chave(h, "y", 1.0));

    char id_out[64];
    double chave_out;
    TEST_ASSERT_TRUE(heap_extrair_min(h, id_out, &chave_out));
    TEST_ASSERT_EQUAL_STRING("y", id_out);
    assert_double_igual(1.0, chave_out);

    /* não pode aumentar a chave */
    TEST_ASSERT_FALSE(heap_diminuir_chave(h, "x", 999.0));

    heap_destruir(h);
}

void test_heap_inserir_duplicado_retorna_false(void) {
    Heap h = heap_criar(8);
    TEST_ASSERT_TRUE(heap_inserir(h, "dup", 5.0));
    TEST_ASSERT_FALSE(heap_inserir(h, "dup", 3.0));
    TEST_ASSERT_EQUAL_INT(1, heap_tamanho(h));
    heap_destruir(h);
}

void test_heap_extrair_vazio_retorna_false(void) {
    Heap h = heap_criar(4);
    char id_out[64];
    double chave_out;
    TEST_ASSERT_FALSE(heap_extrair_min(h, id_out, &chave_out));
    heap_destruir(h);
}

void test_heap_null_safety(void) {
    TEST_ASSERT_TRUE(heap_vazio(NULL));
    TEST_ASSERT_EQUAL_INT(0, heap_tamanho(NULL));
    TEST_ASSERT_FALSE(heap_inserir(NULL, "x", 1.0));
    TEST_ASSERT_FALSE(heap_diminuir_chave(NULL, "x", 1.0));
    heap_destruir(NULL);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_heap_criar_destruir);
    RUN_TEST(test_heap_extrair_ordem_crescente);
    RUN_TEST(test_heap_diminuir_chave);
    RUN_TEST(test_heap_inserir_duplicado_retorna_false);
    RUN_TEST(test_heap_extrair_vazio_retorna_false);
    RUN_TEST(test_heap_null_safety);
    return UNITY_END();
}
