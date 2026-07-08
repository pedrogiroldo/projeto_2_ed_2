/**
 * @file registradores.h
 * @brief Registradores geográficos R0..R10 e cálculo de coordenada por endereço.
 *
 * Cada registrador guarda uma coordenada (x, y) e um estado ocupado/livre.
 * O tipo é opaco; a struct interna é definida apenas no .c.
 *
 * Cálculo de coordenada (sistema de endereçamento de Bitnópolis):
 *   - Origem (0,0) no canto superior esquerdo; X cresce à direita, Y para baixo.
 *   - Ancoragem da quadra = canto sudeste = (qx + qw, qy + qh),
 *     dado (qx, qy) o canto superior esquerdo, qw largura, qh altura.
 *   - O número é a distância, ao longo da face, a partir da projeção do canto
 *     sudeste sobre aquela face.
 */

#ifndef REGISTRADORES_H
#define REGISTRADORES_H

#include <stdbool.h>

/** Número de registradores disponíveis: R0..R10. */
#define REG_TOTAL 11

typedef void *Registradores;

/**
 * @brief Cria um conjunto de registradores R0..R10, todos livres.
 * @return Instância opaca ou NULL em falha.
 */
Registradores registradores_criar(void);

/**
 * @brief Libera os registradores (aceita NULL).
 */
void registradores_destruir(Registradores regs);

/**
 * @brief Converte um nome de registrador ("R0".."R10") em índice 0..10.
 * @return Índice válido, ou -1 se o nome for inválido/fora do intervalo.
 */
int reg_indice(const char *nome);

/**
 * @brief Armazena uma coordenada no registrador de índice idx (0..REG_TOTAL-1).
 * @return true em sucesso, false se regs NULL ou idx fora do intervalo.
 */
bool reg_armazenar(Registradores regs, int idx, double x, double y);

/**
 * @brief Obtém a coordenada armazenada no registrador idx.
 * @param x_out [out] coordenada x (pode ser NULL)
 * @param y_out [out] coordenada y (pode ser NULL)
 * @return true se o registrador está ocupado e idx é válido; false caso contrário.
 */
bool reg_obter(Registradores regs, int idx, double *x_out, double *y_out);

/**
 * @brief Indica se o registrador idx está ocupado.
 */
bool reg_ocupado(Registradores regs, int idx);

/**
 * @brief Calcula a coordenada (x, y) de um endereço sobre uma quadra.
 *
 * @param qx    X do canto superior esquerdo da quadra
 * @param qy    Y do canto superior esquerdo da quadra
 * @param qw    largura da quadra
 * @param qh    altura da quadra
 * @param face  face do endereço: 'N', 'S', 'L' ou 'O' (maiúscula ou minúscula)
 * @param num   número do endereço (distância a partir do canto sudeste projetado)
 * @param x_out [out] coordenada x resultante (não-NULL)
 * @param y_out [out] coordenada y resultante (não-NULL)
 * @return true se a face é válida e os ponteiros de saída não são NULL.
 */
bool reg_coordenada_endereco(double qx, double qy, double qw, double qh,
                             char face, double num,
                             double *x_out, double *y_out);

#endif /* REGISTRADORES_H */
