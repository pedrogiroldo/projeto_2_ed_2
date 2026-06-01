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

/** Tamanho máximo do CEP (sem o terminador nulo). */
#define QUADRA_CEP_MAX 32u

/** Tamanho máximo de uma string de cor (sem o terminador nulo). */
#define QUADRA_COR_MAX 32u

/** Tipo opaco para uma quadra. */
typedef struct quadra quadra_t;

/**
 * @brief Registro plano serializável para armazenamento em hashfile.
 *
 * Chave do hashfile: campo @c cep.
 */
typedef struct {
    char   cep[QUADRA_CEP_MAX + 1u];                /**< CEP da quadra (chave). */
    double x;                                        /**< Coordenada X do canto superior esquerdo. */
    double y;                                        /**< Coordenada Y do canto superior esquerdo. */
    double largura;                                  /**< Largura em unidades do mapa. */
    double altura;                                   /**< Altura em unidades do mapa. */
    char   cor_preenchimento[QUADRA_COR_MAX + 1u];  /**< Cor de preenchimento (ex.: "blue", "#aabbcc"). */
    char   cor_borda[QUADRA_COR_MAX + 1u];          /**< Cor da borda. */
    double espessura_borda;                          /**< Espessura da borda em pixels. */
} quadra_registro_t;

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
