#ifndef REGISTRADORES_H
#define REGISTRADORES_H

/**
 * @file registradores.h
 * @brief Contrato do TAD opaco para referencias geograficas R0..R10.
 *
 * Consultas iniciadas por @ armazenam coordenadas em registradores nomeados
 * R0, R1, ..., R10. Outras consultas, como p?, usam estes registradores como
 * origem e destino de percursos. A representacao concreta fica privada em
 * registradores.c.
 */

/**
 * @brief Quantidade de registradores geograficos exigidos pelo enunciado.
 */
#define REGISTRADORES_TOTAL 11

/**
 * @brief Instancia opaca da tabela de registradores.
 */
typedef void *Registradores;

/**
 * @brief Cria todos os registradores inicialmente vazios.
 *
 * @return Nova tabela de registradores, ou NULL em caso de falha de alocacao.
 */
Registradores registradores_create(void);

/**
 * @brief Converte um nome textual de registrador para indice interno.
 *
 * Aceita apenas os nomes R0, R1, ..., R10.
 *
 * @param nome Nome textual recebido do arquivo .qry.
 * @return Indice entre 0 e 10, ou -1 se o nome for invalido.
 */
int registradores_parse_nome(const char *nome);

/**
 * @brief Armazena uma coordenada em um registrador.
 *
 * @param registradores Tabela alterada.
 * @param indice Indice entre 0 e 10.
 * @param x Coordenada x da referencia geografica.
 * @param y Coordenada y da referencia geografica.
 * @return 1 se armazenou; 0 se os parametros forem invalidos.
 */
int registradores_set(Registradores registradores, int indice,
                      double x, double y);

/**
 * @brief Verifica se um registrador ja recebeu uma coordenada.
 *
 * @param registradores Tabela consultada.
 * @param indice Indice entre 0 e 10.
 * @return 1 se o registrador esta definido; 0 caso contrario.
 */
int registradores_is_set(Registradores registradores, int indice);

/**
 * @brief Recupera a coordenada armazenada em um registrador.
 *
 * @param registradores Tabela consultada.
 * @param indice Indice entre 0 e 10.
 * @param out_x Ponteiro opcional que recebera a coordenada x.
 * @param out_y Ponteiro opcional que recebera a coordenada y.
 * @return 1 se o registrador esta definido; 0 caso contrario.
 */
int registradores_get(Registradores registradores, int indice,
                      double *out_x, double *out_y);

/**
 * @brief Limpa um registrador, tornando-o vazio.
 *
 * @param registradores Tabela alterada.
 * @param indice Indice entre 0 e 10.
 * @return 1 se limpou; 0 se os parametros forem invalidos.
 */
int registradores_clear(Registradores registradores, int indice);

/**
 * @brief Libera a tabela de registradores.
 *
 * @param registradores Tabela criada por registradores_create.
 */
void registradores_destroy(Registradores registradores);

#endif
