#ifndef QRY_EXECUTOR_H
#define QRY_EXECUTOR_H

#include "caminho.h"
#include "grafo.h"
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

/**
 * @brief Calcula os dois percursos de um comando p?.
 *
 * Usa os registradores textuais do comando para recuperar as coordenadas de
 * origem e destino. Cada coordenada e associada ao vertice mais proximo do
 * grafo viario. Em seguida, calcula o caminho mais curto por comprimento e o
 * caminho mais rapido por tempo de travessia.
 *
 * Os caminhos retornados pertencem ao chamador, que deve libera-los com
 * caminho_destroy. Em caso de erro, nenhum caminho fica pendente para o
 * chamador destruir.
 *
 * @param comando Comando p? obtido do parser.
 * @param grafo Grafo viario consultado.
 * @param registradores Tabela R0..R10 com origem e destino ja definidos.
 * @param out_curto Ponteiro que recebera o caminho por comprimento.
 * @param out_rapido Ponteiro que recebera o caminho por tempo.
 * @return 1 se calculou os caminhos; 0 em caso de parametro invalido,
 *         comando de outro tipo, registrador ausente ou falha de calculo.
 */
int qry_executor_calcular_percurso(QryComando comando, Grafo grafo,
                                   Registradores registradores,
                                   Caminho *out_curto,
                                   Caminho *out_rapido);

#endif
