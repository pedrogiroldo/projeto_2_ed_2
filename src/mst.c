#include "mst.h"
#include "grafo.h"

#include <stdlib.h>
#include <string.h>

/* ---- estruturas internas (definidas APENAS no .c) ---- */

typedef struct {
    char  *origem;
    char  *destino;
    double cmp;
} MstArestaInterna;

typedef struct {
    MstArestaInterna *arestas;
    int               n;
    double            peso_total;
} MstResultadoInterno;

/* Aresta candidata (índices de vértice) usada na ordenação de Kruskal */
typedef struct {
    int    u;
    int    v;
    double cmp;
} Candidata;

/* ---- union-find ---- */

static int uf_find(int *parent, int x) {
    while (parent[x] != x) {
        parent[x] = parent[parent[x]]; /* path halving */
        x = parent[x];
    }
    return x;
}

static void uf_union(int *parent, int *rank, int a, int b) {
    int ra = uf_find(parent, a);
    int rb = uf_find(parent, b);
    if (ra == rb) return;
    if (rank[ra] < rank[rb]) { int t = ra; ra = rb; rb = t; }
    parent[rb] = ra;
    if (rank[ra] == rank[rb]) rank[ra]++;
}

/* ---- helpers ---- */

/* strdup local: strdup não é C99 padrão (evita declaração implícita) */
static char *xstrdup(const char *s) {
    if (!s) return NULL;
    char *copy = malloc(strlen(s) + 1);
    if (copy) strcpy(copy, s);
    return copy;
}

static int cmp_candidata(const void *a, const void *b) {
    double ca = ((const Candidata *)a)->cmp;
    double cb = ((const Candidata *)b)->cmp;
    if (ca < cb) return -1;
    if (ca > cb) return 1;
    return 0;
}

/* Busca linear de id de vértice → índice (0..n-1) */
static int id_para_indice(char **ids, int n, const char *id) {
    for (int i = 0; i < n; i++)
        if (strcmp(ids[i], id) == 0) return i;
    return -1;
}

/* ---- API pública ---- */

MstResultado mst_calcular(Grafo g) {
    if (!g) return NULL;

    int n = grafo_num_vertices(g);
    if (n == 0) return NULL;

    /* Cache de ids por índice para busca */
    char **ids = malloc(n * sizeof(char *));
    if (!ids) return NULL;
    for (int i = 0; i < n; i++) {
        void *vert = grafo_vertice_por_indice(g, i);
        ids[i] = (char *)grafo_vertice_id(vert); /* string interna do grafo */
    }

    /* Coletar arestas candidatas (todas as direcionadas; Kruskal trata paralelas) */
    int max_cand = grafo_num_arestas(g);
    Candidata *cand = NULL;
    if (max_cand > 0) {
        cand = malloc(max_cand * sizeof(Candidata));
        if (!cand) { free(ids); return NULL; }
    }

    int nc = 0;
    for (int u = 0; u < n; u++) {
        const char *uid = ids[u];
        for (void *cur = grafo_adjacentes_inicio(g, uid);
             cur != NULL;
             cur = grafo_adjacentes_fim(g, cur)) {
            int v = id_para_indice(ids, n, grafo_aresta_destino(cur));
            if (v < 0 || v == u) continue;
            cand[nc].u = u;
            cand[nc].v = v;
            cand[nc].cmp = grafo_aresta_cmp(cur);
            nc++;
        }
    }

    if (nc > 1) qsort(cand, nc, sizeof(Candidata), cmp_candidata);

    /* Kruskal */
    int *parent = malloc(n * sizeof(int));
    int *rank   = calloc(n, sizeof(int));
    MstResultadoInterno *res = malloc(sizeof(MstResultadoInterno));
    if (!parent || !rank || !res) {
        free(parent); free(rank); free(res); free(cand); free(ids);
        return NULL;
    }
    for (int i = 0; i < n; i++) parent[i] = i;

    res->arestas = (n > 1) ? malloc((n - 1) * sizeof(MstArestaInterna)) : NULL;
    res->n = 0;
    res->peso_total = 0.0;

    for (int i = 0; i < nc && res->n < n - 1; i++) {
        int u = cand[i].u, v = cand[i].v;
        if (uf_find(parent, u) == uf_find(parent, v)) continue;
        uf_union(parent, rank, u, v);

        MstArestaInterna *a = &res->arestas[res->n];
        a->origem  = xstrdup(ids[u]);
        a->destino = xstrdup(ids[v]);
        a->cmp     = cand[i].cmp;
        res->peso_total += cand[i].cmp;
        res->n++;
    }

    free(parent);
    free(rank);
    free(cand);
    free(ids);

    return res;
}

void mst_resultado_destruir(MstResultado r) {
    if (!r) return;
    MstResultadoInterno *res = r;
    for (int i = 0; i < res->n; i++) {
        free(res->arestas[i].origem);
        free(res->arestas[i].destino);
    }
    free(res->arestas);
    free(res);
}

int mst_num_arestas(MstResultado r) {
    if (!r) return 0;
    return ((MstResultadoInterno *)r)->n;
}

double mst_peso_total(MstResultado r) {
    if (!r) return 0.0;
    return ((MstResultadoInterno *)r)->peso_total;
}

const char *mst_aresta_origem(MstResultado r, int i) {
    if (!r) return NULL;
    MstResultadoInterno *res = r;
    if (i < 0 || i >= res->n) return NULL;
    return res->arestas[i].origem;
}

const char *mst_aresta_destino(MstResultado r, int i) {
    if (!r) return NULL;
    MstResultadoInterno *res = r;
    if (i < 0 || i >= res->n) return NULL;
    return res->arestas[i].destino;
}

double mst_aresta_cmp(MstResultado r, int i) {
    if (!r) return 0.0;
    MstResultadoInterno *res = r;
    if (i < 0 || i >= res->n) return 0.0;
    return res->arestas[i].cmp;
}
