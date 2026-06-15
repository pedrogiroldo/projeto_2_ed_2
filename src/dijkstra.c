#include "dijkstra.h"
#include "heap.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* ---- structs internas (NUNCA em .h) ---- */

#define ID_MAX 256

typedef struct Entrada_s {
    char id[ID_MAX];
    double dist;
    void *prev_aresta;      /* cursor de adjacência da aresta que chegou aqui */
    char prev_id[ID_MAX];   /* id do vértice que enviou a aresta acima */
    int no_heap;            /* 1 enquanto o id ainda está no heap */
    struct Entrada_s *prox;
} Entrada_s;

typedef struct {
    Entrada_s *lista;
    int n;
} DijkstraImpl;

/* ---- helpers internos ---- */

static Entrada_s *buscar_entrada(DijkstraImpl *impl, const char *id) {
    for (Entrada_s *e = impl->lista; e; e = e->prox)
        if (strcmp(e->id, id) == 0) return e;
    return NULL;
}

static Entrada_s *criar_entrada(DijkstraImpl *impl, const char *id) {
    Entrada_s *e = malloc(sizeof(Entrada_s));
    if (!e) return NULL;
    strncpy(e->id, id, ID_MAX - 1);
    e->id[ID_MAX - 1] = '\0';
    e->dist = INFINITY;
    e->prev_aresta = NULL;
    e->prev_id[0] = '\0';
    e->no_heap = 0;
    e->prox = impl->lista;
    impl->lista = e;
    impl->n++;
    return e;
}

/* ---- API pública ---- */

DijkstraResultado dijkstra_executar(Grafo g, const char *id_origem,
                                    double (*peso_fn)(void *aresta)) {
    if (!g || !id_origem || !peso_fn) return NULL;

    DijkstraImpl *impl = malloc(sizeof(DijkstraImpl));
    if (!impl) return NULL;
    impl->lista = NULL;
    impl->n = 0;

    int cap = grafo_num_vertices(g);
    if (cap < 1) cap = 1;

    Heap h = heap_criar(cap);
    if (!h) { free(impl); return NULL; }

    Entrada_s *orig = criar_entrada(impl, id_origem);
    if (!orig) { heap_destruir(h); free(impl); return NULL; }
    orig->dist = 0.0;
    orig->no_heap = 1;
    heap_inserir(h, id_origem, 0.0);

    char u_id[ID_MAX];
    double u_dist;

    while (!heap_vazio(h)) {
        if (!heap_extrair_min(h, u_id, &u_dist)) break;

        Entrada_s *eu = buscar_entrada(impl, u_id);
        if (eu) eu->no_heap = 0;

        void *cur = grafo_adjacentes_inicio(g, u_id);
        while (cur) {
            const char *v_id = grafo_aresta_destino(cur);
            double peso = peso_fn(cur);
            double nova = u_dist + peso;

            Entrada_s *ev = buscar_entrada(impl, v_id);
            if (!ev || nova < ev->dist) {
                if (!ev) {
                    ev = criar_entrada(impl, v_id);
                    if (!ev) { cur = grafo_adjacentes_fim(g, cur); continue; }
                }
                ev->dist = nova;
                ev->prev_aresta = cur;
                strncpy(ev->prev_id, u_id, ID_MAX - 1);
                ev->prev_id[ID_MAX - 1] = '\0';

                if (ev->no_heap) {
                    heap_diminuir_chave(h, v_id, nova);
                } else {
                    ev->no_heap = 1;
                    heap_inserir(h, v_id, nova);
                }
            }
            cur = grafo_adjacentes_fim(g, cur);
        }
    }

    heap_destruir(h);
    return impl;
}

void dijkstra_resultado_destruir(DijkstraResultado res) {
    if (!res) return;
    DijkstraImpl *impl = res;
    Entrada_s *e = impl->lista;
    while (e) {
        Entrada_s *prox = e->prox;
        free(e);
        e = prox;
    }
    free(impl);
}

double dijkstra_distancia(DijkstraResultado res, const char *id_destino) {
    if (!res || !id_destino) return INFINITY;
    DijkstraImpl *impl = res;
    Entrada_s *e = buscar_entrada(impl, id_destino);
    return e ? e->dist : INFINITY;
}

bool dijkstra_alcancavel(DijkstraResultado res, const char *id_destino) {
    if (!res || !id_destino) return false;
    DijkstraImpl *impl = res;
    Entrada_s *e = buscar_entrada(impl, id_destino);
    return e && !isinf(e->dist);
}

void dijkstra_caminho_por_arestas(DijkstraResultado res, Grafo g,
                                  const char *id_destino,
                                  void (*cb)(void *cursor_aresta, void *ctx),
                                  void *ctx) {
    if (!res || !g || !id_destino || !cb) return;
    DijkstraImpl *impl = res;

    /* coletar arestas do caminho em ordem reversa (destino → origem) */
    void **buf = NULL;
    int n = 0, cap = 0;

    const char *cur_id = id_destino;
    while (1) {
        Entrada_s *e = buscar_entrada(impl, cur_id);
        if (!e || !e->prev_aresta) break;

        if (n >= cap) {
            cap = cap ? cap * 2 : 8;
            void **novo = realloc(buf, (size_t)cap * sizeof(void *));
            if (!novo) { free(buf); return; }
            buf = novo;
        }
        buf[n++] = e->prev_aresta;
        cur_id = e->prev_id;
    }

    /* chamar cb em ordem (origem → destino) */
    for (int i = n - 1; i >= 0; i--)
        cb(buf[i], ctx);

    free(buf);
}
