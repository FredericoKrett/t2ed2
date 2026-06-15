#ifndef SVG_H
#define SVG_H

#include "caminho.h"
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
 * @brief Lista opaca de percursos que devem ser desenhados sobre o mapa.
 */
typedef void *SvgPercursos;

/**
 * @brief Cria uma lista vazia de percursos desenhaveis.
 *
 * @return Lista criada, ou NULL em caso de falha de alocacao.
 */
SvgPercursos svg_percursos_create(void);

/**
 * @brief Adiciona um caminho calculado a lista de percursos.
 *
 * Quando a insercao ocorre com sucesso, a lista passa a ser dona do caminho e
 * deve libera-lo futuramente. Em caso de falha, o caminho continua pertencendo
 * ao chamador.
 *
 * @param percursos Lista alterada.
 * @param caminho Caminho calculado pelo modulo caminho.
 * @param cor Cor SVG usada para desenhar o percurso.
 * @return 1 se inseriu; 0 em caso de parametro invalido ou falha de alocacao.
 */
int svg_percursos_add(SvgPercursos percursos, Caminho caminho, const char *cor);

/**
 * @brief Libera a lista de percursos e os caminhos armazenados nela.
 *
 * @param percursos Lista criada por svg_percursos_create.
 */
void svg_percursos_destroy(SvgPercursos percursos);

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

/**
 * @brief Gera um SVG com quadras, sistema viario e percursos adicionais.
 *
 * @param filepath Caminho do arquivo SVG a ser criado.
 * @param quadras Tabela de quadras carregada do .geo. Pode ser NULL.
 * @param grafo Grafo viario carregado do .via. Pode ser NULL.
 * @param percursos Lista de percursos calculados. Pode ser NULL.
 * @return 1 se escreveu o arquivo; 0 em caso de parametro invalido ou erro de
 *         abertura/escrita.
 */
int svg_render_com_percursos(const char *filepath, QuadraStore quadras,
                             Grafo grafo, SvgPercursos percursos);

#endif
