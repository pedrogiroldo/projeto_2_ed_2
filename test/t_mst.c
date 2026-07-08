#include "mst.h"
#include "grafo.h"
#include "unity.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

static void assert_double_igual(double esperado, double atual) {
    TEST_ASSERT_TRUE(fabs(esperado - atual) < 1e-9);
}

/* Insere aresta nas duas direções (dígrafo tratado como não-dirigido) */
static void aresta_bidir(Grafo g, const char *a, const char *b,
                         double cmp, double vm) {
    grafo_inserir_aresta(g, a, b, "-", "-", cmp, vm, "r");
    grafo_inserir_aresta(g, b, a, "-", "-", cmp, vm, "r");
}

/* Verifica se o par (origem,destino) da i-ésima aresta bate com {a,b} (não-dirigido) */
static int par_igual(MstResultado res, int i, const char *a, const char *b) {
    const char *o = mst_aresta_origem(res, i);
    const char *d = mst_aresta_destino(res, i);
    if (!o || !d) return 0;
    return (strcmp(o, a) == 0 && strcmp(d, b) == 0) ||
           (strcmp(o, b) == 0 && strcmp(d, a) == 0);
}

/* Conta quantas arestas da AGM formam o par não-dirigido {a,b} */
static int conta_par(MstResultado res, const char *a, const char *b) {
    int total = 0;
    for (int i = 0; i < mst_num_arestas(res); i++)
        if (par_igual(res, i, a, b)) total++;
    return total;
}

/*
 * Cenário 1 — grafo conexo de 4 vértices, pesos cmp conhecidos:
 *
 *   A-B: 1   A-C: 4
 *   B-C: 2   B-D: 6
 *   C-D: 3
 *
 * Kruskal: A-B(1), B-C(2), C-D(3) → AGM = {A-B, B-C, C-D}, total = 6
 */
static void test_agm_grafo_conexo(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "A", 0, 0);
    grafo_inserir_vertice(g, "B", 1, 0);
    grafo_inserir_vertice(g, "C", 2, 0);
    grafo_inserir_vertice(g, "D", 3, 0);

    aresta_bidir(g, "A", "B", 1, 50);
    aresta_bidir(g, "A", "C", 4, 50);
    aresta_bidir(g, "B", "C", 2, 50);
    aresta_bidir(g, "B", "D", 6, 50);
    aresta_bidir(g, "C", "D", 3, 50);

    MstResultado res = mst_calcular(g);
    TEST_ASSERT_NOT_NULL(res);

    /* n_vertices - n_componentes = 4 - 1 = 3 arestas */
    TEST_ASSERT_EQUAL_INT(3, mst_num_arestas(res));
    assert_double_igual(6.0, mst_peso_total(res));

    /* AGM exatamente {A-B, B-C, C-D} */
    TEST_ASSERT_EQUAL_INT(1, conta_par(res, "A", "B"));
    TEST_ASSERT_EQUAL_INT(1, conta_par(res, "B", "C"));
    TEST_ASSERT_EQUAL_INT(1, conta_par(res, "C", "D"));
    TEST_ASSERT_EQUAL_INT(0, conta_par(res, "A", "C"));
    TEST_ASSERT_EQUAL_INT(0, conta_par(res, "B", "D"));

    mst_resultado_destruir(res);
    grafo_destruir(g);
}

/*
 * Cenário 2 — floresta com 2 componentes:
 *   Componente 1: A-B(1), B-C(5), A-C(2) → AGM local {A-B, A-C} total 3
 *   Componente 2: D-E(4)
 * Total de arestas da floresta = 5 vértices - 2 componentes = 3
 * Peso total = 1 + 2 + 4 = 7
 */
static void test_agm_floresta(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "A", 0, 0);
    grafo_inserir_vertice(g, "B", 1, 0);
    grafo_inserir_vertice(g, "C", 2, 0);
    grafo_inserir_vertice(g, "D", 10, 0);
    grafo_inserir_vertice(g, "E", 11, 0);

    aresta_bidir(g, "A", "B", 1, 50);
    aresta_bidir(g, "B", "C", 5, 50);
    aresta_bidir(g, "A", "C", 2, 50);
    aresta_bidir(g, "D", "E", 4, 50);

    MstResultado res = mst_calcular(g);
    TEST_ASSERT_NOT_NULL(res);

    TEST_ASSERT_EQUAL_INT(3, mst_num_arestas(res));
    assert_double_igual(7.0, mst_peso_total(res));

    TEST_ASSERT_EQUAL_INT(1, conta_par(res, "A", "B"));
    TEST_ASSERT_EQUAL_INT(1, conta_par(res, "A", "C"));
    TEST_ASSERT_EQUAL_INT(0, conta_par(res, "B", "C")); /* aresta pesada descartada */
    TEST_ASSERT_EQUAL_INT(1, conta_par(res, "D", "E"));

    mst_resultado_destruir(res);
    grafo_destruir(g);
}

/* Cenário 3 — grafo com 1 vértice: AGM vazia */
static void test_agm_um_vertice(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "solo", 0, 0);

    MstResultado res = mst_calcular(g);
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_INT(0, mst_num_arestas(res));
    assert_double_igual(0.0, mst_peso_total(res));

    mst_resultado_destruir(res);
    grafo_destruir(g);
}

/* Cenário 4 — NULL safety */
static void test_null_safety(void) {
    TEST_ASSERT_NULL(mst_calcular(NULL));

    /* getters e destruir com NULL não devem crashar */
    mst_resultado_destruir(NULL);
    TEST_ASSERT_EQUAL_INT(0, mst_num_arestas(NULL));
    assert_double_igual(0.0, mst_peso_total(NULL));
    TEST_ASSERT_NULL(mst_aresta_origem(NULL, 0));
    TEST_ASSERT_NULL(mst_aresta_destino(NULL, 0));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_agm_grafo_conexo);
    RUN_TEST(test_agm_floresta);
    RUN_TEST(test_agm_um_vertice);
    RUN_TEST(test_null_safety);
    return UNITY_END();
}
