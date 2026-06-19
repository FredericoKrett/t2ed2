#include "unity.h"
#include "via_parser.h"

#include <stdio.h>
#include <stdlib.h>

#define TEST_VIA_FILE "test_via_parser.via"

struct edge_probe {
    int count;
    GrafoAresta first;
};

void setUp(void) {
    remove(TEST_VIA_FILE);
}

void tearDown(void) {
    remove(TEST_VIA_FILE);
}

static void write_file(const char *content) {
    FILE *file = fopen(TEST_VIA_FILE, "w");
    TEST_ASSERT_NOT_NULL(file);
    fputs(content, file);
    fclose(file);
}

static void capture_edge(Grafo grafo, GrafoAresta aresta, void *context) {
    struct edge_probe *probe = (struct edge_probe *)context;

    TEST_ASSERT_NOT_NULL(grafo);
    TEST_ASSERT_NOT_NULL(aresta);
    probe->count++;
    if (probe->first == NULL) {
        probe->first = aresta;
    }
}

void test_via_parser_parse_file_carrega_vertices_e_arestas(void) {
    Grafo grafo;
    GrafoVertice v1;
    GrafoVertice v2;
    struct edge_probe probe = {0, NULL};

    write_file(
        "2\n"
        "v v1 10.0 20.0\n"
        "v v2 30.0 40.0\n"
        "e v1 v2 cep_dir cep_esq 120.0 8.0 Rua_A\n"
    );

    grafo = via_parser_parse_file(TEST_VIA_FILE);

    TEST_ASSERT_NOT_NULL(grafo);
    TEST_ASSERT_EQUAL_INT(2, (int)grafo_get_vertice_count(grafo));
    TEST_ASSERT_EQUAL_INT(1, (int)grafo_get_aresta_count(grafo));

    v1 = grafo_find_vertice(grafo, "v1");
    v2 = grafo_find_vertice(grafo, "v2");

    TEST_ASSERT_TRUE(v1 >= 0);
    TEST_ASSERT_TRUE(v2 >= 0);
    TEST_ASSERT_EQUAL_INT(1, (int)grafo_get_out_degree(grafo, v1));
    TEST_ASSERT_EQUAL_INT(0, (int)grafo_get_out_degree(grafo, v2));

    grafo_for_each_aresta_saida(grafo, v1, capture_edge, &probe);

    TEST_ASSERT_EQUAL_INT(1, probe.count);
    TEST_ASSERT_NOT_NULL(probe.first);
    TEST_ASSERT_EQUAL_INT(v1, grafo_aresta_get_origem(probe.first));
    TEST_ASSERT_EQUAL_INT(v2, grafo_aresta_get_destino(probe.first));
    TEST_ASSERT_EQUAL_STRING("cep_dir", grafo_aresta_get_ldir(probe.first));
    TEST_ASSERT_EQUAL_STRING("cep_esq", grafo_aresta_get_lesq(probe.first));
    TEST_ASSERT_EQUAL_STRING("Rua_A", grafo_aresta_get_nome(probe.first));

    grafo_destroy(grafo);
}

void test_via_parser_parse_file_rejeita_arquivo_inexistente(void) {
    TEST_ASSERT_NULL(via_parser_parse_file("arquivo_inexistente.via"));
}

void test_via_parser_parse_file_rejeita_comando_malformado(void) {
    write_file(
        "1\n"
        "v v1 10.0\n"
    );

    TEST_ASSERT_NULL(via_parser_parse_file(TEST_VIA_FILE));
}

void test_via_parser_parse_file_rejeita_aresta_com_vertice_ausente(void) {
    write_file(
        "1\n"
        "v v1 10.0 20.0\n"
        "e v1 v2 - cep1 50.0 5.0 Rua_A\n"
    );

    TEST_ASSERT_NULL(via_parser_parse_file(TEST_VIA_FILE));
}

void test_via_parser_rejeita_quantidade_de_vertices_inconsistente(void) {
    write_file(
        "2\n"
        "v v1 10.0 20.0\n"
    );

    TEST_ASSERT_NULL(via_parser_parse_file(TEST_VIA_FILE));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_via_parser_parse_file_carrega_vertices_e_arestas);
    RUN_TEST(test_via_parser_parse_file_rejeita_arquivo_inexistente);
    RUN_TEST(test_via_parser_parse_file_rejeita_comando_malformado);
    RUN_TEST(test_via_parser_parse_file_rejeita_aresta_com_vertice_ausente);
    RUN_TEST(test_via_parser_rejeita_quantidade_de_vertices_inconsistente);
    return UNITY_END();
}
