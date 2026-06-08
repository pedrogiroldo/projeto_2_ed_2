/**
 * @file heap.h
 * @brief Min-heap binário com suporte a diminuir_chave — base do Dijkstra.
 *
 * Cada elemento é identificado por uma string @c id e possui uma chave double.
 * A operação @c heap_diminuir_chave permite atualizar a prioridade em O(log n),
 * essencial para o Dijkstra eficiente.
 */

#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>

typedef void *Heap;

/**
 * @brief Cria um heap vazio com capacidade inicial
 * @param capacidade_inicial Número estimado de elementos (> 0)
 * @return Novo heap ou NULL em falha
 */
Heap heap_criar(int capacidade_inicial);

/**
 * @brief Destroi o heap e libera toda a memória
 * @param h Heap a destruir (aceita NULL)
 */
void heap_destruir(Heap h);

/**
 * @brief Insere um elemento com a chave dada
 * @param h     Heap
 * @param id    Identificador do elemento (string, copiada internamente)
 * @param chave Prioridade (menor = maior prioridade)
 * @return true se inserido, false se id duplicado ou erro
 */
bool heap_inserir(Heap h, const char *id, double chave);

/**
 * @brief Extrai o elemento de menor chave
 * @param h         Heap
 * @param id_out    Buffer onde o id do mínimo é copiado (não-NULL)
 * @param chave_out Ponteiro onde a chave do mínimo é escrita (não-NULL)
 * @return true se extraído, false se heap vazio ou erro
 */
bool heap_extrair_min(Heap h, char *id_out, double *chave_out);

/**
 * @brief Diminui a chave de um elemento existente
 * @param h          Heap
 * @param id         Identificador do elemento a atualizar
 * @param nova_chave Nova chave (deve ser menor que a atual)
 * @return true se atualizado, false se id não encontrado ou chave maior
 */
bool heap_diminuir_chave(Heap h, const char *id, double nova_chave);

/**
 * @brief Verifica se o heap está vazio
 * @return true se vazio ou NULL
 */
bool heap_vazio(Heap h);

/**
 * @brief Retorna o número de elementos no heap
 */
int heap_tamanho(Heap h);

#endif /* HEAP_H */
