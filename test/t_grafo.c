#include "grafo.h"

#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_grafo_criar_destruir(void) {
    Grafo g = grafo_criar();
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL_INT(0, grafo_num_vertices(g));
    TEST_ASSERT_EQUAL_INT(0, grafo_num_arestas(g));
    grafo_destruir(g);
}

void test_grafo_inserir_vertices(void) {
    Grafo g = grafo_criar();

    TEST_ASSERT_TRUE(grafo_inserir_vertice(g, "v1", 0.0, 0.0));
    TEST_ASSERT_TRUE(grafo_inserir_vertice(g, "v2", 1.0, 2.0));
    TEST_ASSERT_TRUE(grafo_inserir_vertice(g, "v3", 3.0, 4.0));
    TEST_ASSERT_TRUE(grafo_inserir_vertice(g, "v4", 5.0, 6.0));
    TEST_ASSERT_TRUE(grafo_inserir_vertice(g, "v5", 7.0, 8.0));
    TEST_ASSERT_EQUAL_INT(5, grafo_num_vertices(g));

    /* duplicata deve falhar */
    TEST_ASSERT_FALSE(grafo_inserir_vertice(g, "v1", 9.0, 9.0));
    TEST_ASSERT_EQUAL_INT(5, grafo_num_vertices(g));

    grafo_destruir(g);
}

void test_grafo_inserir_arestas(void) {
    Grafo g = grafo_criar();

    grafo_inserir_vertice(g, "A", 0.0, 0.0);
    grafo_inserir_vertice(g, "B", 1.0, 0.0);
    grafo_inserir_vertice(g, "C", 2.0, 0.0);
    grafo_inserir_vertice(g, "D", 3.0, 0.0);
    grafo_inserir_vertice(g, "E", 4.0, 0.0);

    TEST_ASSERT_TRUE(grafo_inserir_aresta(g, "A", "B", "-", "-", 100.0, 10.0, "rua1"));
    TEST_ASSERT_TRUE(grafo_inserir_aresta(g, "B", "C", "CEP1", "-", 200.0, 20.0, "rua2"));
    TEST_ASSERT_TRUE(grafo_inserir_aresta(g, "C", "D", "-", "CEP2", 300.0, 30.0, "rua3"));
    TEST_ASSERT_TRUE(grafo_inserir_aresta(g, "D", "E", "CEP3", "CEP4", 400.0, 40.0, "rua4"));
    TEST_ASSERT_TRUE(grafo_inserir_aresta(g, "A", "C", "-", "-", 500.0, 50.0, "rua5"));
    TEST_ASSERT_TRUE(grafo_inserir_aresta(g, "E", "A", "-", "-", 600.0, 60.0, "rua6"));
    TEST_ASSERT_EQUAL_INT(6, grafo_num_arestas(g));

    /* vértice inexistente deve falhar */
    TEST_ASSERT_FALSE(grafo_inserir_aresta(g, "A", "Z", "-", "-", 1.0, 1.0, "x"));
    TEST_ASSERT_EQUAL_INT(6, grafo_num_arestas(g));

    grafo_destruir(g);
}

void test_grafo_buscar_vertice(void) {
    Grafo g = grafo_criar();

    grafo_inserir_vertice(g, "P1", 1.5, 2.5);
    grafo_inserir_vertice(g, "P2", 3.5, 4.5);

    void *v = grafo_buscar_vertice(g, "P1");
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL_STRING("P1", grafo_vertice_id(v));
    TEST_ASSERT_EQUAL_DOUBLE(1.5, grafo_vertice_x(v));
    TEST_ASSERT_EQUAL_DOUBLE(2.5, grafo_vertice_y(v));

    TEST_ASSERT_NULL(grafo_buscar_vertice(g, "INEXISTENTE"));

    grafo_destruir(g);
}

void test_grafo_iterar_adjacentes(void) {
    Grafo g = grafo_criar();

    grafo_inserir_vertice(g, "X", 0.0, 0.0);
    grafo_inserir_vertice(g, "Y", 1.0, 0.0);
    grafo_inserir_vertice(g, "Z", 2.0, 0.0);

    grafo_inserir_aresta(g, "X", "Y", "-", "-", 10.0, 5.0, "rua_xy");
    grafo_inserir_aresta(g, "X", "Z", "-", "-", 20.0, 8.0, "rua_xz");

    int count = 0;
    void *cursor = grafo_adjacentes_inicio(g, "X");
    while (cursor != NULL) {
        count++;
        cursor = grafo_adjacentes_fim(g, cursor);
    }
    TEST_ASSERT_EQUAL_INT(2, count);

    /* vértice sem adjacentes */
    TEST_ASSERT_NULL(grafo_adjacentes_inicio(g, "Z"));

    grafo_destruir(g);
}

void test_grafo_getters_aresta(void) {
    Grafo g = grafo_criar();

    grafo_inserir_vertice(g, "M", 0.0, 0.0);
    grafo_inserir_vertice(g, "N", 5.0, 5.0);
    grafo_inserir_aresta(g, "M", "N", "CEP_DIR", "CEP_ESQ", 250.0, 15.0, "av_principal");

    void *cursor = grafo_adjacentes_inicio(g, "M");
    TEST_ASSERT_NOT_NULL(cursor);
    TEST_ASSERT_EQUAL_STRING("N", grafo_aresta_destino(cursor));
    TEST_ASSERT_EQUAL_STRING("av_principal", grafo_aresta_nome(cursor));
    TEST_ASSERT_EQUAL_STRING("CEP_DIR", grafo_aresta_ldir(cursor));
    TEST_ASSERT_EQUAL_STRING("CEP_ESQ", grafo_aresta_lesq(cursor));
    TEST_ASSERT_EQUAL_DOUBLE(250.0, grafo_aresta_cmp(cursor));
    TEST_ASSERT_EQUAL_DOUBLE(15.0, grafo_aresta_vm(cursor));

    grafo_aresta_set_vm(cursor, 30.0);
    TEST_ASSERT_EQUAL_DOUBLE(30.0, grafo_aresta_vm(cursor));

    grafo_destruir(g);
}

void test_grafo_null_safety(void) {
    TEST_ASSERT_EQUAL_INT(0, grafo_num_vertices(NULL));
    TEST_ASSERT_EQUAL_INT(0, grafo_num_arestas(NULL));
    TEST_ASSERT_NULL(grafo_buscar_vertice(NULL, "id"));
    TEST_ASSERT_FALSE(grafo_inserir_vertice(NULL, "id", 0.0, 0.0));
    TEST_ASSERT_FALSE(grafo_inserir_aresta(NULL, "i", "j", "-", "-", 1.0, 1.0, "r"));
    TEST_ASSERT_NULL(grafo_adjacentes_inicio(NULL, "id"));
    grafo_destruir(NULL);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_grafo_criar_destruir);
    RUN_TEST(test_grafo_inserir_vertices);
    RUN_TEST(test_grafo_inserir_arestas);
    RUN_TEST(test_grafo_buscar_vertice);
    RUN_TEST(test_grafo_iterar_adjacentes);
    RUN_TEST(test_grafo_getters_aresta);
    RUN_TEST(test_grafo_null_safety);
    return UNITY_END();
}
