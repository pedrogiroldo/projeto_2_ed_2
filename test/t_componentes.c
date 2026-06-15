#include "componentes.h"
#include "grafo.h"
#include "unity.h"

#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

/*
 * Cenário 1 — 3 componentes óbvias (9 vértices, 3 grupos de 3):
 *
 *   Grupo 0: v0 <-> v1 <-> v2   (vm=50)
 *   Grupo 1: v3 <-> v4 <-> v5   (vm=50)
 *   Grupo 2: v6 <-> v7 <-> v8   (vm=50)
 *
 * Sem arestas entre grupos.
 */
static void test_tres_componentes(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "v0", 0, 0);
    grafo_inserir_vertice(g, "v1", 1, 0);
    grafo_inserir_vertice(g, "v2", 2, 0);
    grafo_inserir_vertice(g, "v3", 10, 0);
    grafo_inserir_vertice(g, "v4", 11, 0);
    grafo_inserir_vertice(g, "v5", 12, 0);
    grafo_inserir_vertice(g, "v6", 20, 0);
    grafo_inserir_vertice(g, "v7", 21, 0);
    grafo_inserir_vertice(g, "v8", 22, 0);

    /* Grupo 0 (bidireccional via dígrafo) */
    grafo_inserir_aresta(g, "v0", "v1", "-", "-", 100, 50, "r0");
    grafo_inserir_aresta(g, "v1", "v0", "-", "-", 100, 50, "r0");
    grafo_inserir_aresta(g, "v1", "v2", "-", "-", 100, 50, "r0");
    grafo_inserir_aresta(g, "v2", "v1", "-", "-", 100, 50, "r0");

    /* Grupo 1 */
    grafo_inserir_aresta(g, "v3", "v4", "-", "-", 100, 50, "r1");
    grafo_inserir_aresta(g, "v4", "v3", "-", "-", 100, 50, "r1");
    grafo_inserir_aresta(g, "v4", "v5", "-", "-", 100, 50, "r1");
    grafo_inserir_aresta(g, "v5", "v4", "-", "-", 100, 50, "r1");

    /* Grupo 2 */
    grafo_inserir_aresta(g, "v6", "v7", "-", "-", 100, 50, "r2");
    grafo_inserir_aresta(g, "v7", "v6", "-", "-", 100, 50, "r2");
    grafo_inserir_aresta(g, "v7", "v8", "-", "-", 100, 50, "r2");
    grafo_inserir_aresta(g, "v8", "v7", "-", "-", 100, 50, "r2");

    int num_comp = 0;
    int *comp = componentes_calcular(g, 10.0, &num_comp);

    TEST_ASSERT_NOT_NULL(comp);
    TEST_ASSERT_EQUAL_INT(3, num_comp);

    /* Vértices do mesmo grupo têm mesmo comp_id */
    TEST_ASSERT_EQUAL_INT(comp[0], comp[1]);
    TEST_ASSERT_EQUAL_INT(comp[1], comp[2]);
    TEST_ASSERT_EQUAL_INT(comp[3], comp[4]);
    TEST_ASSERT_EQUAL_INT(comp[4], comp[5]);
    TEST_ASSERT_EQUAL_INT(comp[6], comp[7]);
    TEST_ASSERT_EQUAL_INT(comp[7], comp[8]);

    /* Grupos diferentes têm comp_ids diferentes */
    TEST_ASSERT_NOT_EQUAL(comp[0], comp[3]);
    TEST_ASSERT_NOT_EQUAL(comp[0], comp[6]);
    TEST_ASSERT_NOT_EQUAL(comp[3], comp[6]);

    free(comp);
    grafo_destruir(g);
}

/*
 * Cenário 2 — vm_minima filtra arestas e divide componentes:
 *
 *   A -[vm=100]-> B -[vm=100]-> C -[vm=5]-> D -[vm=100]-> E
 *   (também versão inversa para tratar como não-dirigido)
 *
 *   vm_min=10: C-D cortada → 2 componentes: {A,B,C} e {D,E}
 *   vm_min=1:  todos visíveis → 1 componente
 */
static void test_filtragem_vm(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "A", 0, 0);
    grafo_inserir_vertice(g, "B", 1, 0);
    grafo_inserir_vertice(g, "C", 2, 0);
    grafo_inserir_vertice(g, "D", 3, 0);
    grafo_inserir_vertice(g, "E", 4, 0);

    grafo_inserir_aresta(g, "A", "B", "-", "-", 100, 100, "r");
    grafo_inserir_aresta(g, "B", "A", "-", "-", 100, 100, "r");
    grafo_inserir_aresta(g, "B", "C", "-", "-", 100, 100, "r");
    grafo_inserir_aresta(g, "C", "B", "-", "-", 100, 100, "r");
    grafo_inserir_aresta(g, "C", "D", "-", "-", 100,   5, "r_lenta");
    grafo_inserir_aresta(g, "D", "C", "-", "-", 100,   5, "r_lenta");
    grafo_inserir_aresta(g, "D", "E", "-", "-", 100, 100, "r");
    grafo_inserir_aresta(g, "E", "D", "-", "-", 100, 100, "r");

    int num_comp = 0;
    int *comp;

    /* vm_min=10: C-D cortada → 2 componentes */
    comp = componentes_calcular(g, 10.0, &num_comp);
    TEST_ASSERT_NOT_NULL(comp);
    TEST_ASSERT_EQUAL_INT(2, num_comp);
    TEST_ASSERT_EQUAL_INT(comp[0], comp[1]); /* A == B */
    TEST_ASSERT_EQUAL_INT(comp[1], comp[2]); /* B == C */
    TEST_ASSERT_EQUAL_INT(comp[3], comp[4]); /* D == E */
    TEST_ASSERT_NOT_EQUAL(comp[2], comp[3]); /* C != D */
    free(comp);

    /* vm_min=1: todos conectados → 1 componente */
    comp = componentes_calcular(g, 1.0, &num_comp);
    TEST_ASSERT_NOT_NULL(comp);
    TEST_ASSERT_EQUAL_INT(1, num_comp);
    /* todos devem ter o mesmo comp_id */
    TEST_ASSERT_EQUAL_INT(comp[0], comp[4]);
    free(comp);

    grafo_destruir(g);
}

/* Cenário 3 — NULL safety */
static void test_null_safety(void) {
    int num_comp = -1;
    int *comp = componentes_calcular(NULL, 0.0, &num_comp);
    TEST_ASSERT_NULL(comp);

    /* num_comp_out NULL não deve crashar */
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "x", 0, 0);
    comp = componentes_calcular(g, 0.0, NULL);
    free(comp); /* pode retornar array válido mesmo sem num_comp_out */
    grafo_destruir(g);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_tres_componentes);
    RUN_TEST(test_filtragem_vm);
    RUN_TEST(test_null_safety);
    return UNITY_END();
}
