/**
 * @file geo_handler.h
 * @brief Parser de arquivos .geo de Bitnópolis.
 *
 * Processa dois comandos:
 * - @c cq  @c sw @c cfill @c cstrk — define estilo global (espessura,
 *   preenchimento, borda) para as quadras seguintes.
 * - @c q   @c cep @c x @c y @c w @c h — insere quadra no hashfile e a
 *   desenha no SVG.
 *
 * Uso típico:
 * @code
 *   extensible_hash_file_t hf = ehf_create(path, cap, sizeof(quadra_registro_t));
 *   svg_writer_t *svg = svg_writer_criar(svg_path, 0, 0);
 *   geo_handler_resultado_t r = geo_handler_processar(geo_path, hf, svg);
 *   printf("%d inseridas, %d erros\n",
 *          geo_handler_resultado_inseridas(r),
 *          geo_handler_resultado_erros(r));
 *   geo_handler_resultado_destruir(r);
 *   svg_writer_finalizar(svg);
 *   svg_writer_destruir(svg);
 *   ehf_close(hf);
 * @endcode
 */

#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H

#include "extensible_hash_file.h"
#include "svg_writer.h"

/** Tipo opaco para o resultado do processamento de um arquivo .geo. */
typedef void *geo_handler_resultado_t;

/**
 * @brief Lê e processa um arquivo .geo, inserindo quadras no hashfile.
 *
 * Para cada comando @c q válido, insere um @c quadra_registro_t no hashfile e,
 * se @p svg não for NULL, desenha o retângulo correspondente.
 *
 * @param geo_filepath Caminho para o arquivo .geo (não-NULL).
 * @param quadras_hf   Hashfile aberto com record_size = sizeof(quadra_registro_t).
 * @param svg          Writer SVG de destino, ou NULL para omitir o desenho.
 * @return Handle opaco com contadores; liberar com geo_handler_resultado_destruir.
 */
geo_handler_resultado_t geo_handler_processar(
    const char *geo_filepath,
    extensible_hash_file_t quadras_hf,
    svg_writer_t *svg);

/** @brief Retorna o número de quadras inseridas com sucesso. */
int geo_handler_resultado_inseridas(geo_handler_resultado_t resultado);

/** @brief Retorna o número de linhas malformadas ou CEPs duplicados. */
int geo_handler_resultado_erros(geo_handler_resultado_t resultado);

/** @brief Libera o resultado retornado por geo_handler_processar. */
void geo_handler_resultado_destruir(geo_handler_resultado_t resultado);

#endif /* GEO_HANDLER_H */
