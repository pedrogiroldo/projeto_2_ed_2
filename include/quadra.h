/**
 * @file quadra.h
 * @brief Quadra urbana de Bitnópolis.
 *
 * Uma quadra é identificada pelo CEP e possui posição, dimensões e atributos
 * visuais. quadra_t é opaco; use quadra_criar() para instanciar e
 * quadra_destruir() para liberar. Para persistência em hashfile use
 * quadra_para_registro() / quadra_criar_de_registro().
 */

#ifndef QUADRA_H
#define QUADRA_H

#include <stddef.h>

/** Tamanho máximo do CEP (sem o terminador nulo). */
#define QUADRA_CEP_MAX 32u

/** Tamanho máximo de uma string de cor (sem o terminador nulo). */
#define QUADRA_COR_MAX 32u

/** Tipo opaco para uma quadra. */
typedef struct quadra quadra_t;

/**
 * @brief Registro plano serializável para armazenamento em hashfile (opaco).
 *
 * O layout é privado (definido em quadra.c). Aloque um buffer de
 * quadra_registro_size() bytes, preencha com quadra_registro_preencher() e
 * leia os campos com os getters quadra_registro_*(). A chave do hashfile é o
 * CEP. Manipular o registro como bytes opacos evita expor a struct no header.
 */
typedef void quadra_registro_t;

/**
 * @brief Tamanho em bytes de um registro de quadra serializado.
 *
 * Use como record_size em ehf_create/ehf_insert/ehf_find.
 */
size_t quadra_registro_size(void);

/**
 * @brief Preenche um buffer de registro (>= quadra_registro_size() bytes).
 *
 * @param registro         Buffer de destino (não-NULL).
 * @param cep              CEP (chave), truncado a QUADRA_CEP_MAX.
 * @param x                Coordenada X do canto superior esquerdo.
 * @param y                Coordenada Y do canto superior esquerdo.
 * @param largura          Largura da quadra.
 * @param altura           Altura da quadra.
 * @param cor_preenchimento Cor de preenchimento.
 * @param cor_borda        Cor da borda.
 * @param espessura_borda  Espessura da borda.
 */
void quadra_registro_preencher(quadra_registro_t *registro, const char *cep,
                               double x, double y, double largura, double altura,
                               const char *cor_preenchimento,
                               const char *cor_borda, double espessura_borda);

/** @brief CEP armazenado no registro. */
const char *quadra_registro_cep(const quadra_registro_t *registro);
/** @brief Coordenada X do registro. */
double quadra_registro_x(const quadra_registro_t *registro);
/** @brief Coordenada Y do registro. */
double quadra_registro_y(const quadra_registro_t *registro);
/** @brief Largura do registro. */
double quadra_registro_largura(const quadra_registro_t *registro);
/** @brief Altura do registro. */
double quadra_registro_altura(const quadra_registro_t *registro);
/** @brief Cor de preenchimento do registro. */
const char *quadra_registro_cor_preenchimento(const quadra_registro_t *registro);
/** @brief Cor da borda do registro. */
const char *quadra_registro_cor_borda(const quadra_registro_t *registro);
/** @brief Espessura da borda do registro. */
double quadra_registro_espessura_borda(const quadra_registro_t *registro);

/**
 * @brief Cria uma quadra com os dados fornecidos.
 *
 * @param cep              CEP da quadra (não-NULL, máximo QUADRA_CEP_MAX caracteres).
 * @param x                Coordenada X do canto superior esquerdo.
 * @param y                Coordenada Y do canto superior esquerdo.
 * @param largura          Largura da quadra.
 * @param altura           Altura da quadra.
 * @param cor_preenchimento Cor de preenchimento (não-NULL).
 * @param cor_borda        Cor da borda (não-NULL).
 * @param espessura_borda  Espessura da borda.
 * @return Nova instância ou NULL se algum argumento for inválido ou faltar memória.
 */
quadra_t *quadra_criar(const char *cep, double x, double y, double largura,
                       double altura, const char *cor_preenchimento,
                       const char *cor_borda, double espessura_borda);

/**
 * @brief Libera todos os recursos da quadra. Aceita NULL com segurança.
 * @param quadra Instância a destruir.
 */
void quadra_destruir(quadra_t *quadra);

/** @brief Retorna o CEP da quadra. NULL quando @p quadra é NULL. */
const char *quadra_obter_cep(const quadra_t *quadra);

/** @brief Retorna a coordenada X do canto superior esquerdo. */
double quadra_obter_x(const quadra_t *quadra);

/** @brief Retorna a coordenada Y do canto superior esquerdo. */
double quadra_obter_y(const quadra_t *quadra);

/** @brief Retorna a largura da quadra. */
double quadra_obter_largura(const quadra_t *quadra);

/** @brief Retorna a altura da quadra. */
double quadra_obter_altura(const quadra_t *quadra);

/** @brief Retorna a cor de preenchimento. NULL quando @p quadra é NULL. */
const char *quadra_obter_cor_preenchimento(const quadra_t *quadra);

/** @brief Retorna a cor da borda. NULL quando @p quadra é NULL. */
const char *quadra_obter_cor_borda(const quadra_t *quadra);

/** @brief Retorna a espessura da borda. */
double quadra_obter_espessura_borda(const quadra_t *quadra);

/**
 * @brief Serializa a quadra para um registro plano.
 *
 * @param quadra   Instância de origem (não pode ser NULL).
 * @param registro Buffer de destino (não pode ser NULL).
 * @return 0 em sucesso, -1 se algum argumento for NULL.
 */
int quadra_para_registro(const quadra_t *quadra, quadra_registro_t *registro);

/**
 * @brief Reconstrói uma quadra a partir de um registro plano.
 *
 * @param registro Registro de origem (não pode ser NULL).
 * @return Nova instância ou NULL se @p registro for NULL ou inválido.
 */
quadra_t *quadra_criar_de_registro(const quadra_registro_t *registro);

#endif // QUADRA_H
