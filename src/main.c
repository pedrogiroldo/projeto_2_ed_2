#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "args_handler.h"
#include "extensible_hash_file.h"
#include "geo_handler.h"
#include "via_handler.h"
#include "qry_handler.h"
#include "grafo.h"
#include "registradores.h"
#include "quadra.h"
#include "svg_writer.h"
#include "txt_writer.h"

#define PATH_MAX_LEN 1024

/* ---- utilitários de caminho ---- */

static void join_path(char *out, size_t out_size,
                      const char *dir, const char *file) {
    snprintf(out, out_size, "%s/%s", dir, file);
}

static void strip_ext(char *buf, size_t buf_size, const char *filename) {
    const char *dot, *slash, *base_name;
    size_t len;

    slash = strrchr(filename, '/');
    base_name = slash != NULL ? slash + 1 : filename;
    dot = strrchr(base_name, '.');
    len = dot != NULL ? (size_t)(dot - base_name) : strlen(base_name);
    if (len >= buf_size) len = buf_size - 1u;
    memcpy(buf, base_name, len);
    buf[len] = '\0';
}

static int ensure_output_dir(const char *path) {
    struct stat st;

    if (path == NULL || path[0] == '\0')
        return 0;
    if (stat(path, &st) == 0)
        return S_ISDIR(st.st_mode);
    if (errno == ENOENT)
        return mkdir(path, 0775) == 0;
    return 0;
}

static int compose_base(char *out, size_t out_size,
                        const char *geo_base, const char *qry_base) {
    size_t gl = strlen(geo_base), ql = strlen(qry_base);
    if (gl + 1u + ql >= out_size)
        return 0;
    memcpy(out, geo_base, gl);
    out[gl] = '-';
    memcpy(out + gl + 1u, qry_base, ql + 1u);
    return 1;
}

/* ---- desenho do mapa e do grafo ---- */

static void desenhar_quadra_visitor(const char *key, const void *record,
                                    size_t record_size, void *user_data) {
    svg_writer_t *svg = (svg_writer_t *)user_data;
    (void)key;
    (void)record_size;
    svg_writer_retangulo_base(svg,
                              quadra_registro_x(record),
                              quadra_registro_y(record),
                              quadra_registro_largura(record),
                              quadra_registro_altura(record),
                              quadra_registro_cor_preenchimento(record),
                              quadra_registro_cor_borda(record),
                              quadra_registro_espessura_borda(record));
}

static void desenhar_mapa_base(extensible_hash_file_t quadras_hf,
                               svg_writer_t *svg) {
    ehf_foreach(quadras_hf, desenhar_quadra_visitor,
                quadra_registro_size(), svg);
}

static void desenhar_grafo(Grafo g, svg_writer_t *svg) {
    int n = grafo_num_vertices(g);

    /* arestas primeiro, para os vértices ficarem por cima */
    for (int i = 0; i < n; i++) {
        void *v = grafo_vertice_por_indice(g, i);
        if (!v) continue;
        double x1 = grafo_vertice_x(v), y1 = grafo_vertice_y(v);
        const char *id = grafo_vertice_id(v);
        for (void *cur = grafo_adjacentes_inicio(g, id);
             cur != NULL;
             cur = grafo_adjacentes_fim(g, cur)) {
            void *vd = grafo_buscar_vertice(g, grafo_aresta_destino(cur));
            if (!vd) continue;
            svg_writer_aresta(svg, x1, y1,
                              grafo_vertice_x(vd), grafo_vertice_y(vd),
                              grafo_aresta_nome(cur));
        }
    }
    for (int i = 0; i < n; i++) {
        void *v = grafo_vertice_por_indice(g, i);
        if (!v) continue;
        svg_writer_vertice(svg, grafo_vertice_x(v), grafo_vertice_y(v));
    }
}

int main(int argc, char **argv) {
    char *bed = get_option_value(argc, argv, "e");
    char *bsd = get_option_value(argc, argv, "o");
    char *geo_file = get_option_value(argc, argv, "f");
    char *via_file = get_option_value(argc, argv, "v");
    char *qry_file = get_option_value(argc, argv, "q");

    char geo_path[PATH_MAX_LEN], via_path[PATH_MAX_LEN], qry_path[PATH_MAX_LEN];
    char geo_base[PATH_MAX_LEN], qry_base[PATH_MAX_LEN], out_base[PATH_MAX_LEN];
    char geo_svg_path[PATH_MAX_LEN], qry_svg_path[PATH_MAX_LEN], txt_path[PATH_MAX_LEN];
    char quad_hf_path[PATH_MAX_LEN];

    extensible_hash_file_t quadras_hf = NULL;
    Grafo grafo = NULL;
    svg_writer_t *svg = NULL;
    txt_writer_t *txt = NULL;

    if (bed == NULL) bed = ".";

    if (bsd == NULL || geo_file == NULL) {
        fprintf(stderr, "uso: ted [-e <BED>] -f <arq.geo> -o <BSD>"
                        " [-v <arq.via>] [-q <arq.qry>]\n");
        return 1;
    }

    if (!ensure_output_dir(bsd)) {
        fprintf(stderr, "erro: diretorio de saida invalido: %s\n", bsd);
        return 1;
    }

    join_path(geo_path, sizeof(geo_path), bed, geo_file);
    strip_ext(geo_base, sizeof(geo_base), geo_file);

    /* nome-base de saída: <geo> ou <geo>-<qry> */
    if (qry_file != NULL) {
        strip_ext(qry_base, sizeof(qry_base), qry_file);
        if (!compose_base(out_base, sizeof(out_base), geo_base, qry_base)) {
            fprintf(stderr, "erro: nome de saida muito longo\n");
            return 1;
        }
    } else {
        memcpy(out_base, geo_base, strlen(geo_base) + 1u);
    }

    snprintf(quad_hf_path, sizeof(quad_hf_path), "%.900s/%.80s-quadras.hf",
             bsd, out_base);

    /* ---- 1. .geo → hashfile de quadras + SVG base ---- */
    quadras_hf = ehf_create(quad_hf_path, 10u, quadra_registro_size());
    if (quadras_hf == NULL) {
        fprintf(stderr, "erro: nao foi possivel criar hashfile de quadras\n");
        return 1;
    }

    join_path(geo_svg_path, sizeof(geo_svg_path), bsd, geo_base);
    strncat(geo_svg_path, ".svg",
            sizeof(geo_svg_path) - strlen(geo_svg_path) - 1u);

    svg = svg_writer_criar(geo_svg_path, 1.0, 1.0);
    if (svg != NULL) {
        geo_handler_resultado_t geo_res =
            geo_handler_processar(geo_path, quadras_hf, svg);
        fprintf(stdout, "quadras inseridas: %d\n",
                geo_handler_resultado_inseridas(geo_res));
        geo_handler_resultado_destruir(geo_res);
        svg_writer_finalizar(svg);
        svg_writer_destruir(svg);
        svg = NULL;
    }

    /* ---- 2. .via → grafo ---- */
    grafo = grafo_criar();
    if (grafo == NULL) {
        fprintf(stderr, "erro: nao foi possivel criar o grafo\n");
        ehf_close(quadras_hf);
        return 1;
    }
    if (via_file != NULL) {
        join_path(via_path, sizeof(via_path), bed, via_file);
        if (!via_carregar(via_path, grafo, quadras_hf))
            fprintf(stderr, "aviso: falha ao carregar %s\n", via_path);
        fprintf(stdout, "vertices: %d  arestas: %d\n",
                grafo_num_vertices(grafo), grafo_num_arestas(grafo));
    }

    /* ---- 3. .qry → consultas → SVG + TXT ---- */
    if (qry_file != NULL) {
        Registradores regs = registradores_criar();

        join_path(qry_path, sizeof(qry_path), bed, qry_file);
        join_path(qry_svg_path, sizeof(qry_svg_path), bsd, out_base);
        strncat(qry_svg_path, ".svg",
                sizeof(qry_svg_path) - strlen(qry_svg_path) - 1u);
        join_path(txt_path, sizeof(txt_path), bsd, out_base);
        strncat(txt_path, ".txt", sizeof(txt_path) - strlen(txt_path) - 1u);

        svg = svg_writer_criar(qry_svg_path, 1.0, 1.0);
        txt = txt_writer_criar(txt_path);

        if (svg != NULL) {
            desenhar_mapa_base(quadras_hf, svg);
            desenhar_grafo(grafo, svg);
        }

        int cmds = 0, erros = 0;
        qry_processar(qry_path, grafo, quadras_hf, regs, svg, txt,
                      &cmds, &erros);
        fprintf(stdout, "comandos .qry: %d  erros: %d\n", cmds, erros);

        if (txt != NULL) txt_writer_destruir(txt);
        if (svg != NULL) {
            svg_writer_finalizar(svg);
            svg_writer_destruir(svg);
        }
        registradores_destruir(regs);
    }

    grafo_destruir(grafo);
    ehf_close(quadras_hf);
    return 0;
}
