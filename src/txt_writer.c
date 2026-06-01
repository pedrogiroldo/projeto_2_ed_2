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
