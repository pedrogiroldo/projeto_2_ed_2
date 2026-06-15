#include "dijkstra.h"
#include "grafo.h"
#include "unity.h"

#include <math.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/*
 * Grafo usado em todos os testes:
 *
 *  A --[cmp=100,vm=10]--> B --[cmp=100,vm=10]--> D --[cmp=100,vm=10]--> F
 *  |                                                                      ^
 *  +--[cmp=200,vm=50]--> C --[cmp=300,vm=100]--> E --[cmp=50,vm=10]----+
 *
 *  G: vértice isolado (sem arestas)
 *
 *  A→F por distância (cmp):  A→B→D→F = 300
 *  A→F por tempo (cmp/vm):   A→C→E→F = 4+3+5 = 12
 */
static Grafo criar_grafo_teste(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "A", 0.0, 0.0);
    grafo_inserir_vertice(g, "B", 3.0, 0.0);
    grafo_inserir_vertice(g, "C", 0.0, 4.0);
    grafo_inserir_vertice(g, "D", 6.0, 0.0);
    grafo_inserir_vertice(g, "E", 6.0, 4.0);
    grafo_inserir_vertice(g, "F", 9.0, 0.0);
    grafo_inserir_vertice(g, "G", 99.0, 99.0);

    grafo_inserir_aresta(g, "A", "B", "-", "-", 100.0, 10.0, "rua_ab");
    grafo_inserir_aresta(g, "A", "C", "-", "-", 200.0, 50.0, "rua_ac");
    grafo_inserir_aresta(g, "B", "D", "-", "-", 100.0, 10.0, "rua_bd");
    grafo_inserir_aresta(g, "C", "E", "-", "-", 300.0, 100.0, "rua_ce");
    grafo_inserir_aresta(g, "D", "F", "-", "-", 100.0, 10.0, "rua_df");
    grafo_inserir_aresta(g, "E", "F", "-", "-",  50.0, 10.0, "rua_ef");
    return g;
}

static double peso_distancia(void *cursor) {
    return grafo_aresta_cmp(cursor);
}

static double peso_tempo(void *cursor) {
    return grafo_aresta_cmp(cursor) / grafo_aresta_vm(cursor);
}

static void assert_double_igual(double esperado, double atual) {
    TEST_ASSERT_TRUE(fabs(esperado - atual) < 1e-9);
}

/* ---- testes ---- */

void test_dijkstra_distancia_por_distancia(void) {
    Grafo g = criar_grafo_teste();
    DijkstraResultado res = dijkstra_executar(g, "A", peso_distancia);
    TEST_ASSERT_NOT_NULL(res);

    assert_double_igual(0.0,   dijkstra_distancia(res, "A"));
    assert_double_igual(100.0, dijkstra_distancia(res, "B"));
    assert_double_igual(200.0, dijkstra_distancia(res, "C"));
    assert_double_igual(200.0, dijkstra_distancia(res, "D"));
    assert_double_igual(300.0, dijkstra_distancia(res, "F"));

    dijkstra_resultado_destruir(res);
    grafo_destruir(g);
}

void test_dijkstra_distancia_por_tempo(void) {
    Grafo g = criar_grafo_teste();
    DijkstraResultado res = dijkstra_executar(g, "A", peso_tempo);
    TEST_ASSERT_NOT_NULL(res);

    assert_double_igual(0.0,  dijkstra_distancia(res, "A"));
    assert_double_igual(10.0, dijkstra_distancia(res, "B"));
    assert_double_igual(4.0,  dijkstra_distancia(res, "C"));
    /* A→C→E: 4+3=7; A→B→D: 10+10=20 → por tempo D é via C→E→... sem aresta direta */
    assert_double_igual(7.0,  dijkstra_distancia(res, "E"));
    /* A→C→E→F: 4+3+5=12; A→B→D→F: 10+10+10=30 */
    assert_double_igual(12.0, dijkstra_distancia(res, "F"));

    dijkstra_resultado_destruir(res);
    grafo_destruir(g);
}

void test_dijkstra_alcancavel(void) {
    Grafo g = criar_grafo_teste();
    DijkstraResultado res = dijkstra_executar(g, "A", peso_distancia);
    TEST_ASSERT_NOT_NULL(res);

    TEST_ASSERT_TRUE(dijkstra_alcancavel(res, "F"));
    TEST_ASSERT_TRUE(dijkstra_alcancavel(res, "A"));
    TEST_ASSERT_FALSE(dijkstra_alcancavel(res, "G")); /* isolado */
    TEST_ASSERT_FALSE(dijkstra_alcancavel(res, "INEXISTENTE"));

    dijkstra_resultado_destruir(res);
    grafo_destruir(g);
}

typedef struct {
    int count;
    char nomes[8][64];
} CaminhoCtx;

static void cb_aresta(void *cursor, void *ctx) {
    CaminhoCtx *c = (CaminhoCtx *)ctx;
    strncpy(c->nomes[c->count], grafo_aresta_nome(cursor), 63);
    c->nomes[c->count][63] = '\0';
    c->count++;
}

void test_dijkstra_caminho_por_arestas_distancia(void) {
    Grafo g = criar_grafo_teste();
    DijkstraResultado res = dijkstra_executar(g, "A", peso_distancia);
    TEST_ASSERT_NOT_NULL(res);

    CaminhoCtx ctx = {0};
    dijkstra_caminho_por_arestas(res, g, "F", cb_aresta, &ctx);

    /* caminho mínimo por dist: A→B→D→F (3 arestas) */
    TEST_ASSERT_EQUAL_INT(3, ctx.count);
    TEST_ASSERT_EQUAL_STRING("rua_ab", ctx.nomes[0]);
    TEST_ASSERT_EQUAL_STRING("rua_bd", ctx.nomes[1]);
    TEST_ASSERT_EQUAL_STRING("rua_df", ctx.nomes[2]);

    /* inacessível: callback não é chamada */
    CaminhoCtx ctx2 = {0};
    dijkstra_caminho_por_arestas(res, g, "G", cb_aresta, &ctx2);
    TEST_ASSERT_EQUAL_INT(0, ctx2.count);

    dijkstra_resultado_destruir(res);
    grafo_destruir(g);
}

void test_dijkstra_null_safety(void) {
    TEST_ASSERT_NULL(dijkstra_executar(NULL, "A", peso_distancia));
    TEST_ASSERT_NULL(dijkstra_executar((Grafo)1, NULL, peso_distancia));
    TEST_ASSERT_NULL(dijkstra_executar((Grafo)1, "A", NULL));

    dijkstra_resultado_destruir(NULL); /* não deve crashar */

    TEST_ASSERT_TRUE(isinf(dijkstra_distancia(NULL, "A")));
    TEST_ASSERT_FALSE(dijkstra_alcancavel(NULL, "A"));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_dijkstra_distancia_por_distancia);
    RUN_TEST(test_dijkstra_distancia_por_tempo);
    RUN_TEST(test_dijkstra_alcancavel);
    RUN_TEST(test_dijkstra_caminho_por_arestas_distancia);
    RUN_TEST(test_dijkstra_null_safety);
    return UNITY_END();
}
