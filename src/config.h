#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

/**
 * @file config.h
 * @brief Contrato do modulo responsavel pelos parametros de execucao.
 *
 * O modulo interpreta a linha de comando do programa ted e guarda os caminhos
 * informados por -e, -f, -q, -v e -o. A estrutura concreta permanece privada
 * em config.c; usuarios do modulo manipulam apenas o ponteiro opaco Config.
 */

/**
 * @brief Instancia opaca com a configuracao da execucao.
 */
typedef void *Config;

/**
 * @brief Cria uma configuracao vazia com valores padrao.
 *
 * O diretorio-base de entrada padrao deve ser ".". Arquivos opcionais que nao
 * forem informados permanecem ausentes. O chamador deve liberar a instancia com
 * config_destroy.
 *
 * @return Configuracao criada, ou NULL em caso de falha de alocacao.
 */
Config config_create(void);

/**
 * @brief Interpreta os argumentos recebidos por main.
 *
 * Reconhece os parametros em qualquer ordem:
 * - -e path: diretorio-base de entrada, opcional;
 * - -f arq.geo: arquivo de quadras, obrigatorio;
 * - -q consulta.qry: arquivo de consultas, opcional;
 * - -v arq.via: arquivo de vias, opcional;
 * - -o dir: diretorio-base de saida, obrigatorio.
 *
 * @param config Configuracao a ser preenchida.
 * @param argc Quantidade de argumentos recebidos por main.
 * @param argv Vetor de argumentos recebido por main.
 * @return true se os argumentos obrigatorios foram informados e reconhecidos;
 *         false caso contrario.
 */
bool config_parse_args(Config config, int argc, char *argv[]);

/**
 * @brief Retorna o diretorio-base de entrada.
 *
 * @param config Configuracao consultada.
 * @return Caminho informado por -e, ou "." quando -e nao foi usado.
 */
const char *config_get_input_dir(Config config);

/**
 * @brief Retorna o diretorio-base de saida.
 *
 * @param config Configuracao consultada.
 * @return Caminho informado por -o, ou NULL se ausente.
 */
const char *config_get_output_dir(Config config);

/**
 * @brief Retorna o nome do arquivo .geo.
 *
 * @param config Configuracao consultada.
 * @return Nome informado por -f, ou NULL se ausente.
 */
const char *config_get_geo_file(Config config);

/**
 * @brief Retorna o nome do arquivo .qry.
 *
 * @param config Configuracao consultada.
 * @return Nome informado por -q, ou NULL se ausente.
 */
const char *config_get_qry_file(Config config);

/**
 * @brief Retorna o nome do arquivo .via.
 *
 * @param config Configuracao consultada.
 * @return Nome informado por -v, ou NULL se ausente.
 */
const char *config_get_via_file(Config config);

/**
 * @brief Libera a configuracao e seus recursos internos.
 *
 * @param config Configuracao criada por config_create.
 */
void config_destroy(Config config);

#endif
