#include "qry_handler.h"
#include "grafo.h"
#include "registradores.h"
#include "quadra.h"
#include "extensible_hash_file.h"
#include "txt_writer.h"
#include "unity.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static const char *QRY_PATH = "/tmp/t_qry.qry";
static const char *TXT_PATH = "/tmp/t_qry.txt";
static const char *HF_PATH  = "/tmp/t_qry.hf";

void setUp(void) {}
void tearDown(void) {
    remove(QRY_PATH);
    remove(TXT_PATH);
    remove(HF_PATH);
}

static void escrever_qry(const char *conteudo) {
    FILE *f = fopen(QRY_PATH, "w");
    fputs(conteudo, f);
    fclose(f);
}

static char *ler_txt(void) {
    static char buf[2048];
    FILE *f = fopen(TXT_PATH, "r");
    if (!f) { buf[0] = '\0'; return buf; }
    size_t n = fread(buf, 1, sizeof(buf) - 1, f);
    buf[n] = '\0';
    fclose(f);
    return buf;
}

/* aresta nos dois sentidos */
static void aresta2(Grafo g, const char *a, const char *b,
                    double cmp, double vm, const char *nome) {
    grafo_inserir_aresta(g, a, b, "-", "-", cmp, vm, nome);
    grafo_inserir_aresta(g, b, a, "-", "-", cmp, vm, nome);
}

static void inserir_quadra(extensible_hash_file_t hf, const char *cep,
                           double x, double y, double w, double h) {
    quadra_registro_t *r = malloc(quadra_registro_size());
    quadra_registro_preencher(r, cep, x, y, w, h, "white", "black", 1.0);
    ehf_insert(hf, cep, r, quadra_registro_size());
    free(r);
}

/* ---- @o? ---- */
static void test_oo_armazena_coordenada(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "A", 0, 0);

    extensible_hash_file_t hf =
        ehf_create(HF_PATH, 4, quadra_registro_size());
    inserir_quadra(hf, "01", 0, 0, 100, 50);

    Registradores regs = registradores_criar();
    txt_writer_t *txt = txt_writer_criar(TXT_PATH);

    escrever_qry("@o? R0 01 S 30\n");

    int cmds = 0, erros = 0;
    int rc = qry_processar(QRY_PATH, g, hf, regs, NULL, txt, &cmds, &erros);
    txt_writer_destruir(txt);

    TEST_ASSERT_EQUAL_INT(0, rc);
    TEST_ASSERT_EQUAL_INT(1, cmds);
    TEST_ASSERT_EQUAL_INT(0, erros);

    double x, y;
    TEST_ASSERT_TRUE(reg_obter(regs, 0, &x, &y));
    TEST_ASSERT_TRUE(fabs(x - 70.0) < 1e-9); /* (0+100)-30 */
    TEST_ASSERT_TRUE(fabs(y - 50.0) < 1e-9); /* 0+50 */

    TEST_ASSERT_NOT_NULL(strstr(ler_txt(), "R0: (70.00, 50.00)"));

    registradores_destruir(regs);
    ehf_close(hf);
    grafo_destruir(g);
}

/* ---- mvm ---- */
static void test_mvm_altera_vm_na_regiao(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "A", 0, 0);
    grafo_inserir_vertice(g, "B", 100, 0);
    aresta2(g, "A", "B", 100, 10, "rua");

    Registradores regs = registradores_criar();
    escrever_qry("mvm 99 -1 -1 5 5\n"); /* região cobre A(0,0), não B(100,0) */

    int cmds = 0, erros = 0;
    qry_processar(QRY_PATH, g, NULL, regs, NULL, NULL, &cmds, &erros);
    TEST_ASSERT_EQUAL_INT(1, cmds);

    /* aresta de saída de A → vm alterada */
    void *ca = grafo_adjacentes_inicio(g, "A");
    TEST_ASSERT_NOT_NULL(ca);
    TEST_ASSERT_TRUE(fabs(grafo_aresta_vm(ca) - 99.0) < 1e-9);

    /* aresta de saída de B → inalterada */
    void *cb = grafo_adjacentes_inicio(g, "B");
    TEST_ASSERT_NOT_NULL(cb);
    TEST_ASSERT_TRUE(fabs(grafo_aresta_vm(cb) - 10.0) < 1e-9);

    registradores_destruir(regs);
    grafo_destruir(g);
}

/* ---- regs ---- */
static void test_regs_conta_componentes(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "A", 0, 0);
    grafo_inserir_vertice(g, "B", 10, 0);
    grafo_inserir_vertice(g, "C", 100, 0);
    grafo_inserir_vertice(g, "D", 110, 0);
    aresta2(g, "A", "B", 10, 50, "r1");
    aresta2(g, "C", "D", 10, 50, "r2");
    aresta2(g, "B", "C", 90, 5, "lenta"); /* vm=5 */

    Registradores regs = registradores_criar();
    txt_writer_t *txt = txt_writer_criar(TXT_PATH);
    escrever_qry("regs 10\n"); /* corta a lenta (vm 5 < 10) → 2 componentes */

    int cmds = 0, erros = 0;
    qry_processar(QRY_PATH, g, NULL, regs, NULL, txt, &cmds, &erros);
    txt_writer_destruir(txt);

    TEST_ASSERT_EQUAL_INT(1, cmds);
    TEST_ASSERT_NOT_NULL(strstr(ler_txt(), "Numero de componentes: 2"));

    registradores_destruir(regs);
    grafo_destruir(g);
}

/* ---- exp ---- */
static void test_exp_acelera_gargalos(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "A", 0, 0);
    grafo_inserir_vertice(g, "B", 100, 0);
    aresta2(g, "A", "B", 100, 5, "gargalo"); /* vm=5 */

    Registradores regs = registradores_criar();
    escrever_qry("exp 10\n"); /* vm 5 < 10 → acelera para 7.5 */

    int cmds = 0, erros = 0;
    qry_processar(QRY_PATH, g, NULL, regs, NULL, NULL, &cmds, &erros);
    TEST_ASSERT_EQUAL_INT(1, cmds);

    void *ca = grafo_adjacentes_inicio(g, "A");
    TEST_ASSERT_TRUE(fabs(grafo_aresta_vm(ca) - 7.5) < 1e-9);
    void *cb = grafo_adjacentes_inicio(g, "B");
    TEST_ASSERT_TRUE(fabs(grafo_aresta_vm(cb) - 7.5) < 1e-9);

    registradores_destruir(regs);
    grafo_destruir(g);
}

/* ---- p? acessível ---- */
static void test_p_percurso_acessivel(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "A", 0, 0);
    grafo_inserir_vertice(g, "B", 100, 0);
    grafo_inserir_vertice(g, "C", 200, 0);
    aresta2(g, "A", "B", 100, 10, "Rua Um");
    aresta2(g, "B", "C", 100, 10, "Rua Dois");

    Registradores regs = registradores_criar();
    reg_armazenar(regs, 0, 0, 0);     /* R0 ~ A */
    reg_armazenar(regs, 1, 200, 0);   /* R1 ~ C */

    txt_writer_t *txt = txt_writer_criar(TXT_PATH);
    escrever_qry("p? R0 R1 blue red\n");

    int cmds = 0, erros = 0;
    qry_processar(QRY_PATH, g, NULL, regs, NULL, txt, &cmds, &erros);
    txt_writer_destruir(txt);

    TEST_ASSERT_EQUAL_INT(1, cmds);
    char *t = ler_txt();
    TEST_ASSERT_NOT_NULL(strstr(t, "Siga na direcao Leste"));
    TEST_ASSERT_NULL(strstr(t, "inacessivel"));

    registradores_destruir(regs);
    grafo_destruir(g);
}

/* ---- p? inacessível ---- */
static void test_p_destino_inacessivel(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "A", 0, 0);
    grafo_inserir_vertice(g, "Z", 500, 500); /* isolado */

    Registradores regs = registradores_criar();
    reg_armazenar(regs, 0, 0, 0);
    reg_armazenar(regs, 1, 500, 500);

    txt_writer_t *txt = txt_writer_criar(TXT_PATH);
    escrever_qry("p? R0 R1 blue red\n");

    qry_processar(QRY_PATH, g, NULL, regs, NULL, txt, NULL, NULL);
    txt_writer_destruir(txt);

    TEST_ASSERT_NOT_NULL(strstr(ler_txt(), "Destino inacessivel"));

    registradores_destruir(regs);
    grafo_destruir(g);
}

/* ---- comando desconhecido conta como erro ---- */
static void test_comando_desconhecido_e_erro(void) {
    Grafo g = grafo_criar();
    grafo_inserir_vertice(g, "A", 0, 0);
    Registradores regs = registradores_criar();
    escrever_qry("xyz 1 2 3\n@o? R0 99 S 1\n"); /* comando inválido + @o? com cep inexistente */

    int cmds = 0, erros = 0;
    qry_processar(QRY_PATH, g, NULL, regs, NULL, NULL, &cmds, &erros);
    TEST_ASSERT_EQUAL_INT(0, cmds);
    TEST_ASSERT_EQUAL_INT(2, erros);

    registradores_destruir(regs);
    grafo_destruir(g);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_oo_armazena_coordenada);
    RUN_TEST(test_mvm_altera_vm_na_regiao);
    RUN_TEST(test_regs_conta_componentes);
    RUN_TEST(test_exp_acelera_gargalos);
    RUN_TEST(test_p_percurso_acessivel);
    RUN_TEST(test_p_destino_inacessivel);
    RUN_TEST(test_comando_desconhecido_e_erro);
    return UNITY_END();
}
