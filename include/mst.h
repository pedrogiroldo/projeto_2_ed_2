/**
 * @file mst.h
 * @brief Árvore Geradora Mínima (AGM) por peso `cmp` — tipo opaco
 *
 * O dígrafo é tratado como não-dirigido: para cada aresta i→j considera-se
 * também j→i com o mesmo peso `cmp`. O resultado é a floresta geradora mínima
 * (uma AGM por componente conexa), obtida via Kruskal com union-find.
 */

#ifndef MST_H
#define MST_H

#include "grafo.h"

typedef void *MstResultado;

/**
 * @brief Calcula a AGM (floresta geradora mínima) do grafo pelo peso `cmp`.
 *
 * Trata o grafo como não-dirigido. Arestas paralelas/reversas são
 * consolidadas: entre dois vértices considera-se o menor `cmp`.
 *
 * @param g Grafo
 * @return Resultado opaco (caller deve chamar mst_resultado_destruir),
 *         ou NULL em erro ou grafo vazio.
 */
MstResultado mst_calcular(Grafo g);

/**
 * @brief Libera o resultado da AGM (aceita NULL).
 */
void mst_resultado_destruir(MstResultado res);

/**
 * @brief Número de arestas na AGM (n_vertices - n_componentes).
 */
int mst_num_arestas(MstResultado res);

/**
 * @brief Soma dos pesos `cmp` das arestas da AGM.
 */
double mst_peso_total(MstResultado res);

/**
 * @brief ID do vértice origem da i-ésima aresta da AGM (0-based).
 * @return ID (string interna, não liberar) ou NULL se i fora dos limites.
 */
const char *mst_aresta_origem(MstResultado res, int i);

/**
 * @brief ID do vértice destino da i-ésima aresta da AGM (0-based).
 * @return ID (string interna, não liberar) ou NULL se i fora dos limites.
 */
const char *mst_aresta_destino(MstResultado res, int i);

/**
 * @brief Peso `cmp` (comprimento) da i-ésima aresta da AGM.
 * @return cmp, ou 0.0 se i fora dos limites.
 */
double mst_aresta_cmp(MstResultado res, int i);

#endif /* MST_H */
