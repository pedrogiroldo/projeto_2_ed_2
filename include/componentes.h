/**
 * @file componentes.h
 * @brief Cálculo de componentes conexas em grafo direcionado tratado como não-dirigido
 */

#ifndef COMPONENTES_H
#define COMPONENTES_H

#include "grafo.h"

/**
 * @brief Calcula componentes conexas tratando o grafo como não-dirigido.
 *        Ignora arestas com vm < vm_minima.
 *
 * @param g            Grafo
 * @param vm_minima    Limiar de velocidade mínima
 * @param num_comp_out [out] Número de componentes encontradas
 * @return Array int[grafo_num_vertices(g)] onde result[i] = id (0-based) da componente
 *         do i-ésimo vértice (via grafo_vertice_por_indice). Caller deve free().
 *         NULL em erro ou grafo vazio.
 */
int *componentes_calcular(Grafo g, double vm_minima, int *num_comp_out);

#endif /* COMPONENTES_H */
