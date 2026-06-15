#include "componentes.h"
#include "grafo.h"

#include <stdlib.h>
#include <string.h>

/* ---- adjacência bidirecional local ---- */

typedef struct {
    int *vizinhos;
    int  n;
    int  cap;
} AdjLocal;

static void adj_init(AdjLocal *a) {
    a->vizinhos = NULL;
    a->n = 0;
    a->cap = 0;
}

static void adj_push(AdjLocal *a, int v) {
    if (a->n == a->cap) {
        int nova_cap = a->cap == 0 ? 4 : a->cap * 2;
        int *tmp = realloc(a->vizinhos, nova_cap * sizeof(int));
        if (!tmp) return;
        a->vizinhos = tmp;
        a->cap = nova_cap;
    }
    a->vizinhos[a->n++] = v;
}

static void adj_free(AdjLocal *a) {
    free(a->vizinhos);
    a->vizinhos = NULL;
    a->n = 0;
    a->cap = 0;
}

/* Mapeia id de vértice para índice 0..n-1 */
static int id_para_indice(Grafo g, int n, const char *id) {
    for (int i = 0; i < n; i++) {
        void *v = grafo_vertice_por_indice(g, i);
        if (v && strcmp(grafo_vertice_id(v), id) == 0)
            return i;
    }
    return -1;
}

/* ---- API pública ---- */

int *componentes_calcular(Grafo g, double vm_minima, int *num_comp_out) {
    if (!g) return NULL;

    int n = grafo_num_vertices(g);
    if (n == 0) {
        if (num_comp_out) *num_comp_out = 0;
        return NULL;
    }

    /* Pré-construir adjacência bidirecional local (respeitando vm_minima) */
    AdjLocal *adj = malloc(n * sizeof(AdjLocal));
    if (!adj) return NULL;
    for (int i = 0; i < n; i++) adj_init(&adj[i]);

    for (int u = 0; u < n; u++) {
        void *vert = grafo_vertice_por_indice(g, u);
        if (!vert) continue;
        const char *uid = grafo_vertice_id(vert);

        for (void *cur = grafo_adjacentes_inicio(g, uid);
             cur != NULL;
             cur = grafo_adjacentes_fim(g, cur)) {

            if (grafo_aresta_vm(cur) < vm_minima) continue;

            const char *vid = grafo_aresta_destino(cur);
            int v = id_para_indice(g, n, vid);
            if (v < 0) continue;

            /* Adicionar nas duas direções (não-dirigido) */
            adj_push(&adj[u], v);
            adj_push(&adj[v], u);
        }
    }

    /* BFS para componentes */
    int *comp  = malloc(n * sizeof(int));
    int *fila  = malloc(n * sizeof(int));
    if (!comp || !fila) {
        free(comp);
        free(fila);
        for (int i = 0; i < n; i++) adj_free(&adj[i]);
        free(adj);
        return NULL;
    }
    for (int i = 0; i < n; i++) comp[i] = -1;

    int componente_id = 0;

    for (int s = 0; s < n; s++) {
        if (comp[s] != -1) continue;

        /* BFS a partir de s */
        int head = 0, tail = 0;
        fila[tail++] = s;
        comp[s] = componente_id;

        while (head < tail) {
            int u = fila[head++];
            for (int k = 0; k < adj[u].n; k++) {
                int v = adj[u].vizinhos[k];
                if (comp[v] == -1) {
                    comp[v] = componente_id;
                    fila[tail++] = v;
                }
            }
        }

        componente_id++;
    }

    if (num_comp_out) *num_comp_out = componente_id;

    free(fila);
    for (int i = 0; i < n; i++) adj_free(&adj[i]);
    free(adj);

    return comp;
}
