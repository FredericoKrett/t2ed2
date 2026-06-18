#ifndef SVG_H
#define SVG_H

#include "caminho.h"
#include "grafo.h"
#include "quadra_store.h"

/**
 * @file svg.h
 * @brief Contrato do modulo responsavel por gerar arquivos SVG.
 *
 * O modulo recebe as estruturas ja carregadas em memoria e escreve uma
 * representacao visual da cidade. A implementacao concreta de como o arquivo
 * e montado fica privada em svg.c; clientes apenas solicitam a renderizacao.
 */

/**
 * @brief Lista opaca de percursos que devem ser desenhados sobre o mapa.
 */
typedef void *SvgPercursos;

/**
 * @brief Lista opaca de regioes destacadas por consultas do .qry.
 */
typedef void *SvgRegioes;

/**
 * @brief Lista opaca de regioes afetadas por comandos mvm.
 */
typedef void *SvgMvmRegioes;

/**
 * @brief Lista opaca de arestas destacadas por expansao de vias.
 */
typedef void *SvgExpansoes;

/**
 * @brief Lista opaca de origens/endereco marcados por comandos @o?.
 */
typedef void *SvgOrigens;

/**
 * @brief Cria uma lista vazia de percursos desenhaveis.
 *
 * @return Lista criada, ou NULL em caso de falha de alocacao.
 */
SvgPercursos svg_percursos_create(void);

/**
 * @brief Adiciona um caminho calculado a lista de percursos.
 *
 * Quando a insercao ocorre com sucesso, a lista passa a ser dona do caminho e
 * deve libera-lo futuramente. Em caso de falha, o caminho continua pertencendo
 * ao chamador.
 *
 * @param percursos Lista alterada.
 * @param caminho Caminho calculado pelo modulo caminho.
 * @param cor Cor SVG usada para desenhar o percurso.
 * @return 1 se inseriu; 0 em caso de parametro invalido ou falha de alocacao.
 */
int svg_percursos_add(SvgPercursos percursos, Caminho caminho, const char *cor);

/**
 * @brief Libera a lista de percursos e os caminhos armazenados nela.
 *
 * @param percursos Lista criada por svg_percursos_create.
 */
void svg_percursos_destroy(SvgPercursos percursos);

/**
 * @brief Cria uma lista vazia de regioes desenhaveis.
 *
 * @return Lista criada, ou NULL em caso de falha de alocacao.
 */
SvgRegioes svg_regioes_create(void);

/**
 * @brief Copia componentes lentos para a lista de regioes desenhaveis.
 *
 * Para cada componente recebido, copia seu retangulo envolvente e atribui uma
 * cor de desenho. O resultado GrafoComponentes continua pertencendo ao
 * chamador e deve ser destruido pelo modulo que o criou.
 *
 * @param regioes Lista alterada.
 * @param componentes Componentes calculados pelo TAD de grafo.
 * @return 1 se copiou todos os componentes; 0 em caso de parametro invalido ou
 *         falha de alocacao.
 */
int svg_regioes_add_componentes(SvgRegioes regioes,
                                GrafoComponentes componentes);

/**
 * @brief Libera a lista de regioes desenhaveis.
 *
 * @param regioes Lista criada por svg_regioes_create.
 */
void svg_regioes_destroy(SvgRegioes regioes);

/**
 * @brief Cria uma lista vazia de regioes alteradas por mvm.
 *
 * @return Lista criada, ou NULL em caso de falha de alocacao.
 */
SvgMvmRegioes svg_mvm_regioes_create(void);

/**
 * @brief Adiciona o retangulo de uma regiao alterada por mvm.
 *
 * O retangulo e copiado para que o SVG possa destacar a area cuja velocidade
 * media foi modificada.
 *
 * @param regioes Lista alterada.
 * @param x Coordenada x do canto inferior esquerdo.
 * @param y Coordenada y do canto inferior esquerdo.
 * @param w Largura da regiao.
 * @param h Altura da regiao.
 * @return 1 se inseriu; 0 em caso de parametro invalido ou falha de alocacao.
 */
int svg_mvm_regioes_add(SvgMvmRegioes regioes, double x, double y,
                        double w, double h);

/**
 * @brief Libera a lista de regioes alteradas por mvm.
 *
 * @param regioes Lista criada por svg_mvm_regioes_create.
 */
void svg_mvm_regioes_destroy(SvgMvmRegioes regioes);

/**
 * @brief Cria uma lista vazia de expansoes desenhaveis.
 *
 * @return Lista criada, ou NULL em caso de falha de alocacao.
 */
SvgExpansoes svg_expansoes_create(void);

/**
 * @brief Copia arestas expandidas para a lista de expansoes desenhaveis.
 *
 * A funcao copia apenas as coordenadas das extremidades. A lista GrafoArestas
 * continua pertencendo ao chamador e pode ser liberada depois da chamada.
 *
 * @param expansoes Lista alterada.
 * @param grafo Grafo usado para consultar as coordenadas das arestas.
 * @param arestas Lista de arestas alteradas pela expansao.
 * @return 1 se copiou todos os segmentos; 0 em caso de parametro invalido ou
 *         falha de alocacao.
 */
int svg_expansoes_add_arestas(SvgExpansoes expansoes, Grafo grafo,
                              GrafoArestas arestas);

/**
 * @brief Libera a lista de expansoes desenhaveis.
 *
 * @param expansoes Lista criada por svg_expansoes_create.
 */
void svg_expansoes_destroy(SvgExpansoes expansoes);

/**
 * @brief Cria uma lista vazia de origens desenhaveis.
 *
 * @return Lista criada, ou NULL em caso de falha de alocacao.
 */
SvgOrigens svg_origens_create(void);

/**
 * @brief Adiciona a coordenada resolvida de um registrador de origem.
 *
 * A funcao copia o nome textual do registrador e a coordenada calculada para
 * que o SVG possa desenhar uma linha vertical pontilhada ate o topo do mapa.
 *
 * @param origens Lista alterada.
 * @param registrador Nome textual do registrador, como R0 ou R10.
 * @param x Coordenada x resolvida.
 * @param y Coordenada y resolvida.
 * @return 1 se inseriu; 0 em caso de parametro invalido ou falha de alocacao.
 */
int svg_origens_add(SvgOrigens origens, const char *registrador,
                    double x, double y);

/**
 * @brief Libera a lista de origens desenhaveis.
 *
 * @param origens Lista criada por svg_origens_create.
 */
void svg_origens_destroy(SvgOrigens origens);

/**
 * @brief Gera um SVG base com quadras e sistema viario.
 *
 * Quadras sao desenhadas como retangulos com o estilo lido do .geo. Quando um
 * grafo viario e informado, suas arestas sao desenhadas como linhas
 * direcionadas e seus vertices como pequenos marcadores identificados.
 *
 * @param filepath Caminho do arquivo SVG a ser criado.
 * @param quadras Tabela de quadras carregada do .geo. Pode ser NULL.
 * @param grafo Grafo viario carregado do .via. Pode ser NULL.
 * @return 1 se escreveu o arquivo; 0 em caso de parametro invalido ou erro de
 *         abertura/escrita.
 */
int svg_render_base(const char *filepath, QuadraStore quadras, Grafo grafo);

/**
 * @brief Gera um SVG com quadras, sistema viario e percursos adicionais.
 *
 * @param filepath Caminho do arquivo SVG a ser criado.
 * @param quadras Tabela de quadras carregada do .geo. Pode ser NULL.
 * @param grafo Grafo viario carregado do .via. Pode ser NULL.
 * @param percursos Lista de percursos calculados. Pode ser NULL.
 * @return 1 se escreveu o arquivo; 0 em caso de parametro invalido ou erro de
 *         abertura/escrita.
 */
int svg_render_com_percursos(const char *filepath, QuadraStore quadras,
                             Grafo grafo, SvgPercursos percursos);

/**
 * @brief Gera um SVG com quadras, vias, percursos, regioes e marcadores.
 *
 * @param filepath Caminho do arquivo SVG a ser criado.
 * @param quadras Tabela de quadras carregada do .geo. Pode ser NULL.
 * @param grafo Grafo viario carregado do .via. Pode ser NULL.
 * @param percursos Lista de percursos calculados. Pode ser NULL.
 * @param regioes Lista de regioes destacadas. Pode ser NULL.
 * @param expansoes Lista de arestas expandidas. Pode ser NULL.
 * @param origens Lista de origens destacadas. Pode ser NULL.
 * @param mvm_regioes Lista de regioes alteradas por mvm. Pode ser NULL.
 * @return 1 se escreveu o arquivo; 0 em caso de parametro invalido ou erro de
 *         abertura/escrita.
 */
int svg_render_com_anotacoes(const char *filepath, QuadraStore quadras,
                             Grafo grafo, SvgPercursos percursos,
                             SvgRegioes regioes, SvgExpansoes expansoes,
                             SvgOrigens origens, SvgMvmRegioes mvm_regioes);

#endif
