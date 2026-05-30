#ifndef GEO_PARSER_H
#define GEO_PARSER_H

#include "quadra_store.h"

/**
 * @file geo_parser.h
 * @brief Contrato do modulo que interpreta arquivos .geo.
 *
 * O modulo reconhece os comandos de cidade exigidos no Trabalho 2:
 * - cq sw cfill cstrk: altera o estilo das quadras seguintes;
 * - q cep x y w h: cria uma quadra e a insere na tabela recebida.
 *
 * A responsabilidade por armazenar e destruir as quadras criadas e da
 * QuadraStore passada pelo chamador.
 */

/**
 * @brief Processa um arquivo .geo e carrega suas quadras na tabela.
 *
 * @param filepath Caminho do arquivo .geo.
 * @param store Tabela de quadras que recebera as quadras lidas.
 * @return Quantidade de quadras inseridas; -1 em caso de erro de abertura,
 *         parametro invalido ou comando malformado.
 */
int geo_parser_parse_file(const char *filepath, QuadraStore store);

#endif
