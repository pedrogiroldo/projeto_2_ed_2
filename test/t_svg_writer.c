#include "svg_writer.h"

#include "unity.h"

#include <stdio.h>
#include <string.h>

static const char *SVG_PATH = "/tmp/svg_writer_test.svg";

static char *read_svg(void) {
    static char buf[4096];
    FILE *f = fopen(SVG_PATH, "r");
    if (f == NULL) {
        buf[0] = '\0';
        return buf;
    }
    size_t n = fread(buf, 1u, sizeof(buf) - 1u, f);
    buf[n] = '\0';
    fclose(f);
    return buf;
}

void setUp(void) { remove(SVG_PATH); }
void tearDown(void) { remove(SVG_PATH); }

void test_svg_writer_escreve_elementos_basicos(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_retangulo(svg, 1.0, 2.0, 3.0, 4.0, "red", "black", 1.0);
    svg_writer_texto(svg, 5.0, 6.0, "ola", "8", "blue");
    svg_writer_circulo_preto(svg, 7.0, 8.0, 2.0);
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(contents, "<svg"));
    TEST_ASSERT_NOT_NULL(strstr(contents, "<rect x=\"1.00\" y=\"2.00\""));
    TEST_ASSERT_NOT_NULL(strstr(contents, ">ola</text>"));
    TEST_ASSERT_NOT_NULL(strstr(contents, "<circle cx=\"7.00\" cy=\"8.00\""));
    TEST_ASSERT_NOT_NULL(strstr(contents, "</svg>"));
}

void test_svg_writer_viewbox_usa_bounds_com_margem(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_retangulo(svg, 100.0, 50.0, 30.0, 20.0,
                         "red", "black", 2.0);
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(contents,
        "width=\"72.00\" height=\"62.00\" viewBox=\"79.00 29.00 72.00 62.00\""));
}

void test_svg_writer_viewbox_aceita_coordenadas_negativas(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_circulo_preto(svg, -5.0, -10.0, 5.0);
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(contents,
        "width=\"50.00\" height=\"50.00\" viewBox=\"-30.00 -35.00 50.00 50.00\""));
}

void test_svg_writer_viewbox_inclui_marcacoes_e_texto(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_x_vermelho(svg, 0.0, 0.0, 10.0);
    svg_writer_cruz_vermelha(svg, 50.0, 10.0, 20.0);
    svg_writer_circulo_preto(svg, 100.0, 100.0, 5.0);
    svg_writer_texto(svg, -30.0, 40.0, "abcd", "10", "black");
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(contents,
        "width=\"175.00\" height=\"151.00\" viewBox=\"-50.00 -26.00 175.00 151.00\""));
}

void test_svg_writer_escreve_camada_base_antes_das_marcacoes(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;
    char *base;
    char *marcacao;

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_texto(svg, 5.0, 6.0, "marcacao", "8", "blue");
    svg_writer_retangulo_base(svg, 1.0, 2.0, 3.0, 4.0,
                              "red", "black", 1.0);
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    base = strstr(contents, "<rect x=\"1.00\" y=\"2.00\"");
    marcacao = strstr(contents, ">marcacao</text>");
    TEST_ASSERT_NOT_NULL(base);
    TEST_ASSERT_NOT_NULL(marcacao);
    TEST_ASSERT_TRUE(base < marcacao);
}

void test_svg_writer_x_quadra_removida_liga_vertices(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_x_quadra_removida(svg, 10.0, 20.0, 30.0, 40.0);
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(contents,
        "x1=\"10.00\" y1=\"20.00\" x2=\"40.00\" y2=\"60.00\""));
    TEST_ASSERT_NOT_NULL(strstr(contents,
        "x1=\"40.00\" y1=\"20.00\" x2=\"10.00\" y2=\"60.00\""));
}

void test_svg_writer_sem_elementos_usa_fallback(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 25.0, 30.0);
    char *contents;

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(contents,
        "width=\"25.00\" height=\"30.00\" viewBox=\"0.00 0.00 25.00 30.00\""));
}

void test_svg_writer_rejeita_path_null(void) {
    TEST_ASSERT_NULL(svg_writer_criar(NULL, 100.0, 100.0));
    svg_writer_destruir(NULL);
    svg_writer_finalizar(NULL);
}

/* ---- Extensões do Projeto 2 ---- */

void test_svg_writer_vertice_desenha_circulo_lilas(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_vertice(svg, 15.0, 25.0);
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(contents, "<circle cx=\"15.00\" cy=\"25.00\""));
    TEST_ASSERT_NOT_NULL(strstr(contents, "#C8A2C8"));
}

void test_svg_writer_aresta_desenha_linha_seta_e_nome(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_aresta(svg, 0.0, 0.0, 30.0, 0.0, "Rua A");
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    /* linha principal */
    TEST_ASSERT_NOT_NULL(strstr(contents,
        "x1=\"0.00\" y1=\"0.00\" x2=\"30.00\" y2=\"0.00\""));
    /* nome no meio */
    TEST_ASSERT_NOT_NULL(strstr(contents, ">Rua A</text>"));
    /* ponta de seta: pelo menos 3 linhas (1 principal + 2 asas) */
    {
        int count = 0;
        char *p = contents;
        while ((p = strstr(p, "<line")) != NULL) { count++; p += 5; }
        TEST_ASSERT_TRUE(count >= 3);
    }
}

void test_svg_writer_linha_pontilhada_vertical_com_label(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;

    TEST_ASSERT_NOT_NULL(svg);
    /* estabelece bounds com um retângulo, depois desenha a linha */
    svg_writer_retangulo_base(svg, 0.0, 0.0, 100.0, 60.0, "white", "black", 1.0);
    svg_writer_linha_pontilhada_vertical(svg, 40.0, "R3");
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(contents, "stroke-dasharray"));
    TEST_ASSERT_NOT_NULL(strstr(contents, "x1=\"40.00\""));
    TEST_ASSERT_NOT_NULL(strstr(contents, ">R3</text>"));
}

void test_svg_writer_bounding_box_semitransparente(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_bounding_box(svg, 5.0, 10.0, 50.0, 40.0, "green", 0.5);
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(contents, "fill=\"green\""));
    TEST_ASSERT_NOT_NULL(strstr(contents, "fill-opacity=\"0.50\""));
    TEST_ASSERT_NOT_NULL(strstr(contents, "width=\"50.00\" height=\"40.00\""));
}

void test_svg_writer_aresta_grossa(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_aresta_grossa(svg, 1.0, 2.0, 3.0, 4.0, "red");
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(contents, "stroke=\"red\" stroke-width=\"4.00\""));
    TEST_ASSERT_NOT_NULL(strstr(contents,
        "x1=\"1.00\" y1=\"2.00\" x2=\"3.00\" y2=\"4.00\""));
}

void test_svg_writer_percurso_animado(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;
    double pontos[] = { 0.0, 0.0, 10.0, 5.0, 20.0, 0.0 };

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_percurso_animado(svg, pontos, 3, "blue", "cam1");
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(contents, "<path id=\"cam1\""));
    TEST_ASSERT_NOT_NULL(strstr(contents, "d=\"M 0.00 0.00 L 10.00 5.00 L 20.00 0.00\""));
    TEST_ASSERT_NOT_NULL(strstr(contents, "<animateMotion"));
    TEST_ASSERT_NOT_NULL(strstr(contents, "<mpath href=\"#cam1\"/>"));
    TEST_ASSERT_NOT_NULL(strstr(contents, ">I</text>"));
    TEST_ASSERT_NOT_NULL(strstr(contents, ">F</text>"));

    /* n < 2 não desenha path */
    remove(SVG_PATH);
    svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    svg_writer_percurso_animado(svg, pontos, 1, "blue", "x");
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);
    contents = read_svg();
    TEST_ASSERT_NULL(strstr(contents, "<path"));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_svg_writer_escreve_elementos_basicos);
    RUN_TEST(test_svg_writer_viewbox_usa_bounds_com_margem);
    RUN_TEST(test_svg_writer_viewbox_aceita_coordenadas_negativas);
    RUN_TEST(test_svg_writer_viewbox_inclui_marcacoes_e_texto);
    RUN_TEST(test_svg_writer_escreve_camada_base_antes_das_marcacoes);
    RUN_TEST(test_svg_writer_x_quadra_removida_liga_vertices);
    RUN_TEST(test_svg_writer_sem_elementos_usa_fallback);
    RUN_TEST(test_svg_writer_rejeita_path_null);
    RUN_TEST(test_svg_writer_vertice_desenha_circulo_lilas);
    RUN_TEST(test_svg_writer_aresta_desenha_linha_seta_e_nome);
    RUN_TEST(test_svg_writer_linha_pontilhada_vertical_com_label);
    RUN_TEST(test_svg_writer_bounding_box_semitransparente);
    RUN_TEST(test_svg_writer_aresta_grossa);
    RUN_TEST(test_svg_writer_percurso_animado);
    return UNITY_END();
}
