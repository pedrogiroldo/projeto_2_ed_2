#include "qry_handler.h"
#include "grafo.h"
#include "registradores.h"
#include "mst.h"
#include "componentes.h"
#include "dijkstra.h"
#include "quadra.h"
#include "file_reader.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ---- pesos para o Dijkstra ---- */

static double peso_distancia(void *aresta) {
    return grafo_aresta_cmp(aresta);
}

static double peso_tempo(void *aresta) {
    double vm = grafo_aresta_vm(aresta);
    if (vm <= 0.0) return INFINITY;
    return grafo_aresta_cmp(aresta) / vm;
}

/* ---- helpers de geometria/grafo ---- */

static int dentro_regiao(double px, double py,
                         double x, double y, double w, double h) {
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

/* Retorna o cursor da aresta from->to, ou NULL se não existir */
static void *achar_aresta(Grafo g, const char *from, const char *to) {
    for (void *cur = grafo_adjacentes_inicio(g, from);
         cur != NULL;
         cur = grafo_adjacentes_fim(g, cur)) {
        if (strcmp(grafo_aresta_destino(cur), to) == 0)
            return cur;
    }
    return NULL;
}

/* Retorna o vértice (cursor opaco) mais próximo de (x, y), ou NULL se grafo vazio */
static void *vertice_mais_proximo(Grafo g, double x, double y) {
    int n = grafo_num_vertices(g);
    void *melhor = NULL;
    double melhor_d2 = INFINITY;
    for (int i = 0; i < n; i++) {
        void *v = grafo_vertice_por_indice(g, i);
        if (!v) continue;
        double dx = grafo_vertice_x(v) - x;
        double dy = grafo_vertice_y(v) - y;
        double d2 = dx * dx + dy * dy;
        if (d2 < melhor_d2) {
            melhor_d2 = d2;
            melhor = v;
        }
    }
    return melhor;
}

/* ===================================================================== */
/* Handler @o? — reg cep face num                                        */
/* ===================================================================== */

static int handle_oo(const char *linha, Grafo g,
                     extensible_hash_file_t hf, Registradores regs,
                     svg_writer_t *svg, txt_writer_t *txt) {
    char reg[64], cep[64], face_str[8];
    double num;
    (void)g;

    if (sscanf(linha, "@o? %63s %63s %7s %lf", reg, cep, face_str, &num) != 4)
        return -1;

    int idx = reg_indice(reg);
    if (idx < 0) return -1;

    quadra_registro_t *registro = malloc(quadra_registro_size());
    if (!registro) return -1;
    if (!hf || ehf_find(hf, cep, registro, quadra_registro_size()) != EHF_OK) {
        free(registro);
        return -1;
    }

    double x, y;
    int ok = reg_coordenada_endereco(quadra_registro_x(registro),
                                     quadra_registro_y(registro),
                                     quadra_registro_largura(registro),
                                     quadra_registro_altura(registro),
                                     face_str[0], num, &x, &y);
    free(registro);
    if (!ok)
        return -1;

    reg_armazenar(regs, idx, x, y);
    if (svg) svg_writer_linha_pontilhada_vertical(svg, x, reg);
    if (txt) txt_writer_endereco(txt, reg, x, y);
    return 0;
}

/* ===================================================================== */
/* Handler mvm — v x y w h                                               */
/* ===================================================================== */

static int handle_mvm(const char *linha, Grafo g) {
    double v, x, y, w, h;
    if (sscanf(linha, "mvm %lf %lf %lf %lf %lf", &v, &x, &y, &w, &h) != 5)
        return -1;

    int n = grafo_num_vertices(g);
    for (int i = 0; i < n; i++) {
        void *vert = grafo_vertice_por_indice(g, i);
        if (!vert) continue;
        if (!dentro_regiao(grafo_vertice_x(vert), grafo_vertice_y(vert),
                           x, y, w, h))
            continue;
        const char *id = grafo_vertice_id(vert);
        for (void *cur = grafo_adjacentes_inicio(g, id);
             cur != NULL;
             cur = grafo_adjacentes_fim(g, cur)) {
            grafo_aresta_set_vm(cur, v);
        }
    }
    return 0;
}

/* ===================================================================== */
/* Handler regs — vl                                                     */
/* ===================================================================== */

static const char *COR_COMPONENTE[] = {
    "red", "green", "blue", "orange", "purple",
    "brown", "magenta", "teal", "olive", "navy"
};
#define NUM_CORES_COMPONENTE 10

static int handle_regs(const char *linha, Grafo g,
                       svg_writer_t *svg, txt_writer_t *txt) {
    double vl;
    if (sscanf(linha, "regs %lf", &vl) != 1)
        return -1;

    int num = 0;
    int *comp = componentes_calcular(g, vl, &num);

    if (txt) txt_writer_num_componentes(txt, num);

    if (svg && comp && num > 0) {
        /* bounding box de cada componente */
        double *minx = malloc(num * sizeof(double));
        double *miny = malloc(num * sizeof(double));
        double *maxx = malloc(num * sizeof(double));
        double *maxy = malloc(num * sizeof(double));
        if (minx && miny && maxx && maxy) {
            for (int c = 0; c < num; c++) {
                minx[c] = INFINITY;  miny[c] = INFINITY;
                maxx[c] = -INFINITY; maxy[c] = -INFINITY;
            }
            int n = grafo_num_vertices(g);
            for (int i = 0; i < n; i++) {
                void *v = grafo_vertice_por_indice(g, i);
                if (!v) continue;
                int c = comp[i];
                if (c < 0 || c >= num) continue;
                double vx = grafo_vertice_x(v), vy = grafo_vertice_y(v);
                if (vx < minx[c]) minx[c] = vx;
                if (vy < miny[c]) miny[c] = vy;
                if (vx > maxx[c]) maxx[c] = vx;
                if (vy > maxy[c]) maxy[c] = vy;
            }
            for (int c = 0; c < num; c++) {
                if (minx[c] > maxx[c]) continue; /* componente vazia */
                svg_writer_bounding_box(svg, minx[c], miny[c],
                                        maxx[c] - minx[c], maxy[c] - miny[c],
                                        COR_COMPONENTE[c % NUM_CORES_COMPONENTE],
                                        0.5);
            }
        }
        free(minx); free(miny); free(maxx); free(maxy);
    }

    free(comp);
    return 0;
}

/* ===================================================================== */
/* Handler exp — vl                                                      */
/* ===================================================================== */

/* Acelera em 50% a aresta from->to se existir e vm < vl. Retorna 1 se acelerou. */
static int acelerar_se_gargalo(Grafo g, const char *from, const char *to,
                               double vl) {
    void *cur = achar_aresta(g, from, to);
    if (!cur) return 0;
    double vm = grafo_aresta_vm(cur);
    if (vm < vl) {
        grafo_aresta_set_vm(cur, vm * 1.5);
        return 1;
    }
    return 0;
}

static int handle_exp(const char *linha, Grafo g, svg_writer_t *svg) {
    double vl;
    if (sscanf(linha, "exp %lf", &vl) != 1)
        return -1;

    MstResultado res = mst_calcular(g);
    if (!res) return 0; /* grafo vazio: nada a expandir */

    int m = mst_num_arestas(res);
    for (int i = 0; i < m; i++) {
        const char *o = mst_aresta_origem(res, i);
        const char *d = mst_aresta_destino(res, i);

        int acel = 0;
        acel |= acelerar_se_gargalo(g, o, d, vl);
        acel |= acelerar_se_gargalo(g, d, o, vl);

        if (acel && svg) {
            void *vo = grafo_buscar_vertice(g, o);
            void *vd = grafo_buscar_vertice(g, d);
            if (vo && vd) {
                svg_writer_aresta_grossa(svg,
                                         grafo_vertice_x(vo), grafo_vertice_y(vo),
                                         grafo_vertice_x(vd), grafo_vertice_y(vd),
                                         "red");
            }
        }
    }

    mst_resultado_destruir(res);
    return 0;
}

/* ===================================================================== */
/* Handler p? — reg1 reg2 cc cr                                          */
/* ===================================================================== */

typedef struct {
    Grafo         g;
    double       *pts;
    int           n;
    int           cap;
    double        prev_x;
    double        prev_y;
    txt_writer_t *txt; /* NULL para não narrar passos */
} PercursoCtx;

static void percurso_push(PercursoCtx *ctx, double x, double y) {
    if (ctx->n == ctx->cap) {
        int nova = ctx->cap == 0 ? 8 : ctx->cap * 2;
        double *tmp = realloc(ctx->pts, nova * 2 * sizeof(double));
        if (!tmp) return;
        ctx->pts = tmp;
        ctx->cap = nova;
    }
    ctx->pts[2 * ctx->n] = x;
    ctx->pts[2 * ctx->n + 1] = y;
    ctx->n++;
}

static void percurso_cb(void *cursor_aresta, void *vctx) {
    PercursoCtx *ctx = vctx;
    const char *destino = grafo_aresta_destino(cursor_aresta);
    void *vd = grafo_buscar_vertice(ctx->g, destino);
    if (!vd) return;
    double x = grafo_vertice_x(vd), y = grafo_vertice_y(vd);

    if (ctx->txt) {
        const char *dir = txt_writer_direcao_cardeal(x - ctx->prev_x,
                                                     y - ctx->prev_y);
        txt_writer_passo(ctx->txt, dir, grafo_aresta_nome(cursor_aresta));
    }
    percurso_push(ctx, x, y);
    ctx->prev_x = x;
    ctx->prev_y = y;
}

/* Executa Dijkstra com o peso dado, desenha o percurso na cor e (se txt) narra. */
static void trilhar(Grafo g, const char *id_org, void *vorg,
                    const char *id_dst,
                    double (*peso)(void *), const char *cor,
                    const char *id_path,
                    svg_writer_t *svg, txt_writer_t *txt) {
    DijkstraResultado res = dijkstra_executar(g, id_org, peso);
    if (!res) return;

    if (!dijkstra_alcancavel(res, id_dst)) {
        if (txt) txt_writer_destino_inacessivel(txt);
        dijkstra_resultado_destruir(res);
        return;
    }

    PercursoCtx ctx;
    ctx.g = g;
    ctx.pts = NULL;
    ctx.n = 0;
    ctx.cap = 0;
    ctx.prev_x = grafo_vertice_x(vorg);
    ctx.prev_y = grafo_vertice_y(vorg);
    ctx.txt = txt;

    /* Ponto inicial: vértice de origem */
    percurso_push(&ctx, ctx.prev_x, ctx.prev_y);
    dijkstra_caminho_por_arestas(res, g, id_dst, percurso_cb, &ctx);

    if (svg && ctx.n >= 2) {
        svg_writer_percurso_animado(svg, ctx.pts, ctx.n, cor, id_path);
    }

    free(ctx.pts);
    dijkstra_resultado_destruir(res);
}

static int handle_p(const char *linha, Grafo g, Registradores regs,
                    svg_writer_t *svg, txt_writer_t *txt) {
    char r1[64], r2[64], cc[64], cr[64];
    if (sscanf(linha, "p? %63s %63s %63s %63s", r1, r2, cc, cr) != 4)
        return -1;

    int i1 = reg_indice(r1), i2 = reg_indice(r2);
    if (i1 < 0 || i2 < 0) return -1;

    double x1, y1, x2, y2;
    if (!reg_obter(regs, i1, &x1, &y1) || !reg_obter(regs, i2, &x2, &y2))
        return -1;

    void *vorg = vertice_mais_proximo(g, x1, y1);
    void *vdst = vertice_mais_proximo(g, x2, y2);
    if (!vorg || !vdst) return -1;

    /* Cópias dos ids: os cursores podem ser invalidados por buscas subsequentes */
    char id_org[64], id_dst[64];
    strncpy(id_org, grafo_vertice_id(vorg), sizeof(id_org) - 1);
    id_org[sizeof(id_org) - 1] = '\0';
    strncpy(id_dst, grafo_vertice_id(vdst), sizeof(id_dst) - 1);
    id_dst[sizeof(id_dst) - 1] = '\0';

    /* Percurso mais curto (distância, cor cc) — narra passos no TXT */
    trilhar(g, id_org, vorg, id_dst, peso_distancia, cc, "caminho_curto",
            svg, txt);
    /* Percurso mais rápido (tempo, cor cr) — apenas SVG */
    trilhar(g, id_org, vorg, id_dst, peso_tempo, cr, "caminho_rapido",
            svg, NULL);

    return 0;
}

/* ===================================================================== */
/* Dispatcher                                                            */
/* ===================================================================== */

int qry_processar(const char *qry_filepath,
                  Grafo grafo,
                  extensible_hash_file_t quadras_hf,
                  Registradores registradores,
                  svg_writer_t *svg,
                  txt_writer_t *txt,
                  int *comandos_out,
                  int *erros_out) {
    int comandos = 0, erros = 0;

    if (comandos_out) *comandos_out = 0;
    if (erros_out) *erros_out = 0;

    if (!qry_filepath || !grafo)
        return -1;

    FileData fd = file_data_create(qry_filepath);
    if (!fd) return -1;

    Queue linhas = get_file_lines_queue(fd);

    while (!queue_is_empty(linhas)) {
        char *linha = (char *)queue_dequeue(linhas);
        if (!linha) continue;

        char cmd[16];
        if (sscanf(linha, "%15s", cmd) != 1)
            continue; /* linha em branco */

        int r;
        if (strcmp(cmd, "@o?") == 0)
            r = handle_oo(linha, grafo, quadras_hf, registradores, svg, txt);
        else if (strcmp(cmd, "mvm") == 0)
            r = handle_mvm(linha, grafo);
        else if (strcmp(cmd, "regs") == 0)
            r = handle_regs(linha, grafo, svg, txt);
        else if (strcmp(cmd, "exp") == 0)
            r = handle_exp(linha, grafo, svg);
        else if (strcmp(cmd, "p?") == 0)
            r = handle_p(linha, grafo, registradores, svg, txt);
        else
            r = -1; /* comando desconhecido */

        if (r == 0) comandos++;
        else erros++;
    }

    file_data_destroy(fd);

    if (comandos_out) *comandos_out = comandos;
    if (erros_out) *erros_out = erros;
    return 0;
}
