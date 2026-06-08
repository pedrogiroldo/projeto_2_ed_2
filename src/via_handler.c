#include "via_handler.h"
#include "grafo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool via_carregar(const char *via_filepath,
                  Grafo grafo,
                  extensible_hash_file_t hashfile_quadras) {
    (void)hashfile_quadras; /* reservado para uso futuro em consultas */

    if (!via_filepath || !grafo) return false;

    FILE *f = fopen(via_filepath, "r");
    if (!f) return false;

    int nv = 0;
    if (fscanf(f, "%d", &nv) != 1 || nv < 0) {
        fclose(f);
        return false;
    }

    char line[512];
    /* consumir restante da linha do nv */
    if (fgets(line, sizeof(line), f) == NULL && nv > 0) {
        fclose(f);
        return false;
    }

    while (fgets(line, sizeof(line), f)) {
        char tipo = '\0';
        if (sscanf(line, " %c", &tipo) != 1) continue;

        if (tipo == 'v') {
            char id[64];
            double x, y;
            if (sscanf(line, " v %63s %lf %lf", id, &x, &y) == 3)
                grafo_inserir_vertice(grafo, id, x, y);

        } else if (tipo == 'e') {
            char i[64], j[64], ldir[64], lesq[64], nome[256];
            double cmp, vm;
            if (sscanf(line, " e %63s %63s %63s %63s %lf %lf %255s",
                       i, j, ldir, lesq, &cmp, &vm, nome) == 7)
                grafo_inserir_aresta(grafo, i, j, ldir, lesq, cmp, vm, nome);
        }
    }

    fclose(f);
    return true;
}
