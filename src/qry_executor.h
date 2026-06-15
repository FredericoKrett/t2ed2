#ifndef QRY_EXECUTOR_H
#define QRY_EXECUTOR_H

#include "qry_parser.h"
#include "quadra_store.h"
#include "registradores.h"

/**
 * @file qry_executor.h
 * @brief Contrato do modulo que aplica consultas .qry sobre a cidade.
 *
 * O parser de .qry apenas valida e guarda os comandos em ordem. Este modulo
 * interpreta parte desses comandos usando as estruturas ja carregadas em
 * memoria. A representacao concreta dos comandos, das quadras e dos
 * registradores permanece escondida nos respectivos TADs.
 */

/**
 * @brief Resolve comandos @o? e armazena suas coordenadas em registradores.
 *
 * Percorre a lista de comandos na ordem original. Para cada @o?, busca a
 * quadra pelo CEP, calcula o ponto do endereco na face informada e armazena a
 * coordenada no registrador textual indicado. Comandos de outros tipos sao
 * ignorados nesta etapa, pois serao tratados por outras funcoes do executor.
 *
 * @param comandos Lista criada pelo parser .qry.
 * @param quadras Tabela de quadras carregada do .geo.
 * @param registradores Tabela R0..R10 que recebera as coordenadas.
 * @return 1 se todos os @o? foram resolvidos; 0 em caso de parametro invalido,
 *         registrador invalido, CEP ausente ou face invalida.
 */
int qry_executor_resolve_origens(QryComandos comandos, QuadraStore quadras,
                                 Registradores registradores);

#endif
