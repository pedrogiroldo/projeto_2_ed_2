/**
 * @file extensible_hash_file.h
 * @brief Índice hash extensível persistido em arquivo.
 *
 * Use ehf_create() para criar um novo arquivo de índice ou ehf_open() para
 * reabrir um existente. Armazene registros de tamanho fixo com ehf_insert(),
 * recupere-os com ehf_find(), remova entradas com ehf_remove() e finalize
 * sempre com ehf_close().
 */

#ifndef EXTENSIBLE_HASH_FILE_H
#define EXTENSIBLE_HASH_FILE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/** Handle opaco para um índice hash extensível persistido em arquivo. */
typedef void *extensible_hash_file_t;

/**
 * @brief Códigos de resultado retornados pelas operações do índice.
 */
typedef enum {
    EHF_OK = 0,           /**< Operação concluída com sucesso. */
    EHF_NOT_FOUND,        /**< Chave não encontrada no índice. */
    EHF_DUPLICATE_KEY,    /**< Chave já existe no índice. */
    EHF_INVALID_ARGUMENT, /**< Argumento NULL ou inválido. */
    EHF_IO_ERROR,         /**< Falha de leitura ou escrita no arquivo. */
    EHF_CORRUPTED_FILE    /**< Conteúdo do arquivo é inválido ou corrompido. */
} ehf_status_t;

/**
 * @brief Cria um novo arquivo de índice hash em @p index_path.
 *
 * @param index_path       Caminho do arquivo a criar (não-NULL).
 * @param bucket_capacity  Número máximo de entradas por bucket antes de dividir.
 * @param record_size      Tamanho em bytes de cada registro armazenado.
 * @return Handle válido ou NULL se os argumentos forem inválidos ou ocorrer
 *         falha de alocação/escrita.
 */
extensible_hash_file_t ehf_create(const char *index_path, uint32_t bucket_capacity,
                                  size_t record_size);

/**
 * @brief Abre um arquivo de índice existente e valida seu cabeçalho.
 *
 * @param index_path Caminho do arquivo (não-NULL, deve existir).
 * @return Handle válido ou NULL se @p index_path for inválido, o arquivo não
 *         existir, ou seu conteúdo não corresponder ao formato esperado.
 */
extensible_hash_file_t ehf_open(const char *index_path);

/**
 * @brief Fecha o índice e libera seus recursos. Aceita NULL com segurança.
 * @param hash Handle a fechar.
 */
void ehf_close(extensible_hash_file_t hash);

/**
 * @brief Insere um registro associado a uma chave alfanumérica de 1 a 32 caracteres.
 *
 * @param hash        Handle do índice (não-NULL).
 * @param key         Chave alfanumérica (1–32 caracteres, não-NULL).
 * @param record      Buffer com os bytes do registro (não-NULL).
 * @param record_size Tamanho do registro em bytes.
 * @return EHF_OK em sucesso; EHF_DUPLICATE_KEY se a chave já existir;
 *         EHF_INVALID_ARGUMENT para argumentos inválidos; EHF_IO_ERROR em
 *         falha de escrita.
 */
ehf_status_t ehf_insert(extensible_hash_file_t hash, const char *key,
                        const void *record, size_t record_size);

/**
 * @brief Localiza uma chave e copia o registro associado para @p out_record.
 *
 * @param hash        Handle do índice (não-NULL).
 * @param key         Chave alfanumérica a buscar (1–32 caracteres, não-NULL).
 * @param out_record  Buffer de destino para o registro encontrado (não-NULL).
 * @param record_size Tamanho esperado do registro.
 * @return EHF_OK em sucesso; EHF_NOT_FOUND se a chave não existir;
 *         EHF_INVALID_ARGUMENT para argumentos inválidos.
 */
ehf_status_t ehf_find(extensible_hash_file_t hash, const char *key,
                      void *out_record, size_t record_size);

/**
 * @brief Remove uma chave do índice.
 *
 * @param hash Handle do índice (não-NULL).
 * @param key  Chave alfanumérica a remover (1–32 caracteres, não-NULL).
 * @return EHF_OK em sucesso; EHF_NOT_FOUND se a chave não existir;
 *         EHF_INVALID_ARGUMENT para argumentos inválidos.
 */
ehf_status_t ehf_remove(extensible_hash_file_t hash, const char *key);

/**
 * @brief Verifica se o handle aponta para um índice aberto e válido.
 * @param hash Handle a verificar.
 * @return @c true se válido, @c false caso contrário.
 */
bool ehf_is_open(extensible_hash_file_t hash);

/**
 * @brief Tipo de callback para ehf_foreach.
 *
 * Chamada uma vez por entrada ocupada no índice.
 *
 * @param key         Chave da entrada.
 * @param record      Ponteiro para os bytes do registro.
 * @param record_size Tamanho do registro.
 * @param user_data   Ponteiro fornecido pelo chamador.
 */
typedef void (*ehf_visitor_fn)(const char *key, const void *record,
                               size_t record_size, void *user_data);

/**
 * @brief Itera todas as entradas ocupadas, chamando @p visitor para cada uma.
 *
 * A ordem de iteração não é garantida. @p record_size deve corresponder ao
 * tamanho usado na criação do arquivo.
 *
 * @param hash        Handle do índice (não-NULL).
 * @param visitor     Callback a invocar por entrada (não-NULL).
 * @param record_size Tamanho do registro em bytes.
 * @param user_data   Ponteiro livre repassado a @p visitor.
 * @return EHF_OK em sucesso; EHF_INVALID_ARGUMENT para argumentos inválidos;
 *         EHF_IO_ERROR ou EHF_CORRUPTED_FILE em problemas de leitura.
 */
ehf_status_t ehf_foreach(extensible_hash_file_t hash, ehf_visitor_fn visitor,
                         size_t record_size, void *user_data);

/**
 * @brief Grava um dump legível do arquivo de índice em @p output_path.
 *
 * O dump inclui o diretório, cada bucket com suas entradas e um log de todas
 * as divisões de bucket ocorridas na sessão.
 *
 * @param hash        Handle do índice (não-NULL).
 * @param output_path Caminho do arquivo de saída (não-NULL).
 * @return EHF_OK em sucesso; EHF_INVALID_ARGUMENT para argumentos NULL;
 *         EHF_IO_ERROR se o arquivo de saída não puder ser escrito.
 */
ehf_status_t ehf_dump(extensible_hash_file_t hash, const char *output_path);

#endif // EXTENSIBLE_HASH_FILE_H
