#include "unity.h"
#include "caminho.h"
#include "svg.h"
#include "grafo.h"
#include "quadra.h"
#include "quadra_store.h"

#include <stdio.h>
#include <string.h>

#define TEST_SVG_FILE "test_svg.svg"

static int file_contains(const char *pattern) {
    FILE *file = fopen(TEST_SVG_FILE, "r");
    char line[512];
    int found = 0;

    TEST_ASSERT_NOT_NULL(file);

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, pattern) != NULL) {
            found = 1;
            break;
        }
    }

    fclose(file);
    return found;
}

void setUp(void) {
    remove(TEST_SVG_FILE);
}

void tearDown(void) {
    remove(TEST_SVG_FILE);
}

void test_svg_render_base_cria_documento_com_quadras_e_vias(void) {
    QuadraStore quadras = quadra_store_create(1);
    Grafo grafo = grafo_create(2);
    GrafoVertice origem;
    GrafoVertice destino;
    Quadra quadra = quadra_create("cep1", 10.0, 20.0, 30.0, 40.0,
                                  2.0, "white", "black");

    TEST_ASSERT_NOT_NULL(quadras);
    TEST_ASSERT_NOT_NULL(grafo);
    TEST_ASSERT_NOT_NULL(quadra);
    TEST_ASSERT_EQUAL_INT(1, quadra_store_insert(quadras, quadra));

    origem = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    destino = grafo_add_vertice(grafo, "v2", 100.0, 0.0);
    TEST_ASSERT_NOT_EQUAL(-1, origem);
    TEST_ASSERT_NOT_EQUAL(-1, destino);
    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, origem, destino, "cep1", "-",
                                          100.0, 10.0, "Rua_A"));

    TEST_ASSERT_EQUAL_INT(1, svg_render_base(TEST_SVG_FILE, quadras, grafo));
    TEST_ASSERT_TRUE(file_contains("<svg:svg"));
    TEST_ASSERT_TRUE(file_contains("<svg:g id=\"via\""));
    TEST_ASSERT_TRUE(file_contains("<svg:line"));
    TEST_ASSERT_TRUE(file_contains("<svg:circle id=\"v1\""));
    TEST_ASSERT_TRUE(file_contains("<svg:g id=\"quadras\""));
    TEST_ASSERT_TRUE(file_contains("<svg:rect id=\"cep1\""));
    TEST_ASSERT_TRUE(file_contains(">cep1</svg:text>"));

    grafo_destroy(grafo);
    quadra_store_destroy(quadras);
}

void test_svg_render_base_rejeita_caminho_invalido(void) {
    TEST_ASSERT_EQUAL_INT(0, svg_render_base(NULL, NULL, NULL));
}

void test_svg_render_com_percursos_desenha_caminho_colorido(void) {
    Grafo grafo = grafo_create(2);
    GrafoVertice origem;
    GrafoVertice destino;
    Caminho caminho;
    SvgPercursos percursos = svg_percursos_create();

    TEST_ASSERT_NOT_NULL(grafo);
    TEST_ASSERT_NOT_NULL(percursos);

    origem = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    destino = grafo_add_vertice(grafo, "v2", 100.0, 0.0);
    TEST_ASSERT_NOT_EQUAL(-1, origem);
    TEST_ASSERT_NOT_EQUAL(-1, destino);
    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, origem, destino, "-", "-",
                                          100.0, 10.0, "Rua_A"));

    caminho = caminho_calcular(grafo, origem, destino,
                               CAMINHO_CRITERIO_COMPRIMENTO);
    TEST_ASSERT_NOT_NULL(caminho);
    TEST_ASSERT_EQUAL_INT(1, svg_percursos_add(percursos, caminho, "#ff6600"));

    TEST_ASSERT_EQUAL_INT(1, svg_render_com_percursos(TEST_SVG_FILE, NULL,
                                                      grafo, percursos));
    TEST_ASSERT_TRUE(file_contains(
        "xmlns:xlink=\"http://www.w3.org/1999/xlink\""));
    TEST_ASSERT_TRUE(file_contains("<svg:g id=\"percursos\""));
    TEST_ASSERT_TRUE(file_contains("stroke=\"#ff6600\""));
    TEST_ASSERT_TRUE(file_contains("stroke-width=\"4\""));
    TEST_ASSERT_TRUE(file_contains("<svg:path id=\"percurso_anim_0\""));
    TEST_ASSERT_TRUE(file_contains("<svg:circle r=\"5.000000\""));
    TEST_ASSERT_TRUE(file_contains("<svg:animateMotion"));
    TEST_ASSERT_TRUE(file_contains("repeatCount=\"indefinite\""));
    TEST_ASSERT_TRUE(file_contains("<svg:mpath xlink:href=\"#percurso_anim_0\""));
    TEST_ASSERT_TRUE(file_contains("width=\"18.000000\""));
    TEST_ASSERT_TRUE(file_contains("font-weight=\"bold\""));
    TEST_ASSERT_TRUE(file_contains(">I</svg:text>"));
    TEST_ASSERT_TRUE(file_contains(">F</svg:text>"));

    svg_percursos_destroy(percursos);
    grafo_destroy(grafo);
}

void test_svg_percursos_rejeita_parametros_invalidos(void) {
    SvgPercursos percursos = svg_percursos_create();

    TEST_ASSERT_NOT_NULL(percursos);
    TEST_ASSERT_EQUAL_INT(0, svg_percursos_add(NULL, NULL, "red"));
    TEST_ASSERT_EQUAL_INT(0, svg_percursos_add(percursos, NULL, "red"));
    TEST_ASSERT_EQUAL_INT(0, svg_percursos_add(percursos, NULL, NULL));

    svg_percursos_destroy(percursos);
}

void test_svg_render_com_anotacoes_desenha_regioes_de_regs(void) {
    Grafo grafo = grafo_create(3);
    GrafoVertice v1;
    GrafoVertice v2;
    GrafoVertice v3;
    GrafoComponentes componentes;
    SvgRegioes regioes = svg_regioes_create();

    TEST_ASSERT_NOT_NULL(grafo);
    TEST_ASSERT_NOT_NULL(regioes);

    v1 = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    v2 = grafo_add_vertice(grafo, "v2", 100.0, 0.0);
    v3 = grafo_add_vertice(grafo, "v3", 200.0, 0.0);
    TEST_ASSERT_NOT_EQUAL(-1, v1);
    TEST_ASSERT_NOT_EQUAL(-1, v2);
    TEST_ASSERT_NOT_EQUAL(-1, v3);
    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, v1, v2, "-", "-",
                                          100.0, 2.0, "Rua_Lenta"));
    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, v2, v3, "-", "-",
                                          100.0, 8.0, "Rua_Rapida"));

    componentes = grafo_calcular_componentes_viarios(grafo, 5.0);
    TEST_ASSERT_NOT_NULL(componentes);
    TEST_ASSERT_EQUAL_INT(1, svg_regioes_add_componentes(regioes,
                                                         componentes));
    grafo_componentes_destroy(componentes);

    TEST_ASSERT_EQUAL_INT(1, svg_render_com_anotacoes(TEST_SVG_FILE, NULL,
                                                      grafo, NULL, regioes,
                                                      NULL, NULL, NULL));
    TEST_ASSERT_TRUE(file_contains("<svg:g id=\"regs\""));
    TEST_ASSERT_TRUE(file_contains("fill-opacity=\"0.500000\""));
    TEST_ASSERT_TRUE(file_contains("stroke-width=\"3\""));

    svg_regioes_destroy(regioes);
    grafo_destroy(grafo);
}

void test_svg_regioes_rejeita_parametros_invalidos(void) {
    SvgRegioes regioes = svg_regioes_create();

    TEST_ASSERT_NOT_NULL(regioes);
    TEST_ASSERT_EQUAL_INT(0, svg_regioes_add_componentes(NULL, NULL));
    TEST_ASSERT_EQUAL_INT(0, svg_regioes_add_componentes(regioes, NULL));

    svg_regioes_destroy(regioes);
}

void test_svg_render_com_anotacoes_desenha_regioes_de_mvm(void) {
    SvgMvmRegioes regioes = svg_mvm_regioes_create();

    TEST_ASSERT_NOT_NULL(regioes);
    TEST_ASSERT_EQUAL_INT(1, svg_mvm_regioes_add(regioes, 10.0, 20.0,
                                                 30.0, 40.0));

    TEST_ASSERT_EQUAL_INT(1, svg_render_com_anotacoes(TEST_SVG_FILE, NULL,
                                                      NULL, NULL, NULL, NULL,
                                                      NULL, regioes));
    TEST_ASSERT_TRUE(file_contains("<svg:g id=\"mvm\""));
    TEST_ASSERT_TRUE(file_contains("fill=\"red\""));
    TEST_ASSERT_TRUE(file_contains("fill-opacity=\"0.300000\""));
    TEST_ASSERT_TRUE(file_contains("stroke-dasharray=\"3,3\""));

    svg_mvm_regioes_destroy(regioes);
}

void test_svg_render_com_anotacoes_inclui_mvm_no_viewbox(void) {
    QuadraStore quadras = quadra_store_create(1);
    Quadra quadra = quadra_create("cep1", 100.0, 100.0, 10.0, 10.0,
                                  1.0, "white", "black");
    SvgMvmRegioes regioes = svg_mvm_regioes_create();

    TEST_ASSERT_NOT_NULL(quadras);
    TEST_ASSERT_NOT_NULL(quadra);
    TEST_ASSERT_NOT_NULL(regioes);
    TEST_ASSERT_EQUAL_INT(1, quadra_store_insert(quadras, quadra));
    TEST_ASSERT_EQUAL_INT(1, svg_mvm_regioes_add(regioes, 0.0, 0.0,
                                                 20.0, 20.0));

    TEST_ASSERT_EQUAL_INT(1, svg_render_com_anotacoes(TEST_SVG_FILE, quadras,
                                                      NULL, NULL, NULL, NULL,
                                                      NULL, regioes));
    TEST_ASSERT_TRUE(file_contains(
        "viewBox=\"-10.000000 -10.000000 130.000000 130.000000\""));

    svg_mvm_regioes_destroy(regioes);
    quadra_store_destroy(quadras);
}

void test_svg_mvm_regioes_rejeita_parametros_invalidos(void) {
    SvgMvmRegioes regioes = svg_mvm_regioes_create();

    TEST_ASSERT_NOT_NULL(regioes);
    TEST_ASSERT_EQUAL_INT(0, svg_mvm_regioes_add(NULL, 0.0, 0.0, 1.0, 1.0));
    TEST_ASSERT_EQUAL_INT(0, svg_mvm_regioes_add(regioes, 0.0, 0.0,
                                                 -1.0, 1.0));
    TEST_ASSERT_EQUAL_INT(0, svg_mvm_regioes_add(regioes, 0.0, 0.0,
                                                 1.0, -1.0));

    svg_mvm_regioes_destroy(regioes);
}

void test_svg_render_com_anotacoes_desenha_expansoes(void) {
    Grafo grafo = grafo_create(2);
    GrafoVertice origem;
    GrafoVertice destino;
    GrafoArestas arestas;
    SvgExpansoes expansoes = svg_expansoes_create();

    TEST_ASSERT_NOT_NULL(grafo);
    TEST_ASSERT_NOT_NULL(expansoes);

    origem = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    destino = grafo_add_vertice(grafo, "v2", 100.0, 0.0);
    TEST_ASSERT_NOT_EQUAL(-1, origem);
    TEST_ASSERT_NOT_EQUAL(-1, destino);
    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, origem, destino, "-", "-",
                                          100.0, 2.0, "Rua_Lenta"));

    arestas = grafo_aplicar_expansao_agm(grafo, 5.0);
    TEST_ASSERT_NOT_NULL(arestas);
    TEST_ASSERT_EQUAL_INT(1, svg_expansoes_add_arestas(expansoes, grafo,
                                                       arestas));
    grafo_arestas_destroy(arestas);

    TEST_ASSERT_EQUAL_INT(1, svg_render_com_anotacoes(TEST_SVG_FILE, NULL,
                                                      grafo, NULL, NULL,
                                                      expansoes, NULL, NULL));
    TEST_ASSERT_TRUE(file_contains("<svg:g id=\"exp\""));
    TEST_ASSERT_TRUE(file_contains("stroke=\"red\""));
    TEST_ASSERT_TRUE(file_contains("stroke-width=\"5\""));

    svg_expansoes_destroy(expansoes);
    grafo_destroy(grafo);
}

void test_svg_render_com_anotacoes_desenha_origens(void) {
    SvgOrigens origens = svg_origens_create();

    TEST_ASSERT_NOT_NULL(origens);
    TEST_ASSERT_EQUAL_INT(1, svg_origens_add(origens, "R3", 25.0, 40.0));

    TEST_ASSERT_EQUAL_INT(1, svg_render_com_anotacoes(TEST_SVG_FILE, NULL,
                                                      NULL, NULL, NULL, NULL,
                                                      origens, NULL));
    TEST_ASSERT_TRUE(file_contains("<svg:g id=\"origens\""));
    TEST_ASSERT_TRUE(file_contains("stroke-dasharray=\"5,5\""));
    TEST_ASSERT_TRUE(file_contains(">R3</svg:text>"));

    svg_origens_destroy(origens);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_svg_render_base_cria_documento_com_quadras_e_vias);
    RUN_TEST(test_svg_render_base_rejeita_caminho_invalido);
    RUN_TEST(test_svg_render_com_percursos_desenha_caminho_colorido);
    RUN_TEST(test_svg_percursos_rejeita_parametros_invalidos);
    RUN_TEST(test_svg_render_com_anotacoes_desenha_regioes_de_regs);
    RUN_TEST(test_svg_regioes_rejeita_parametros_invalidos);
    RUN_TEST(test_svg_render_com_anotacoes_desenha_regioes_de_mvm);
    RUN_TEST(test_svg_render_com_anotacoes_inclui_mvm_no_viewbox);
    RUN_TEST(test_svg_mvm_regioes_rejeita_parametros_invalidos);
    RUN_TEST(test_svg_render_com_anotacoes_desenha_expansoes);
    RUN_TEST(test_svg_render_com_anotacoes_desenha_origens);
    return UNITY_END();
}
