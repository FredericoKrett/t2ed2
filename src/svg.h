#ifndef SVG_H
#define SVG_H

#include "grafo.h"
#include "quadra_store.h"

/**
 * @file svg.h
 * @brief Contrato do modulo responsavel por gerar arquivos SVG.
 *
 * O modulo recebe as estruturas ja carregadas em memoria e escreve uma
 * representacao visual da cidade. A implementacao concreta de como o arquivo
 * e montado fica privada em svg.c; clientes apenas solicitam a renderizacao.
 */

/**
 * @brief Gera um SVG base com quadras e sistema viario.
 *
 * Quadras sao desenhadas como retangulos com o estilo lido do .geo. Quando um
 * grafo viario e informado, suas arestas sao desenhadas como linhas
 * direcionadas e seus vertices como pequenos marcadores identificados.
 *
 * @param filepath Caminho do arquivo SVG a ser criado.
 * @param quadras Tabela de quadras carregada do .geo. Pode ser NULL.
 * @param grafo Grafo viario carregado do .via. Pode ser NULL.
 * @return 1 se escreveu o arquivo; 0 em caso de parametro invalido ou erro de
 *         abertura/escrita.
 */
int svg_render_base(const char *filepath, QuadraStore quadras, Grafo grafo);

#endif
