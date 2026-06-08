#include "heap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- structs internas (NUNCA em .h) ---- */

#define ID_MAX 64

typedef struct {
    char id[ID_MAX];
    double chave;
} No;

typedef struct {
    /* mapa id→posição: tabela hash aberta (linear probing) */
    char  **mapa_ids;
    int    *mapa_pos;
    int     mapa_cap;

    No     *array;   /* heap binário indexado por 0 */
    int     tamanho;
    int     capacidade;
} HeapImpl;

/* ---- hash simples para o mapa ---- */

static unsigned int hash_str(const char *s, int cap) {
    unsigned int h = 5381;
    while (*s) h = h * 33 ^ (unsigned char)*s++;
    return h % (unsigned int)cap;
}

static int mapa_buscar(HeapImpl *h, const char *id) {
    unsigned int idx = hash_str(id, h->mapa_cap);
    for (int i = 0; i < h->mapa_cap; i++) {
        int pos = (idx + i) % h->mapa_cap;
        if (!h->mapa_ids[pos]) return -1;
        if (strcmp(h->mapa_ids[pos], id) == 0) return pos;
    }
    return -1;
}

static void mapa_inserir(HeapImpl *h, const char *id, int heap_pos) {
    unsigned int idx = hash_str(id, h->mapa_cap);
    for (int i = 0; i < h->mapa_cap; i++) {
        int pos = (idx + i) % h->mapa_cap;
        if (!h->mapa_ids[pos]) {
            h->mapa_ids[pos] = malloc(strlen(id) + 1);
            if (h->mapa_ids[pos]) strcpy(h->mapa_ids[pos], id);
            h->mapa_pos[pos] = heap_pos;
            return;
        }
    }
}

static void mapa_atualizar_pos(HeapImpl *h, const char *id, int heap_pos) {
    int slot = mapa_buscar(h, id);
    if (slot >= 0) h->mapa_pos[slot] = heap_pos;
}

static int mapa_obter_pos(HeapImpl *h, const char *id) {
    int slot = mapa_buscar(h, id);
    return slot >= 0 ? h->mapa_pos[slot] : -1;
}

/* ---- operações do heap ---- */

static void trocar(HeapImpl *h, int a, int b) {
    No tmp = h->array[a];
    h->array[a] = h->array[b];
    h->array[b] = tmp;
    mapa_atualizar_pos(h, h->array[a].id, a);
    mapa_atualizar_pos(h, h->array[b].id, b);
}

static void sift_up(HeapImpl *h, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (h->array[pai].chave <= h->array[i].chave) break;
        trocar(h, i, pai);
        i = pai;
    }
}

static void sift_down(HeapImpl *h, int i) {
    int n = h->tamanho;
    while (1) {
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;
        int menor = i;
        if (esq < n && h->array[esq].chave < h->array[menor].chave) menor = esq;
        if (dir < n && h->array[dir].chave < h->array[menor].chave) menor = dir;
        if (menor == i) break;
        trocar(h, i, menor);
        i = menor;
    }
}

/* ---- API pública ---- */

Heap heap_criar(int capacidade_inicial) {
    if (capacidade_inicial <= 0) capacidade_inicial = 8;
    HeapImpl *h = malloc(sizeof(HeapImpl));
    if (!h) return NULL;

    h->array = malloc(capacidade_inicial * sizeof(No));
    if (!h->array) { free(h); return NULL; }

    /* mapa com capacidade 2x para baixa colisão */
    h->mapa_cap = capacidade_inicial * 2;
    h->mapa_ids = calloc(h->mapa_cap, sizeof(char *));
    h->mapa_pos = malloc(h->mapa_cap * sizeof(int));
    if (!h->mapa_ids || !h->mapa_pos) {
        free(h->array); free(h->mapa_ids); free(h->mapa_pos); free(h);
        return NULL;
    }

    h->tamanho = 0;
    h->capacidade = capacidade_inicial;
    return (Heap)h;
}

void heap_destruir(Heap heap) {
    if (!heap) return;
    HeapImpl *h = (HeapImpl *)heap;
    for (int i = 0; i < h->mapa_cap; i++) free(h->mapa_ids[i]);
    free(h->mapa_ids);
    free(h->mapa_pos);
    free(h->array);
    free(h);
}

bool heap_inserir(Heap heap, const char *id, double chave) {
    if (!heap || !id) return false;
    HeapImpl *h = (HeapImpl *)heap;

    if (mapa_obter_pos(h, id) >= 0) return false; /* duplicata */

    /* crescer array se necessário */
    if (h->tamanho == h->capacidade) {
        int nova_cap = h->capacidade * 2;
        No *novo = realloc(h->array, nova_cap * sizeof(No));
        if (!novo) return false;
        h->array = novo;

        /* recriar mapa com capacidade maior */
        int novo_mapa_cap = nova_cap * 2;
        char **novo_ids  = calloc(novo_mapa_cap, sizeof(char *));
        int  *novo_pos   = malloc(novo_mapa_cap * sizeof(int));
        if (!novo_ids || !novo_pos) { free(novo_ids); free(novo_pos); return false; }

        for (int i = 0; i < h->mapa_cap; i++) {
            if (h->mapa_ids[i]) {
                unsigned int idx = hash_str(h->mapa_ids[i], novo_mapa_cap);
                for (int k = 0; k < novo_mapa_cap; k++) {
                    int slot = (idx + k) % novo_mapa_cap;
                    if (!novo_ids[slot]) {
                        novo_ids[slot] = h->mapa_ids[i];
                        novo_pos[slot] = h->mapa_pos[i];
                        break;
                    }
                }
            }
        }
        free(h->mapa_ids);
        free(h->mapa_pos);
        h->mapa_ids  = novo_ids;
        h->mapa_pos  = novo_pos;
        h->mapa_cap  = novo_mapa_cap;
        h->capacidade = nova_cap;
    }

    int i = h->tamanho++;
    strncpy(h->array[i].id, id, ID_MAX - 1);
    h->array[i].id[ID_MAX - 1] = '\0';
    h->array[i].chave = chave;
    mapa_inserir(h, id, i);
    sift_up(h, i);
    return true;
}

bool heap_extrair_min(Heap heap, char *id_out, double *chave_out) {
    if (!heap || !id_out || !chave_out) return false;
    HeapImpl *h = (HeapImpl *)heap;
    if (h->tamanho == 0) return false;

    strncpy(id_out, h->array[0].id, ID_MAX - 1);
    id_out[ID_MAX - 1] = '\0';
    *chave_out = h->array[0].chave;

    /* remover do mapa */
    int slot = mapa_buscar(h, h->array[0].id);
    if (slot >= 0) { free(h->mapa_ids[slot]); h->mapa_ids[slot] = NULL; }

    h->tamanho--;
    if (h->tamanho > 0) {
        h->array[0] = h->array[h->tamanho];
        mapa_atualizar_pos(h, h->array[0].id, 0);
        sift_down(h, 0);
    }
    return true;
}

bool heap_diminuir_chave(Heap heap, const char *id, double nova_chave) {
    if (!heap || !id) return false;
    HeapImpl *h = (HeapImpl *)heap;
    int pos = mapa_obter_pos(h, id);
    if (pos < 0 || nova_chave > h->array[pos].chave) return false;
    h->array[pos].chave = nova_chave;
    sift_up(h, pos);
    return true;
}

bool heap_vazio(Heap heap) {
    if (!heap) return true;
    return ((HeapImpl *)heap)->tamanho == 0;
}

int heap_tamanho(Heap heap) {
    if (!heap) return 0;
    return ((HeapImpl *)heap)->tamanho;
}
