/**
 * @file dijkstra.h
 * @brief Dijkstra com peso genérico via ponteiro de função (tipo opaco)
 */

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stdbool.h>
#include "grafo.h"

typedef void *DijkstraResultado;

/**
 * @brief Executa Dijkstra a partir de id_origem
 * @param g        Grafo direcionado
 * @param id_origem ID do vértice de origem
 * @param peso_fn  Função de peso: recebe cursor de aresta, retorna double >= 0
 *                 Ex.: distância → grafo_aresta_cmp(a)
 *                      tempo    → grafo_aresta_cmp(a) / grafo_aresta_vm(a)
 * @return Resultado opaco ou NULL em erro
 */
DijkstraResultado dijkstra_executar(Grafo g, const char *id_origem,
                                    double (*peso_fn)(void *aresta));

/**
 * @brief Libera o resultado do Dijkstra
 * @param res Resultado a liberar (aceita NULL)
 */
void dijkstra_resultado_destruir(DijkstraResultado res);

/**
 * @brief Retorna a distância mínima até id_destino
 * @return Distância, ou INFINITY se inacessível/não encontrado
 */
double dijkstra_distancia(DijkstraResultado res, const char *id_destino);

/**
 * @brief Verifica se id_destino é alcançável a partir da origem
 */
bool dijkstra_alcancavel(DijkstraResultado res, const char *id_destino);

/**
 * @brief Reconstrói o caminho mínimo, chamando cb para cada aresta (origem→destino)
 * @param res       Resultado do Dijkstra
 * @param g         Grafo usado na execução
 * @param id_destino Vértice destino
 * @param cb        Callback chamado para cada aresta do caminho em ordem
 * @param ctx       Contexto opaco passado ao callback
 *
 * Se id_destino for inacessível, cb nunca é chamada.
 * Os cursores passados ao cb são válidos enquanto g não for modificado.
 */
void dijkstra_caminho_por_arestas(DijkstraResultado res, Grafo g,
                                  const char *id_destino,
                                  void (*cb)(void *cursor_aresta, void *ctx),
                                  void *ctx);

#endif /* DIJKSTRA_H */
