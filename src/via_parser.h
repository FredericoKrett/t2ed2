#ifndef VIA_PARSER_H
#define VIA_PARSER_H

#include "grafo.h"

/**
 * @file via_parser.h
 * @brief Contrato do modulo que interpreta arquivos .via.
 *
 * O arquivo .via descreve o sistema viario da cidade. A primeira linha informa
 * a quantidade de vertices; linhas "v" criam vertices com id e coordenadas;
 * linhas "e" criam arestas direcionadas com os atributos exigidos no enunciado.
 */

/**
 * @brief Processa um arquivo .via e cria o grafo viario correspondente.
 *
 * O grafo retornado pertence ao chamador, que deve libera-lo com grafo_destroy.
 * Em caso de erro de abertura, comando malformado, quantidade de vertices
 * diferente da declarada, vertice duplicado ou aresta referenciando vertice
 * inexistente, a funcao retorna NULL.
 *
 * @param filepath Caminho do arquivo .via.
 * @return Grafo direcionado carregado, ou NULL em caso de erro.
 */
Grafo via_parser_parse_file(const char *filepath);

#endif
