#include "unity.h"
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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_svg_render_base_cria_documento_com_quadras_e_vias);
    RUN_TEST(test_svg_render_base_rejeita_caminho_invalido);
    return UNITY_END();
}
