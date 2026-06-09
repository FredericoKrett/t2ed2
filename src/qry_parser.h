#ifndef QRY_PARSER_H
#define QRY_PARSER_H

#include <stddef.h>

/**
 * @file qry_parser.h
 * @brief Contrato do modulo que interpreta arquivos .qry.
 *
 * O arquivo .qry contem consultas e alteracoes sobre a cidade e o sistema
 * viario. Este modulo le o arquivo e devolve uma lista opaca de comandos
 * validados, preservando a ordem em que aparecem na entrada. A execucao dos
 * comandos e responsabilidade de outro modulo.
 */

/**
 * @brief Lista opaca de comandos lidos de um arquivo .qry.
 */
typedef void *QryComandos;

/**
 * @brief Comando opaco pertencente a uma lista QryComandos.
 */
typedef void *QryComando;

/**
 * @brief Tipo de comando reconhecido no arquivo .qry.
 */
typedef enum {
    QRY_COMANDO_ORIGEM = 0,
    QRY_COMANDO_MVM = 1,
    QRY_COMANDO_REGS = 2,
    QRY_COMANDO_EXP = 3,
    QRY_COMANDO_PERCURSO = 4
} QryComandoTipo;

/**
 * @brief Processa um arquivo .qry e cria uma lista de comandos.
 *
 * Reconhece os comandos exigidos no enunciado:
 * - @o? reg cep face num;
 * - mvm v x y w h;
 * - regs vl;
 * - exp vl;
 * - p? reg1 reg2 cc cr.
 *
 * @param filepath Caminho do arquivo .qry.
 * @return Lista de comandos, ou NULL em caso de erro de abertura, parametro
 *         invalido ou comando malformado.
 */
QryComandos qry_parser_parse_file(const char *filepath);

/**
 * @brief Retorna a quantidade de comandos armazenados.
 *
 * @param comandos Lista consultada.
 * @return Quantidade de comandos, ou 0 se a lista for invalida.
 */
size_t qry_comandos_count(QryComandos comandos);

/**
 * @brief Retorna um comando por indice.
 *
 * @param comandos Lista consultada.
 * @param indice Posicao do comando na ordem do arquivo.
 * @return Comando pertencente a lista, ou NULL se o indice for invalido.
 */
QryComando qry_comandos_get(QryComandos comandos, size_t indice);

/**
 * @brief Retorna o tipo do comando.
 *
 * @param comando Comando consultado.
 * @return Tipo do comando, ou QRY_COMANDO_ORIGEM se invalido.
 */
QryComandoTipo qry_comando_get_tipo(QryComando comando);

/**
 * @brief Retorna o primeiro registrador textual do comando.
 *
 * Usado por @o? e como origem em p?.
 *
 * @param comando Comando consultado.
 * @return Nome do registrador, ou NULL se ausente.
 */
const char *qry_comando_get_reg1(QryComando comando);

/**
 * @brief Retorna o segundo registrador textual do comando.
 *
 * Usado como destino em p?.
 *
 * @param comando Comando consultado.
 * @return Nome do registrador, ou NULL se ausente.
 */
const char *qry_comando_get_reg2(QryComando comando);

/**
 * @brief Retorna o CEP usado por @o?.
 *
 * @param comando Comando consultado.
 * @return CEP do endereco, ou NULL se ausente.
 */
const char *qry_comando_get_cep(QryComando comando);

/**
 * @brief Retorna a face usada por @o?.
 *
 * @param comando Comando consultado.
 * @return Face N, S, L ou O; '\0' se ausente.
 */
char qry_comando_get_face(QryComando comando);

/**
 * @brief Retorna o numero do endereco usado por @o?.
 */
double qry_comando_get_num(QryComando comando);

/**
 * @brief Retorna a velocidade usada por mvm.
 */
double qry_comando_get_velocidade(QryComando comando);

/**
 * @brief Retorna o limite de velocidade usado por regs ou exp.
 */
double qry_comando_get_limite_velocidade(QryComando comando);

/**
 * @brief Retorna a coordenada x usada por mvm.
 */
double qry_comando_get_x(QryComando comando);

/**
 * @brief Retorna a coordenada y usada por mvm.
 */
double qry_comando_get_y(QryComando comando);

/**
 * @brief Retorna a largura usada por mvm.
 */
double qry_comando_get_w(QryComando comando);

/**
 * @brief Retorna a altura usada por mvm.
 */
double qry_comando_get_h(QryComando comando);

/**
 * @brief Retorna a cor do caminho mais curto usada por p?.
 *
 * @param comando Comando consultado.
 * @return Cor textual, ou NULL se ausente.
 */
const char *qry_comando_get_cor_curto(QryComando comando);

/**
 * @brief Retorna a cor do caminho mais rapido usada por p?.
 *
 * @param comando Comando consultado.
 * @return Cor textual, ou NULL se ausente.
 */
const char *qry_comando_get_cor_rapido(QryComando comando);

/**
 * @brief Libera a lista de comandos e seus recursos internos.
 *
 * @param comandos Lista criada por qry_parser_parse_file.
 */
void qry_comandos_destroy(QryComandos comandos);

#endif
