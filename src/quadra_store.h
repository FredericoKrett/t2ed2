#ifndef QUADRA_STORE_H
#define QUADRA_STORE_H

#include <stddef.h>

#include "quadra.h"

/**
 * @file quadra_store.h
 * @brief Contrato do TAD opaco que indexa quadras pelo CEP.
 *
 * O modulo oferece busca eficiente de quadras por CEP e iteracao sobre todas
 * as quadras carregadas do arquivo .geo. A representacao interna permanece
 * privada em quadra_store.c.
 */

/**
 * @brief Instancia opaca da tabela de quadras.
 */
typedef void *QuadraStore;

/**
 * @brief Funcao chamada durante a iteracao pelas quadras armazenadas.
 */
typedef void (*QuadraStoreVisitor)(Quadra quadra, void *context);

/**
 * @brief Cria uma tabela vazia para armazenar quadras.
 *
 * @param capacity_hint Estimativa inicial da quantidade de quadras. Pode ser 0.
 * @return Nova tabela, ou NULL em caso de falha de alocacao.
 */
QuadraStore quadra_store_create(size_t capacity_hint);

/**
 * @brief Insere uma quadra na tabela, usando seu CEP como chave.
 *
 * A tabela passa a ser responsavel por destruir a quadra quando a insercao
 * ocorre com sucesso. CEPs duplicados sao rejeitados.
 *
 * @param store Tabela de destino.
 * @param quadra Quadra criada pelo modulo quadra.
 * @return 1 se inseriu; 0 se houve parametro invalido, erro ou CEP duplicado.
 */
int quadra_store_insert(QuadraStore store, Quadra quadra);

/**
 * @brief Busca uma quadra pelo CEP.
 *
 * A quadra retornada continua pertencendo a tabela e nao deve ser destruida
 * diretamente pelo chamador.
 *
 * @param store Tabela consultada.
 * @param cep CEP procurado.
 * @return Quadra encontrada, ou NULL se ausente.
 */
Quadra quadra_store_find(QuadraStore store, const char *cep);

/**
 * @brief Remove uma quadra pelo CEP sem destrui-la.
 *
 * A propriedade da quadra removida passa para o chamador, que deve destrui-la
 * com quadra_destroy.
 *
 * @param store Tabela alterada.
 * @param cep CEP removido.
 * @return Quadra removida, ou NULL se ausente.
 */
Quadra quadra_store_remove(QuadraStore store, const char *cep);

/**
 * @brief Retorna a quantidade de quadras armazenadas.
 *
 * @param store Tabela consultada.
 * @return Quantidade de quadras, ou 0 se a tabela for invalida.
 */
size_t quadra_store_count(QuadraStore store);

/**
 * @brief Percorre todas as quadras armazenadas.
 *
 * A ordem de visita nao faz parte do contrato. A funcao visitante recebe cada
 * quadra e o contexto informado pelo chamador.
 *
 * @param store Tabela percorrida.
 * @param visitor Funcao chamada para cada quadra.
 * @param context Contexto repassado ao visitante.
 */
void quadra_store_for_each(QuadraStore store, QuadraStoreVisitor visitor,
                           void *context);

/**
 * @brief Destroi a tabela e todas as quadras ainda armazenadas nela.
 *
 * @param store Tabela criada por quadra_store_create.
 */
void quadra_store_destroy(QuadraStore store);

#endif
