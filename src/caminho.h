#ifndef CAMINHO_H
#define CAMINHO_H

#include "grafo.h"

#include <stddef.h>

/**
 * @file caminho.h
 * @brief Contrato do TAD opaco para caminhos minimos no grafo viario.
 *
 * O modulo calcula percursos entre vertices usando Dijkstra. O mesmo contrato
 * atende ao percurso mais curto, baseado no comprimento das arestas, e ao
 * percurso mais rapido, baseado no tempo de travessia de cada aresta. A
 * representacao concreta do resultado fica privada em caminho.c.
 */

/**
 * @brief Instancia opaca de caminho calculado.
 */
typedef void *Caminho;

/**
 * @brief Criterio usado para ponderar as arestas no Dijkstra.
 */
typedef enum {
    CAMINHO_CRITERIO_COMPRIMENTO = 0,
    CAMINHO_CRITERIO_TEMPO = 1
} CaminhoCriterio;

/**
 * @brief Calcula um caminho minimo entre dois vertices.
 *
 * Para CAMINHO_CRITERIO_COMPRIMENTO, o custo de cada aresta e seu comprimento.
 * Para CAMINHO_CRITERIO_TEMPO, o custo de cada aresta e cmp/vm. Arestas com
 * velocidade media menor ou igual a zero nao devem compor caminhos por tempo.
 *
 * @param grafo Grafo viario consultado.
 * @param origem Vertice inicial.
 * @param destino Vertice final.
 * @param criterio Criterio de ponderacao das arestas.
 * @return Caminho calculado, ou NULL em caso de parametro invalido ou falha de
 *         alocacao. Um destino inalcancavel retorna um caminho valido, mas sem
 *         percurso existente.
 */
Caminho caminho_calcular(Grafo grafo, GrafoVertice origem,
                         GrafoVertice destino, CaminhoCriterio criterio);

/**
 * @brief Informa se o destino foi alcancado pelo caminho.
 *
 * @param caminho Caminho consultado.
 * @return 1 se existe percurso da origem ate o destino; 0 caso contrario.
 */
int caminho_existe(Caminho caminho);

/**
 * @brief Retorna o criterio usado no calculo.
 *
 * @param caminho Caminho consultado.
 * @return Criterio usado, ou CAMINHO_CRITERIO_COMPRIMENTO se invalido.
 */
CaminhoCriterio caminho_get_criterio(Caminho caminho);

/**
 * @brief Retorna o vertice inicial do caminho.
 *
 * @param caminho Caminho consultado.
 * @return Vertice de origem, ou -1 se invalido.
 */
GrafoVertice caminho_get_origem(Caminho caminho);

/**
 * @brief Retorna o vertice final do caminho.
 *
 * @param caminho Caminho consultado.
 * @return Vertice de destino, ou -1 se invalido.
 */
GrafoVertice caminho_get_destino(Caminho caminho);

/**
 * @brief Retorna o custo total do percurso.
 *
 * @param caminho Caminho consultado.
 * @return Soma dos pesos das arestas, ou 0.0 se nao existe percurso.
 */
double caminho_get_custo(Caminho caminho);

/**
 * @brief Retorna a quantidade de arestas no percurso.
 *
 * @param caminho Caminho consultado.
 * @return Quantidade de arestas do percurso, ou 0 se invalido ou inalcancavel.
 */
size_t caminho_get_aresta_count(Caminho caminho);

/**
 * @brief Retorna uma aresta do percurso, na ordem origem-destino.
 *
 * As arestas retornadas pertencem ao grafo usado no calculo. O grafo deve
 * permanecer vivo enquanto o caminho estiver sendo consultado.
 *
 * @param caminho Caminho consultado.
 * @param indice Posicao da aresta no percurso.
 * @return Aresta do percurso, ou NULL se o indice for invalido.
 */
GrafoAresta caminho_get_aresta(Caminho caminho, size_t indice);

/**
 * @brief Libera o caminho calculado.
 *
 * @param caminho Caminho criado por caminho_calcular.
 */
void caminho_destroy(Caminho caminho);

#endif
