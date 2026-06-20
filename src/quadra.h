#ifndef QUADRA_H
#define QUADRA_H

/**
 * @file quadra.h
 * @brief Contrato do TAD opaco que representa uma quadra de Bitnopolis.
 *
 * O modulo guarda os dados lidos pelo comando "q cep x y w h" e o estilo
 * vigente definido por "cq sw cfill cstrk". A estrutura concreta da quadra
 * fica privada em quadra.c; clientes manipulam apenas o ponteiro opaco Quadra.
 */

/**
 * @brief Instancia opaca de uma quadra.
 */
typedef void *Quadra;

/**
 * @brief Cria uma quadra com geometria e estilo de desenho.
 *
 * @param cep Identificador alfanumerico da quadra.
 * @param x Coordenada x do retangulo.
 * @param y Coordenada y do retangulo.
 * @param w Largura do retangulo.
 * @param h Altura do retangulo.
 * @param sw Espessura da borda vigente.
 * @param cfill Cor de preenchimento vigente.
 * @param cstrk Cor da borda vigente.
 * @return Nova quadra, ou NULL em caso de erro de alocacao, parametro invalido
 *         ou valor numerico nao finito.
 */
Quadra quadra_create(const char *cep, double x, double y, double w, double h,
                     double sw, const char *cfill, const char *cstrk);

/**
 * @brief Libera a quadra e seus dados internos.
 *
 * @param quadra Quadra criada por quadra_create.
 */
void quadra_destroy(Quadra quadra);

/**
 * @brief Retorna o CEP da quadra.
 *
 * @param quadra Quadra consultada.
 * @return CEP armazenado, ou NULL se a quadra for invalida.
 */
const char *quadra_get_cep(Quadra quadra);

/**
 * @brief Retorna a coordenada x da quadra.
 *
 * @param quadra Quadra consultada.
 * @return Coordenada x, ou 0.0 se a quadra for invalida.
 */
double quadra_get_x(Quadra quadra);

/**
 * @brief Retorna a coordenada y da quadra.
 *
 * @param quadra Quadra consultada.
 * @return Coordenada y, ou 0.0 se a quadra for invalida.
 */
double quadra_get_y(Quadra quadra);

/**
 * @brief Retorna a largura da quadra.
 *
 * @param quadra Quadra consultada.
 * @return Largura, ou 0.0 se a quadra for invalida.
 */
double quadra_get_w(Quadra quadra);

/**
 * @brief Retorna a altura da quadra.
 *
 * @param quadra Quadra consultada.
 * @return Altura, ou 0.0 se a quadra for invalida.
 */
double quadra_get_h(Quadra quadra);

/**
 * @brief Retorna a espessura da borda da quadra.
 *
 * @param quadra Quadra consultada.
 * @return Espessura da borda, ou 0.0 se a quadra for invalida.
 */
double quadra_get_sw(Quadra quadra);

/**
 * @brief Retorna a cor de preenchimento da quadra.
 *
 * @param quadra Quadra consultada.
 * @return Cor pertencente a quadra, ou NULL se a quadra for invalida.
 */
const char *quadra_get_cfill(Quadra quadra);

/**
 * @brief Retorna a cor da borda da quadra.
 *
 * @param quadra Quadra consultada.
 * @return Cor pertencente a quadra, ou NULL se a quadra for invalida.
 */
const char *quadra_get_cstrk(Quadra quadra);

/**
 * @brief Calcula o ponto de ancoragem da quadra.
 *
 * O ponto de ancoragem e o canto sudeste descrito no enunciado e serve como
 * referencia para localizar enderecos em uma das quatro faces.
 *
 * @param quadra Quadra consultada.
 * @param out_x Ponteiro opcional que recebera a coordenada x da ancora.
 * @param out_y Ponteiro opcional que recebera a coordenada y da ancora.
 */
void quadra_get_anchor(Quadra quadra, double *out_x, double *out_y);

/**
 * @brief Calcula a coordenada de um endereco localizado em uma face da quadra.
 *
 * @param quadra Quadra consultada.
 * @param face Face do endereco: 'N', 'S', 'L' ou 'O'.
 * @param num Distancia do endereco a partir da referencia da respectiva face.
 * @param out_x Ponteiro opcional que recebera a coordenada x calculada.
 * @param out_y Ponteiro opcional que recebera a coordenada y calculada.
 * @return 1 se a face foi reconhecida; 0 caso contrario.
 */
int quadra_get_address_point(Quadra quadra, char face, double num,
                             double *out_x, double *out_y);

#endif
