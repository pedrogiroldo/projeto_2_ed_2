/**
 * @file qry_handler.h
 * @brief Processador de consultas (.qry) do Projeto 2 — SIG viário de Bitnópolis.
 *
 * Comandos suportados:
 * | Comando | Argumentos          | Descrição                                             |
 * |---------|---------------------|-------------------------------------------------------|
 * | @c \@o? | @c reg @c cep @c face @c num | Armazena a coordenada do endereço no registrador. |
 * | @c mvm  | @c v @c x @c y @c w @c h     | Ajusta vm das arestas com origem na região.       |
 * | @c regs | @c vl               | Componentes conexos do subgrafo com vm >= vl.        |
 * | @c exp  | @c vl               | AGM; acelera trechos-gargalo (vm < vl) em 50%.       |
 * | @c p?   | @c reg1 @c reg2 @c cc @c cr | Percurso mais curto e mais rápido entre registradores. |
 *
 * Saídas textuais via @p txt e marcações visuais via @p svg (ambos podem ser
 * NULL para omitir a saída correspondente).
 */

#ifndef QRY_HANDLER_H
#define QRY_HANDLER_H

#include "grafo.h"
#include "registradores.h"
#include "extensible_hash_file.h"
#include "svg_writer.h"
#include "txt_writer.h"

/**
 * @brief Lê e processa um arquivo .qry, consultando o hashfile de quadras e
 *        atualizando o grafo/registradores conforme os comandos.
 *
 * @param qry_filepath  Caminho para o arquivo .qry (não-NULL).
 * @param grafo         Grafo viário já carregado (não-NULL).
 * @param quadras_hf    Hashfile de quadras aberto (record = quadra_registro_t).
 * @param registradores Conjunto de registradores R0..R10 (não-NULL).
 * @param svg           Writer SVG para marcações, ou NULL para omitir.
 * @param txt           Writer de texto para respostas, ou NULL para omitir.
 * @param comandos_out  [out] Nº de comandos processados com sucesso (pode ser NULL).
 * @param erros_out     [out] Nº de comandos com erro/argumentos inválidos (pode ser NULL).
 * @return 0 em sucesso; -1 se não foi possível abrir o arquivo ou argumento inválido.
 */
int qry_processar(const char *qry_filepath,
                  Grafo grafo,
                  extensible_hash_file_t quadras_hf,
                  Registradores registradores,
                  svg_writer_t *svg,
                  txt_writer_t *txt,
                  int *comandos_out,
                  int *erros_out);

#endif /* QRY_HANDLER_H */
