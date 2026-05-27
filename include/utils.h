/**
 * @file utils.h
 * @brief Funções utilitárias gerais.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>

/**
 * @brief Duplica uma string usando malloc.
 *
 * @param s String de origem (pode ser NULL).
 * @return Nova string alocada ou NULL se @p s for NULL ou faltar memória.
 *         O chamador deve liberar com free().
 */
char *duplicate_string(const char *s);

/**
 * @brief Retorna a cor invertida de @p color.
 *
 * Suporta cores hexadecimais de 6 dígitos (ex.: @c "#aabbcc") e um conjunto
 * reduzido de nomes CSS (@c "red", @c "blue", @c "green", @c "yellow",
 * @c "black", @c "pink", @c "cyan", @c "orange", @c "teal", @c "purple").
 * Para entradas reconhecidas, retorna uma nova string no formato @c "#RRGGBB"
 * representando a cor invertida. Para entradas não reconhecidas, retorna uma
 * cópia da string original.
 *
 * @param color String da cor de entrada (não-NULL).
 * @return Nova string alocada com a cor invertida. O chamador deve liberar com free().
 */
char *invert_color(const char *color);

#endif // UTILS_H
