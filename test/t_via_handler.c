#include "via_handler.h"
#include "grafo.h"
#include "unity.h"

#include <stdio.h>
#include <string.h>

static const char *VIA_PATH = "/tmp/via_handler_test.via";

static void write_via(const char *contents) {
    FILE *f = fopen(VIA_PATH, "w");
    if (f != NULL) {
        fputs(contents, f);
        fclose(f);
    }
}

void setUp(void)    { remove(VIA_PATH); }
void tearDown(void) { remove(VIA_PATH); }

void test_via_carregar_insere_vertices_e_arestas(void) {
    write_via("5\n"
              "v v1 0.0 0.0\n"
              "v v2 1.0 0.0\n"
              "v v3 2.0 0.0\n"
              "v v4 3.0 0.0\n"
              "v v5 4.0 0.0\n"
              "e v1 v2 - - 100.0 10.0 rua_a\n"
              "e v2 v3 CEP1 - 200.0 20.0 rua_b\n"
              "e v3 v4 - CEP2 300.0 30.0 rua_c\n"
              "e v4 v5 CEP3 CEP4 400.0 40.0 rua_d\n"
              "e v1 v3 - - 500.0 50.0 rua_e\n"
              "e v5 v1 - - 600.0 60.0 rua_f\n");

    Grafo g = grafo_criar();
    TEST_ASSERT_NOT_NULL(g);

    bool ok = via_carregar(VIA_PATH, g, NULL);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_INT(5, grafo_num_vertices(g));
    TEST_ASSERT_EQUAL_INT(6, grafo_num_arestas(g));

    void *v = grafo_buscar_vertice(g, "v1");
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL_STRING("v1", grafo_vertice_id(v));

    grafo_destruir(g);
}

void test_via_carregar_arquivo_inexistente_retorna_false(void) {
    Grafo g = grafo_criar();
    TEST_ASSERT_FALSE(via_carregar("/tmp/nao_existe.via", g, NULL));
    TEST_ASSERT_EQUAL_INT(0, grafo_num_vertices(g));
    grafo_destruir(g);
}

void test_via_carregar_null_retorna_false(void) {
    Grafo g = grafo_criar();
    TEST_ASSERT_FALSE(via_carregar(NULL, g, NULL));
    TEST_ASSERT_FALSE(via_carregar(VIA_PATH, NULL, NULL));
    grafo_destruir(g);
}

void test_via_carregar_arquivo_vazio_retorna_false(void) {
    write_via("");
    Grafo g = grafo_criar();
    TEST_ASSERT_FALSE(via_carregar(VIA_PATH, g, NULL));
    TEST_ASSERT_EQUAL_INT(0, grafo_num_vertices(g));
    grafo_destruir(g);
}

void test_via_carregar_sem_arestas(void) {
    write_via("3\n"
              "v a 0.0 0.0\n"
              "v b 1.0 1.0\n"
              "v c 2.0 2.0\n");

    Grafo g = grafo_criar();
    TEST_ASSERT_TRUE(via_carregar(VIA_PATH, g, NULL));
    TEST_ASSERT_EQUAL_INT(3, grafo_num_vertices(g));
    TEST_ASSERT_EQUAL_INT(0, grafo_num_arestas(g));
    grafo_destruir(g);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_via_carregar_insere_vertices_e_arestas);
    RUN_TEST(test_via_carregar_arquivo_inexistente_retorna_false);
    RUN_TEST(test_via_carregar_null_retorna_false);
    RUN_TEST(test_via_carregar_arquivo_vazio_retorna_false);
    RUN_TEST(test_via_carregar_sem_arestas);
    return UNITY_END();
}
