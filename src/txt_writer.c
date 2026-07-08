#include "txt_writer.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

struct txt_writer {
    FILE *file;
};

txt_writer_t *txt_writer_criar(const char *output_path) {
    txt_writer_t *tw;

    if (output_path == NULL) {
        return NULL;
    }

    tw = (txt_writer_t *)malloc(sizeof(*tw));
    if (tw == NULL) {
        return NULL;
    }

    tw->file = fopen(output_path, "w");
    if (tw->file == NULL) {
        free(tw);
        return NULL;
    }

    return tw;
}

void txt_writer_destruir(txt_writer_t *tw) {
    if (tw == NULL) {
        return;
    }
    if (tw->file != NULL) {
        fclose(tw->file);
    }
    free(tw);
}

void txt_writer_linha(txt_writer_t *tw, const char *fmt, ...) {
    va_list args;

    if (tw == NULL || tw->file == NULL || fmt == NULL) {
        return;
    }

    va_start(args, fmt);
    vfprintf(tw->file, fmt, args);
    va_end(args);
    fprintf(tw->file, "\n");
}

/* ======================================================================== */
/* Extensões do Projeto 2                                                   */
/* ======================================================================== */

void txt_writer_endereco(txt_writer_t *tw, const char *reg,
                         double x, double y) {
    txt_writer_linha(tw, "%s: (%.2f, %.2f)",
                     reg != NULL ? reg : "?", x, y);
}

void txt_writer_num_componentes(txt_writer_t *tw, int num) {
    txt_writer_linha(tw, "Numero de componentes: %d", num);
}

void txt_writer_passo(txt_writer_t *tw, const char *direcao, const char *rua) {
    txt_writer_linha(tw, "Siga na direcao %s pela %s",
                     direcao != NULL ? direcao : "?",
                     rua != NULL ? rua : "?");
}

void txt_writer_destino_inacessivel(txt_writer_t *tw) {
    txt_writer_linha(tw, "Destino inacessivel");
}

const char *txt_writer_direcao_cardeal(double dx, double dy) {
    double adx = dx < 0 ? -dx : dx;
    double ady = dy < 0 ? -dy : dy;

    if (adx == 0.0 && ady == 0.0) {
        return "";
    }
    if (adx >= ady) {
        return dx > 0 ? "Leste" : "Oeste";
    }
    /* Y cresce para baixo: dy < 0 aponta para o Norte */
    return dy < 0 ? "Norte" : "Sul";
}
