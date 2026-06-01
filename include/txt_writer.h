/**
 * @file txt_writer.h
 * @brief Gravador incremental de arquivo de texto para respostas de consultas.
 *
 * Crie com txt_writer_criar(), adicione linhas com txt_writer_linha() e
 * libere com txt_writer_destruir() (que fecha o arquivo automaticamente).
 */

#ifndef TXT_WRITER_H
#define TXT_WRITER_H

/** Tipo opaco para o writer de texto. */
typedef struct txt_writer txt_writer_t;

/**
 * @brief Cria um novo writer e abre @p output_path para escrita.
 *
 * @param output_path Caminho do arquivo de saída (não-NULL).
 * @return Nova instância ou NULL se @p output_path for NULL ou o arquivo
 *         não puder ser aberto.
 */
txt_writer_t *txt_writer_criar(const char *output_path);

/**
 * @brief Fecha o arquivo e libera a memória. Aceita NULL com segurança.
 * @param tw Writer a destruir.
 */
void txt_writer_destruir(txt_writer_t *tw);

/**
 * @brief Escreve uma linha formatada seguida de newline.
 *
 * Aceita os mesmos especificadores de formato que @c printf.
 * Chamada segura mesmo se @p tw for NULL (ignorada silenciosamente).
 *
 * @param tw  Writer de destino (pode ser NULL).
 * @param fmt String de formato (estilo printf).
 * @param ... Argumentos do formato.
 */
void txt_writer_linha(txt_writer_t *tw, const char *fmt, ...);

#endif // TXT_WRITER_H
