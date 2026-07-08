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

/* ======================================================================== */
/* Extensões do Projeto 2 — saídas textuais de consultas                    */
/* ======================================================================== */

/**
 * @brief Escreve a coordenada calculada de um endereço (comando @c \@o?).
 *        Formato: "<reg>: (x, y)".
 *
 * @param tw  Writer de destino.
 * @param reg Nome do registrador (ex.: "R3"); pode ser NULL.
 * @param x   Coordenada X calculada.
 * @param y   Coordenada Y calculada.
 */
void txt_writer_endereco(txt_writer_t *tw, const char *reg, double x, double y);

/**
 * @brief Escreve o número de componentes conexos (comando @c regs).
 *        Formato: "Numero de componentes: <num>".
 *
 * @param tw  Writer de destino.
 * @param num Quantidade de componentes.
 */
void txt_writer_num_componentes(txt_writer_t *tw, int num);

/**
 * @brief Escreve um passo do percurso (comando @c p?).
 *        Formato: "Siga na direcao <direcao> pela <rua>".
 *
 * @param tw      Writer de destino.
 * @param direcao Direção cardeal ("Norte", "Sul", "Leste", "Oeste").
 * @param rua     Nome da rua percorrida.
 */
void txt_writer_passo(txt_writer_t *tw, const char *direcao, const char *rua);

/**
 * @brief Escreve que o destino é inacessível (comando @c p?).
 *        Formato: "Destino inacessivel".
 *
 * @param tw Writer de destino.
 */
void txt_writer_destino_inacessivel(txt_writer_t *tw);

/**
 * @brief Retorna a direção cardeal dominante de um deslocamento (dx, dy).
 *
 * Considera o sistema de coordenadas onde Y cresce para baixo:
 *   dy < 0 → "Norte", dy > 0 → "Sul", dx > 0 → "Leste", dx < 0 → "Oeste".
 * A componente de maior módulo prevalece. Retorna "" se dx == dy == 0.
 *
 * @param dx Deslocamento em X (destino - origem).
 * @param dy Deslocamento em Y (destino - origem).
 * @return String estática constante (não liberar).
 */
const char *txt_writer_direcao_cardeal(double dx, double dy);

#endif // TXT_WRITER_H
