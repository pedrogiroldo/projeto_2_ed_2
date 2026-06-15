/**
 * @file grafo.h
 * @brief Grafo direcionado com listas de adjacência (tipo opaco)
 */

#ifndef GRAFO_H
#define GRAFO_H

#include <stdbool.h>

typedef void *Grafo;

/**
 * @brief Cria um grafo vazio
 * @return Novo grafo ou NULL em falha
 */
Grafo grafo_criar(void);

/**
 * @brief Destroi o grafo e libera toda a memória
 * @param g Grafo a destruir
 */
void grafo_destruir(Grafo g);

/**
 * @brief Insere um vértice no grafo
 * @param g    Grafo
 * @param id   Identificador do vértice (string, copiada internamente)
 * @param x    Coordenada x
 * @param y    Coordenada y
 * @return true se inserido, false se duplicado ou erro
 */
bool grafo_inserir_vertice(Grafo g, const char *id, double x, double y);

/**
 * @brief Insere uma aresta direcionada de i para j
 * @param g    Grafo
 * @param i    ID do vértice origem
 * @param j    ID do vértice destino
 * @param ldir CEP da quadra à direita (ou "-")
 * @param lesq CEP da quadra à esquerda (ou "-")
 * @param cmp  Comprimento em metros
 * @param vm   Velocidade média em m/s
 * @param nome Nome da rua
 * @return true se inserida, false se vértice não existe ou erro
 */
bool grafo_inserir_aresta(Grafo g, const char *i, const char *j,
                          const char *ldir, const char *lesq,
                          double cmp, double vm, const char *nome);

/**
 * @brief Busca um vértice pelo id; retorna cursor opaco para uso com getters
 * @param g  Grafo
 * @param id ID a buscar
 * @return Ponteiro opaco para o vértice ou NULL se não encontrado
 */
void *grafo_buscar_vertice(Grafo g, const char *id);

/**
 * @brief Retorna o número de vértices no grafo
 */
int grafo_num_vertices(Grafo g);

/**
 * @brief Retorna o número de arestas no grafo
 */
int grafo_num_arestas(Grafo g);

/**
 * @brief Inicia iteração sobre as arestas adjacentes a um vértice
 * @param g  Grafo
 * @param id ID do vértice
 * @return Cursor opaco para a primeira aresta adjacente, ou NULL se nenhuma
 */
void *grafo_adjacentes_inicio(Grafo g, const char *id);

/**
 * @brief Avança o cursor de adjacência para a próxima aresta
 * @param g      Grafo
 * @param cursor Cursor atual (retornado por grafo_adjacentes_inicio ou chamada anterior)
 * @return Cursor para a próxima aresta, ou NULL se não há mais
 */
void *grafo_adjacentes_fim(Grafo g, void *cursor);

/* ---- Getters para vértice opaco ---- */

/** Retorna o ID do vértice apontado pelo cursor */
const char *grafo_vertice_id(void *v);

/** Retorna a coordenada x do vértice */
double grafo_vertice_x(void *v);

/** Retorna a coordenada y do vértice */
double grafo_vertice_y(void *v);

/* ---- Getters para aresta opaca (cursor de adjacência) ---- */

/** Retorna o ID do vértice destino da aresta apontada pelo cursor */
const char *grafo_aresta_destino(void *cursor);

/** Retorna o nome da rua da aresta */
const char *grafo_aresta_nome(void *cursor);

/** Retorna o CEP da quadra à direita */
const char *grafo_aresta_ldir(void *cursor);

/** Retorna o CEP da quadra à esquerda */
const char *grafo_aresta_lesq(void *cursor);

/** Retorna o comprimento em metros */
double grafo_aresta_cmp(void *cursor);

/** Retorna a velocidade média em m/s */
double grafo_aresta_vm(void *cursor);

/** Atualiza a velocidade média de uma aresta (usada por mvm e exp) */
void grafo_aresta_set_vm(void *cursor, double vm);

/** Retorna o i-ésimo vértice (opaco), ou NULL se i fora dos limites */
void *grafo_vertice_por_indice(Grafo g, int i);

#endif /* GRAFO_H */
