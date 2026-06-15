#include "grafo.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- structs internas (NUNCA em .h) ---- */

typedef struct Aresta_s {
    char *destino_id;
    char *nome;
    char *ldir;
    char *lesq;
    double cmp;
    double vm;
    struct Aresta_s *next;
} Aresta_s;

typedef struct {
    char *id;
    double x;
    double y;
    Aresta_s *adj; /* cabeça da lista de adjacência */
} Vertice_s;

typedef struct {
    List vertices;
    int num_vertices;
    int num_arestas;
} GrafoImpl;

/* ---- auxiliares ---- */

static char *xstrdup(const char *s) {
    if (!s) return NULL;
    char *copy = malloc(strlen(s) + 1);
    if (copy) strcpy(copy, s);
    return copy;
}

static Vertice_s *buscar_vertice_interno(GrafoImpl *g, const char *id) {
    int n = list_size(g->vertices);
    for (int i = 0; i < n; i++) {
        Vertice_s *v = list_get(g->vertices, i);
        if (strcmp(v->id, id) == 0) return v;
    }
    return NULL;
}

static void aresta_destruir(Aresta_s *a) {
    while (a) {
        Aresta_s *prox = a->next;
        free(a->destino_id);
        free(a->nome);
        free(a->ldir);
        free(a->lesq);
        free(a);
        a = prox;
    }
}

/* ---- API pública ---- */

Grafo grafo_criar(void) {
    GrafoImpl *g = malloc(sizeof(GrafoImpl));
    if (!g) return NULL;
    g->vertices = list_create();
    if (!g->vertices) { free(g); return NULL; }
    g->num_vertices = 0;
    g->num_arestas = 0;
    return (Grafo)g;
}

void grafo_destruir(Grafo grafo) {
    if (!grafo) return;
    GrafoImpl *g = (GrafoImpl *)grafo;
    int n = list_size(g->vertices);
    for (int i = 0; i < n; i++) {
        Vertice_s *v = list_get(g->vertices, i);
        aresta_destruir(v->adj);
        free(v->id);
        free(v);
    }
    list_destroy(g->vertices);
    free(g);
}

bool grafo_inserir_vertice(Grafo grafo, const char *id, double x, double y) {
    if (!grafo || !id) return false;
    GrafoImpl *g = (GrafoImpl *)grafo;
    if (buscar_vertice_interno(g, id)) return false; /* duplicata */

    Vertice_s *v = malloc(sizeof(Vertice_s));
    if (!v) return false;
    v->id = xstrdup(id);
    v->x = x;
    v->y = y;
    v->adj = NULL;
    if (!list_insert_back(g->vertices, v)) { free(v->id); free(v); return false; }
    g->num_vertices++;
    return true;
}

bool grafo_inserir_aresta(Grafo grafo, const char *i, const char *j,
                          const char *ldir, const char *lesq,
                          double cmp, double vm, const char *nome) {
    if (!grafo || !i || !j) return false;
    GrafoImpl *g = (GrafoImpl *)grafo;
    Vertice_s *orig = buscar_vertice_interno(g, i);
    Vertice_s *dest = buscar_vertice_interno(g, j);
    if (!orig || !dest) return false;

    Aresta_s *a = malloc(sizeof(Aresta_s));
    if (!a) return false;
    a->destino_id = xstrdup(j);
    a->nome = xstrdup(nome);
    a->ldir = xstrdup(ldir);
    a->lesq = xstrdup(lesq);
    a->cmp = cmp;
    a->vm = vm;
    a->next = orig->adj;
    orig->adj = a;
    g->num_arestas++;
    return true;
}

void *grafo_buscar_vertice(Grafo grafo, const char *id) {
    if (!grafo || !id) return NULL;
    return buscar_vertice_interno((GrafoImpl *)grafo, id);
}

int grafo_num_vertices(Grafo grafo) {
    if (!grafo) return 0;
    return ((GrafoImpl *)grafo)->num_vertices;
}

int grafo_num_arestas(Grafo grafo) {
    if (!grafo) return 0;
    return ((GrafoImpl *)grafo)->num_arestas;
}

void *grafo_adjacentes_inicio(Grafo grafo, const char *id) {
    if (!grafo || !id) return NULL;
    GrafoImpl *g = (GrafoImpl *)grafo;
    Vertice_s *v = buscar_vertice_interno(g, id);
    if (!v) return NULL;
    return v->adj;
}

void *grafo_adjacentes_fim(Grafo grafo, void *cursor) {
    (void)grafo;
    if (!cursor) return NULL;
    return ((Aresta_s *)cursor)->next;
}

/* ---- getters de vértice ---- */

const char *grafo_vertice_id(void *v) {
    if (!v) return NULL;
    return ((Vertice_s *)v)->id;
}

double grafo_vertice_x(void *v) {
    if (!v) return 0.0;
    return ((Vertice_s *)v)->x;
}

double grafo_vertice_y(void *v) {
    if (!v) return 0.0;
    return ((Vertice_s *)v)->y;
}

/* ---- getters de aresta ---- */

const char *grafo_aresta_destino(void *cursor) {
    if (!cursor) return NULL;
    return ((Aresta_s *)cursor)->destino_id;
}

const char *grafo_aresta_nome(void *cursor) {
    if (!cursor) return NULL;
    return ((Aresta_s *)cursor)->nome;
}

const char *grafo_aresta_ldir(void *cursor) {
    if (!cursor) return NULL;
    return ((Aresta_s *)cursor)->ldir;
}

const char *grafo_aresta_lesq(void *cursor) {
    if (!cursor) return NULL;
    return ((Aresta_s *)cursor)->lesq;
}

double grafo_aresta_cmp(void *cursor) {
    if (!cursor) return 0.0;
    return ((Aresta_s *)cursor)->cmp;
}

double grafo_aresta_vm(void *cursor) {
    if (!cursor) return 0.0;
    return ((Aresta_s *)cursor)->vm;
}

void grafo_aresta_set_vm(void *cursor, double vm) {
    if (!cursor) return;
    ((Aresta_s *)cursor)->vm = vm;
}

void *grafo_vertice_por_indice(Grafo grafo, int i) {
    if (!grafo || i < 0) return NULL;
    return list_get(((GrafoImpl *)grafo)->vertices, i);
}
