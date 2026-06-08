/**
 * @file via_handler.h
 * @brief Parser de arquivos .via — constrói o grafo viário de Bitnópolis.
 *
 * Formato do arquivo .via:
 * @code
 *   nv                          <- número de vértices
 *   v id x y                   <- vértice com id (string) e coordenadas
 *   e i j ldir lesq cmp vm nome <- aresta direcionada de i para j
 * @endcode
 */

#ifndef VIA_HANDLER_H
#define VIA_HANDLER_H

#include "grafo.h"
#include "extensible_hash_file.h"

/**
 * @brief Lê um arquivo .via e popula o grafo com vértices e arestas.
 *
 * Insere cada linha @c v como vértice e cada linha @c e como aresta
 * direcionada. Os campos @c ldir e @c lesq são CEPs que identificam quadras
 * no hashfile (ou "-" quando não há quadra adjacente).
 *
 * @param via_filepath     Caminho para o arquivo .via (não-NULL).
 * @param grafo            Grafo destino já criado (não-NULL).
 * @param hashfile_quadras Hashfile de quadras para associar ldir/lesq,
 *                         ou NULL se não houver associação.
 * @return true se o arquivo foi lido sem erros fatais, false caso contrário.
 */
bool via_carregar(const char *via_filepath,
                  Grafo grafo,
                  extensible_hash_file_t hashfile_quadras);

#endif /* VIA_HANDLER_H */
