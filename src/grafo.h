#ifndef GRAFO_H
#define GRAFO_H

#include <stddef.h>

/**
 * @file grafo.h
 * @brief Contrato do TAD opaco para o grafo viario direcionado.
 *
 * O sistema viario de Bitnopolis e representado por um grafo direcionado:
 * vertices guardam identificador e coordenadas; arestas guardam os atributos
 * lidos do arquivo .via. A implementacao deve usar listas de adjacencia e
 * manter sua representacao concreta privada em grafo.c.
 */

/**
 * @brief Instancia opaca de grafo direcionado.
 */
typedef void *Grafo;

/**
 * @brief Identificador numerico interno de vertice.
 *
 * Valores negativos representam vertices invalidos.
 */
typedef int GrafoVertice;

/**
 * @brief Instancia opaca de uma aresta pertencente ao grafo.
 */
typedef void *GrafoAresta;

/**
 * @brief Resultado opaco de componentes conexos calculados sobre o grafo.
 */
typedef void *GrafoComponentes;

/**
 * @brief Lista opaca de arestas selecionadas em uma operacao sobre o grafo.
 */
typedef void *GrafoArestas;

/**
 * @brief Funcao chamada ao visitar arestas de saida de um vertice.
 */
typedef void (*GrafoArestaVisitor)(Grafo grafo, GrafoAresta aresta,
                                   void *context);

/**
 * @brief Cria um grafo direcionado vazio.
 *
 * @param capacity_hint Estimativa inicial da quantidade de vertices. Pode ser 0.
 * @return Novo grafo, ou NULL em caso de falha de alocacao.
 */
Grafo grafo_create(size_t capacity_hint);

/**
 * @brief Adiciona um vertice ao grafo.
 *
 * @param grafo Grafo alterado.
 * @param id Identificador textual do vertice, como aparece no .via.
 * @param x Coordenada x do vertice.
 * @param y Coordenada y do vertice.
 * @return Identificador interno do vertice, ou -1 em caso de erro ou id duplicado.
 */
GrafoVertice grafo_add_vertice(Grafo grafo, const char *id, double x, double y);

/**
 * @brief Busca um vertice pelo identificador textual.
 *
 * @param grafo Grafo consultado.
 * @param id Identificador textual procurado.
 * @return Identificador interno do vertice, ou -1 se ausente.
 */
GrafoVertice grafo_find_vertice(Grafo grafo, const char *id);

/**
 * @brief Retorna o identificador textual de um vertice.
 *
 * @param grafo Grafo consultado.
 * @param vertice Identificador interno do vertice.
 * @return String pertencente ao grafo, ou NULL se o vertice for invalido.
 */
const char *grafo_get_vertice_id(Grafo grafo, GrafoVertice vertice);

/**
 * @brief Copia as coordenadas de um vertice.
 *
 * @param grafo Grafo consultado.
 * @param vertice Identificador interno do vertice.
 * @param out_x Ponteiro que recebera a coordenada x.
 * @param out_y Ponteiro que recebera a coordenada y.
 * @return 1 se o vertice existe; 0 caso contrario.
 */
int grafo_get_vertice_coords(Grafo grafo, GrafoVertice vertice,
                             double *out_x, double *out_y);

/**
 * @brief Busca o vertice mais proximo de uma coordenada.
 *
 * Usado para ancorar pontos geograficos, como os registradores de @o?, no
 * sistema viario antes de calcular percursos. A distancia considerada e a
 * euclidiana no plano do SVG.
 *
 * @param grafo Grafo consultado.
 * @param x Coordenada x de referencia.
 * @param y Coordenada y de referencia.
 * @return Vertice mais proximo, ou -1 se o grafo for invalido ou vazio.
 */
GrafoVertice grafo_find_vertice_mais_proximo(Grafo grafo, double x, double y);

/**
 * @brief Adiciona uma aresta direcionada entre dois vertices.
 *
 * @param grafo Grafo alterado.
 * @param origem Vertice de origem.
 * @param destino Vertice de destino.
 * @param ldir CEP da quadra a direita, ou "-".
 * @param lesq CEP da quadra a esquerda, ou "-".
 * @param cmp Comprimento do trecho em metros.
 * @param vm Velocidade media do trecho.
 * @param nome Nome da rua ou avenida.
 * @return Aresta criada, ou NULL em caso de erro.
 */
GrafoAresta grafo_add_aresta(Grafo grafo, GrafoVertice origem,
                             GrafoVertice destino, const char *ldir,
                             const char *lesq, double cmp, double vm,
                             const char *nome);

/**
 * @brief Retorna a quantidade de vertices do grafo.
 */
size_t grafo_get_vertice_count(Grafo grafo);

/**
 * @brief Retorna a quantidade de arestas do grafo.
 */
size_t grafo_get_aresta_count(Grafo grafo);

/**
 * @brief Retorna a quantidade de arestas que saem de um vertice.
 */
size_t grafo_get_out_degree(Grafo grafo, GrafoVertice vertice);

/**
 * @brief Percorre as arestas de saida de um vertice.
 *
 * @param grafo Grafo percorrido.
 * @param origem Vertice de origem.
 * @param visitor Funcao chamada para cada aresta de saida.
 * @param context Contexto repassado ao visitante.
 */
void grafo_for_each_aresta_saida(Grafo grafo, GrafoVertice origem,
                                 GrafoArestaVisitor visitor, void *context);

/**
 * @brief Atualiza a velocidade media das arestas dentro de uma regiao.
 *
 * Considera uma aresta dentro da regiao quando seus vertices de origem e
 * destino pertencem ao retangulo fechado definido por (x, y, w, h). A funcao
 * altera apenas as arestas selecionadas.
 *
 * @param grafo Grafo alterado.
 * @param vm Nova velocidade media.
 * @param x Coordenada x do canto inferior esquerdo da regiao.
 * @param y Coordenada y do canto inferior esquerdo da regiao.
 * @param w Largura da regiao.
 * @param h Altura da regiao.
 * @return Quantidade de arestas atualizadas, ou -1 em caso de parametro
 *         invalido.
 */
int grafo_atualizar_vm_regiao(Grafo grafo, double vm, double x, double y,
                              double w, double h);

/**
 * @brief Calcula componentes formados por arestas com velocidade insuficiente.
 *
 * Seleciona arestas com velocidade media menor que limite_vm e calcula os
 * componentes conexos considerando essas arestas sem direcao. Vertices que nao
 * pertencem a nenhuma aresta selecionada nao formam componentes no resultado.
 *
 * @param grafo Grafo consultado.
 * @param limite_vm Limite de velocidade usado para selecionar arestas lentas.
 * @return Resultado opaco com os componentes, ou NULL em caso de parametro
 *         invalido ou falha de alocacao.
 */
GrafoComponentes grafo_calcular_componentes_lentos(Grafo grafo,
                                                   double limite_vm);

/**
 * @brief Retorna a quantidade de componentes calculados.
 *
 * @param componentes Resultado criado por grafo_calcular_componentes_lentos.
 * @return Quantidade de componentes, ou 0 se o resultado for invalido.
 */
size_t grafo_componentes_count(GrafoComponentes componentes);

/**
 * @brief Copia o retangulo envolvente de um componente.
 *
 * O retangulo envolve os vertices pertencentes ao componente lento.
 *
 * @param componentes Resultado consultado.
 * @param indice Indice do componente.
 * @param out_x Ponteiro que recebera a coordenada x minima.
 * @param out_y Ponteiro que recebera a coordenada y minima.
 * @param out_w Ponteiro que recebera a largura.
 * @param out_h Ponteiro que recebera a altura.
 * @return 1 se o componente existe; 0 caso contrario.
 */
int grafo_componentes_get_bbox(GrafoComponentes componentes, size_t indice,
                               double *out_x, double *out_y,
                               double *out_w, double *out_h);

/**
 * @brief Libera o resultado de componentes conexos.
 *
 * @param componentes Resultado criado por grafo_calcular_componentes_lentos.
 */
void grafo_componentes_destroy(GrafoComponentes componentes);

/**
 * @brief Aplica a expansao de velocidade sobre arestas lentas da AGM.
 *
 * Calcula a arvore geradora minima usando o comprimento das arestas como peso
 * e tratando as ligacoes como nao direcionadas apenas para essa selecao. Entre
 * as arestas da AGM, seleciona aquelas com velocidade media menor que
 * limite_vm, aumenta suas velocidades em 50% e retorna a lista de arestas
 * alteradas. Se o grafo nao for conexo, a selecao corresponde a floresta
 * geradora minima dos componentes existentes.
 *
 * A lista retornada pertence ao chamador e deve ser liberada com
 * grafo_arestas_destroy. As arestas armazenadas pertencem ao grafo original.
 *
 * @param grafo Grafo alterado.
 * @param limite_vm Limite de velocidade usado para selecionar arestas lentas.
 * @return Lista de arestas alteradas, possivelmente vazia, ou NULL em caso de
 *         parametro invalido ou falha de alocacao.
 */
GrafoArestas grafo_aplicar_expansao_agm(Grafo grafo, double limite_vm);

/**
 * @brief Retorna a quantidade de arestas armazenadas na lista.
 *
 * @param arestas Lista consultada.
 * @return Quantidade de arestas, ou 0 se a lista for invalida.
 */
size_t grafo_arestas_count(GrafoArestas arestas);

/**
 * @brief Retorna uma aresta selecionada por indice.
 *
 * @param arestas Lista consultada.
 * @param indice Posicao da aresta.
 * @return Aresta pertencente ao grafo original, ou NULL se o indice for
 *         invalido.
 */
GrafoAresta grafo_arestas_get(GrafoArestas arestas, size_t indice);

/**
 * @brief Libera uma lista de arestas selecionadas.
 *
 * @param arestas Lista criada por grafo_aplicar_expansao_agm.
 */
void grafo_arestas_destroy(GrafoArestas arestas);

/**
 * @brief Retorna o vertice de origem da aresta.
 */
GrafoVertice grafo_aresta_get_origem(GrafoAresta aresta);

/**
 * @brief Retorna o vertice de destino da aresta.
 */
GrafoVertice grafo_aresta_get_destino(GrafoAresta aresta);

/**
 * @brief Retorna o CEP da quadra a direita da aresta.
 */
const char *grafo_aresta_get_ldir(GrafoAresta aresta);

/**
 * @brief Retorna o CEP da quadra a esquerda da aresta.
 */
const char *grafo_aresta_get_lesq(GrafoAresta aresta);

/**
 * @brief Retorna o comprimento da aresta.
 */
double grafo_aresta_get_cmp(GrafoAresta aresta);

/**
 * @brief Retorna a velocidade media da aresta.
 */
double grafo_aresta_get_vm(GrafoAresta aresta);

/**
 * @brief Atualiza a velocidade media da aresta.
 */
void grafo_aresta_set_vm(GrafoAresta aresta, double vm);

/**
 * @brief Retorna o nome da via associada a aresta.
 */
const char *grafo_aresta_get_nome(GrafoAresta aresta);

/**
 * @brief Destroi o grafo e todas as arestas armazenadas.
 *
 * @param grafo Grafo criado por grafo_create.
 */
void grafo_destroy(Grafo grafo);

#endif
