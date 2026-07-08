#include "geo_handler.h"

#include "file_reader.h"
#include "quadra.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GEO_COR_MAX 64
#define GEO_CEP_MAX 64

typedef struct {
    int quadras_inseridas;
    int erros;
} geo_resultado_interno_t;

typedef struct {
    double sw;
    char cfill[GEO_COR_MAX];
    char cstrk[GEO_COR_MAX];
    int definido;
} geo_estilo_t;

static void geo_processar_cq(const char *linha, geo_estilo_t *estilo) {
    char cfill[GEO_COR_MAX];
    char cstrk[GEO_COR_MAX];
    char sw_token[64];
    double sw;
    char extra;

    if (sscanf(linha, "cq %63s %63s %63s %c", sw_token, cfill, cstrk, &extra) != 3)
        return;

    if (sscanf(sw_token, "%lfpx", &sw) != 1 &&
        sscanf(sw_token, "%lf", &sw) != 1)
        return;

    if (sw < 0.0)
        return;

    estilo->sw = sw;
    strncpy(estilo->cfill, cfill, GEO_COR_MAX - 1);
    estilo->cfill[GEO_COR_MAX - 1] = '\0';
    strncpy(estilo->cstrk, cstrk, GEO_COR_MAX - 1);
    estilo->cstrk[GEO_COR_MAX - 1] = '\0';
    estilo->definido = 1;
}

static void geo_processar_q(const char *linha, const geo_estilo_t *estilo,
                             extensible_hash_file_t quadras_hf,
                             svg_writer_t *svg,
                             geo_resultado_interno_t *res) {
    char cep[GEO_CEP_MAX];
    double x, y, w, h;
    quadra_t *q;
    quadra_registro_t *reg;
    ehf_status_t status;

    if (sscanf(linha, "q %63s %lf %lf %lf %lf", cep, &x, &y, &w, &h) != 5
        || w <= 0.0 || h <= 0.0) {
        res->erros++;
        return;
    }

    if (!estilo->definido) {
        res->erros++;
        return;
    }

    q = quadra_criar(cep, x, y, w, h, estilo->cfill, estilo->cstrk, estilo->sw);
    if (q == NULL) {
        res->erros++;
        return;
    }

    reg = malloc(quadra_registro_size());
    if (reg == NULL || quadra_para_registro(q, reg) == 0) {
        free(reg);
        quadra_destruir(q);
        res->erros++;
        return;
    }

    status = ehf_insert(quadras_hf, cep, reg, quadra_registro_size());
    free(reg);
    if (status == EHF_OK) {
        res->quadras_inseridas++;
        if (svg != NULL)
            svg_writer_retangulo(svg, x, y, w, h,
                                 estilo->cfill, estilo->cstrk, estilo->sw);
    } else {
        res->erros++;
    }

    quadra_destruir(q);
}

geo_handler_resultado_t geo_handler_processar(const char *geo_filepath,
                                              extensible_hash_file_t quadras_hf,
                                              svg_writer_t *svg) {
    geo_resultado_interno_t *res = calloc(1, sizeof(geo_resultado_interno_t));
    geo_estilo_t estilo;
    FileData fd;
    Queue linhas;
    char *linha;

    memset(&estilo, 0, sizeof(estilo));

    if (geo_filepath == NULL || quadras_hf == NULL) {
        res->erros++;
        return res;
    }

    fd = file_data_create(geo_filepath);
    if (fd == NULL) {
        res->erros++;
        return res;
    }

    linhas = get_file_lines_queue(fd);

    while (!queue_is_empty(linhas)) {
        linha = (char *)queue_dequeue(linhas);
        if (linha == NULL)
            continue;

        if (strncmp(linha, "cq ", 3) == 0)
            geo_processar_cq(linha, &estilo);
        else if (strncmp(linha, "q ", 2) == 0)
            geo_processar_q(linha, &estilo, quadras_hf, svg, res);
    }

    file_data_destroy(fd);
    return res;
}

int geo_handler_resultado_inseridas(geo_handler_resultado_t resultado) {
    return ((geo_resultado_interno_t *)resultado)->quadras_inseridas;
}

int geo_handler_resultado_erros(geo_handler_resultado_t resultado) {
    return ((geo_resultado_interno_t *)resultado)->erros;
}

void geo_handler_resultado_destruir(geo_handler_resultado_t resultado) {
    free(resultado);
}
