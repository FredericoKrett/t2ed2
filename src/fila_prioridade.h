#ifndef FILA_PRIORIDADE_H
#define FILA_PRIORIDADE_H

#include <stddef.h>

/**
 * @file fila_prioridade.h
 * @brief Contrato do TAD opaco de fila de prioridade minima.
 *
 * A fila armazena pares (valor, prioridade) e remove sempre o valor com menor
 * prioridade. O modulo sera usado pelo algoritmo de Dijkstra para selecionar o
 * proximo vertice a processar. A representacao concreta fica privada em
 * fila_prioridade.c.
 */

/**
 * @brief Instancia opaca da fila de prioridade.
 */
typedef void *FilaPrioridade;

/**
 * @brief Cria uma fila vazia.
 *
 * @param capacity_hint Estimativa inicial de elementos. Pode ser 0.
 * @return Nova fila, ou NULL em caso de falha de alocacao.
 */
FilaPrioridade fila_prioridade_create(size_t capacity_hint);

/**
 * @brief Insere um valor associado a uma prioridade.
 *
 * @param fila Fila alterada.
 * @param valor Valor inteiro armazenado.
 * @param prioridade Prioridade usada para ordenar a remocao.
 * @return 1 se inseriu; 0 em caso de parametro invalido ou falha de alocacao.
 */
int fila_prioridade_push(FilaPrioridade fila, int valor, double prioridade);

/**
 * @brief Remove o item de menor prioridade.
 *
 * @param fila Fila alterada.
 * @param out_valor Ponteiro que recebera o valor removido.
 * @param out_prioridade Ponteiro que recebera a prioridade removida.
 * @return 1 se removeu um item; 0 se a fila estava vazia ou invalida.
 */
int fila_prioridade_pop(FilaPrioridade fila, int *out_valor,
                        double *out_prioridade);

/**
 * @brief Consulta o item de menor prioridade sem remove-lo.
 *
 * @param fila Fila consultada.
 * @param out_valor Ponteiro que recebera o valor do topo.
 * @param out_prioridade Ponteiro que recebera a prioridade do topo.
 * @return 1 se havia item no topo; 0 se a fila estava vazia ou invalida.
 */
int fila_prioridade_peek(FilaPrioridade fila, int *out_valor,
                         double *out_prioridade);

/**
 * @brief Informa se a fila esta vazia.
 *
 * @param fila Fila consultada.
 * @return 1 se vazia ou invalida; 0 se possui elementos.
 */
int fila_prioridade_is_empty(FilaPrioridade fila);

/**
 * @brief Retorna a quantidade de elementos armazenados.
 *
 * @param fila Fila consultada.
 * @return Quantidade de elementos, ou 0 se a fila for invalida.
 */
size_t fila_prioridade_size(FilaPrioridade fila);

/**
 * @brief Libera a fila e seus recursos internos.
 *
 * @param fila Fila criada por fila_prioridade_create.
 */
void fila_prioridade_destroy(FilaPrioridade fila);

#endif
