#include "unity.h"
#include "grafo.h"

#define EPSILON 0.001

struct visita_context {
    int chamadas;
    GrafoAresta ultima_aresta;
};

static void assert_double_near(double expected, double actual) {
    double diff = expected - actual;
    if (diff < 0.0) {
        diff = -diff;
    }
    TEST_ASSERT_TRUE(diff <= EPSILON);
}

static void contar_aresta(Grafo grafo, GrafoAresta aresta, void *context) {
    struct visita_context *ctx = (struct visita_context *)context;

    TEST_ASSERT_NOT_NULL(grafo);
    TEST_ASSERT_NOT_NULL(aresta);
    ctx->chamadas++;
    ctx->ultima_aresta = aresta;
}

void setUp(void) {
}

void tearDown(void) {
}

void test_grafo_add_vertice_busca_e_coordenadas(void) {
    Grafo grafo = grafo_create(2);
    GrafoVertice v1;
    GrafoVertice v2;
    double x = 0.0;
    double y = 0.0;

    TEST_ASSERT_NOT_NULL(grafo);

    v1 = grafo_add_vertice(grafo, "v1", 10.0, 20.0);
    v2 = grafo_add_vertice(grafo, "v2", 30.0, 40.0);

    TEST_ASSERT_EQUAL_INT(0, v1);
    TEST_ASSERT_EQUAL_INT(1, v2);
    TEST_ASSERT_EQUAL_INT(2, (int)grafo_get_vertice_count(grafo));
    TEST_ASSERT_EQUAL_INT(v1, grafo_find_vertice(grafo, "v1"));
    TEST_ASSERT_EQUAL_INT(v2, grafo_find_vertice(grafo, "v2"));
    TEST_ASSERT_EQUAL_INT(-1, grafo_find_vertice(grafo, "ausente"));
    TEST_ASSERT_EQUAL_STRING("v1", grafo_get_vertice_id(grafo, v1));

    TEST_ASSERT_EQUAL_INT(1, grafo_get_vertice_coords(grafo, v2, &x, &y));
    assert_double_near(30.0, x);
    assert_double_near(40.0, y);
    TEST_ASSERT_EQUAL_INT(0, grafo_get_vertice_coords(grafo, -1, &x, &y));

    grafo_destroy(grafo);
}

void test_grafo_rejeita_vertice_duplicado(void) {
    Grafo grafo = grafo_create(1);

    TEST_ASSERT_EQUAL_INT(0, grafo_add_vertice(grafo, "v1", 10.0, 20.0));
    TEST_ASSERT_EQUAL_INT(-1, grafo_add_vertice(grafo, "v1", 30.0, 40.0));
    TEST_ASSERT_EQUAL_INT(1, (int)grafo_get_vertice_count(grafo));

    grafo_destroy(grafo);
}

void test_grafo_find_vertice_mais_proximo(void) {
    Grafo grafo = grafo_create(3);
    GrafoVertice v1 = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    GrafoVertice v2 = grafo_add_vertice(grafo, "v2", 10.0, 0.0);
    GrafoVertice v3 = grafo_add_vertice(grafo, "v3", 0.0, 10.0);

    TEST_ASSERT_EQUAL_INT(v2, grafo_find_vertice_mais_proximo(grafo, 8.0, 1.0));
    TEST_ASSERT_EQUAL_INT(v3, grafo_find_vertice_mais_proximo(grafo, 2.0, 9.0));
    TEST_ASSERT_EQUAL_INT(v1, grafo_find_vertice_mais_proximo(grafo, -1.0, -1.0));
    TEST_ASSERT_EQUAL_INT(-1, grafo_find_vertice_mais_proximo(NULL, 0.0, 0.0));

    grafo_destroy(grafo);

    grafo = grafo_create(0);
    TEST_ASSERT_EQUAL_INT(-1, grafo_find_vertice_mais_proximo(grafo, 0.0, 0.0));
    grafo_destroy(grafo);
}

void test_grafo_add_aresta_direcionada_com_atributos(void) {
    Grafo grafo = grafo_create(2);
    GrafoVertice origem = grafo_add_vertice(grafo, "v1", 10.0, 20.0);
    GrafoVertice destino = grafo_add_vertice(grafo, "v2", 30.0, 40.0);
    GrafoAresta aresta;

    aresta = grafo_add_aresta(grafo, origem, destino, "cep_dir", "cep_esq",
                              120.0, 8.0, "Rua_A");

    TEST_ASSERT_NOT_NULL(aresta);
    TEST_ASSERT_EQUAL_INT(1, (int)grafo_get_aresta_count(grafo));
    TEST_ASSERT_EQUAL_INT(1, (int)grafo_get_out_degree(grafo, origem));
    TEST_ASSERT_EQUAL_INT(0, (int)grafo_get_out_degree(grafo, destino));
    TEST_ASSERT_EQUAL_INT(origem, grafo_aresta_get_origem(aresta));
    TEST_ASSERT_EQUAL_INT(destino, grafo_aresta_get_destino(aresta));
    TEST_ASSERT_EQUAL_STRING("cep_dir", grafo_aresta_get_ldir(aresta));
    TEST_ASSERT_EQUAL_STRING("cep_esq", grafo_aresta_get_lesq(aresta));
    TEST_ASSERT_EQUAL_STRING("Rua_A", grafo_aresta_get_nome(aresta));
    assert_double_near(120.0, grafo_aresta_get_cmp(aresta));
    assert_double_near(8.0, grafo_aresta_get_vm(aresta));

    grafo_aresta_set_vm(aresta, 12.5);
    assert_double_near(12.5, grafo_aresta_get_vm(aresta));

    grafo_destroy(grafo);
}

void test_grafo_for_each_aresta_saida_visita_adjacencias(void) {
    Grafo grafo = grafo_create(3);
    GrafoVertice v1 = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    GrafoVertice v2 = grafo_add_vertice(grafo, "v2", 10.0, 0.0);
    GrafoVertice v3 = grafo_add_vertice(grafo, "v3", 20.0, 0.0);
    struct visita_context ctx = {0, NULL};

    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, v1, v2, "-", "cep2", 10.0, 5.0, "Rua_A"));
    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, v1, v3, "cep1", "-", 20.0, 4.0, "Rua_B"));

    grafo_for_each_aresta_saida(grafo, v1, contar_aresta, &ctx);

    TEST_ASSERT_EQUAL_INT(2, ctx.chamadas);
    TEST_ASSERT_NOT_NULL(ctx.ultima_aresta);

    grafo_destroy(grafo);
}

void test_grafo_atualizar_vm_regiao_altera_arestas_internas(void) {
    Grafo grafo = grafo_create(4);
    GrafoVertice v1 = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    GrafoVertice v2 = grafo_add_vertice(grafo, "v2", 10.0, 0.0);
    GrafoVertice v3 = grafo_add_vertice(grafo, "v3", 20.0, 0.0);
    GrafoAresta interna = grafo_add_aresta(grafo, v1, v2, "-", "-",
                                           10.0, 5.0, "Rua_A");
    GrafoAresta externa = grafo_add_aresta(grafo, v2, v3, "-", "-",
                                           10.0, 7.0, "Rua_B");

    TEST_ASSERT_NOT_NULL(interna);
    TEST_ASSERT_NOT_NULL(externa);
    TEST_ASSERT_EQUAL_INT(1, grafo_atualizar_vm_regiao(grafo, 2.5,
                                                       -1.0, -1.0,
                                                       12.0, 2.0));
    assert_double_near(2.5, grafo_aresta_get_vm(interna));
    assert_double_near(7.0, grafo_aresta_get_vm(externa));
    TEST_ASSERT_EQUAL_INT(-1, grafo_atualizar_vm_regiao(NULL, 1.0,
                                                        0.0, 0.0,
                                                        10.0, 10.0));

    grafo_destroy(grafo);
}

void test_grafo_calcular_componentes_lentos_retorna_bboxes(void) {
    Grafo grafo = grafo_create(5);
    GrafoVertice v1 = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    GrafoVertice v2 = grafo_add_vertice(grafo, "v2", 10.0, 0.0);
    GrafoVertice v3 = grafo_add_vertice(grafo, "v3", 10.0, 5.0);
    GrafoVertice v4 = grafo_add_vertice(grafo, "v4", 100.0, 100.0);
    GrafoVertice v5 = grafo_add_vertice(grafo, "v5", 110.0, 100.0);
    GrafoComponentes componentes;
    double x = 0.0;
    double y = 0.0;
    double w = 0.0;
    double h = 0.0;

    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, v1, v2, "-", "-",
                                          10.0, 2.0, "Rua_A"));
    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, v2, v3, "-", "-",
                                          10.0, 3.0, "Rua_B"));
    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, v3, v4, "-", "-",
                                          10.0, 8.0, "Rua_C"));
    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, v4, v5, "-", "-",
                                          10.0, 1.0, "Rua_D"));

    componentes = grafo_calcular_componentes_lentos(grafo, 5.0);

    TEST_ASSERT_NOT_NULL(componentes);
    TEST_ASSERT_EQUAL_INT(2, (int)grafo_componentes_count(componentes));
    TEST_ASSERT_EQUAL_INT(1, grafo_componentes_get_bbox(componentes, 0,
                                                        &x, &y, &w, &h));
    assert_double_near(0.0, x);
    assert_double_near(0.0, y);
    assert_double_near(10.0, w);
    assert_double_near(5.0, h);
    TEST_ASSERT_EQUAL_INT(1, grafo_componentes_get_bbox(componentes, 1,
                                                        &x, &y, &w, &h));
    assert_double_near(100.0, x);
    assert_double_near(100.0, y);
    assert_double_near(10.0, w);
    assert_double_near(0.0, h);
    TEST_ASSERT_EQUAL_INT(0, grafo_componentes_get_bbox(componentes, 2,
                                                        &x, &y, &w, &h));

    grafo_componentes_destroy(componentes);
    grafo_destroy(grafo);

    TEST_ASSERT_NULL(grafo_calcular_componentes_lentos(NULL, 5.0));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_grafo_add_vertice_busca_e_coordenadas);
    RUN_TEST(test_grafo_rejeita_vertice_duplicado);
    RUN_TEST(test_grafo_find_vertice_mais_proximo);
    RUN_TEST(test_grafo_add_aresta_direcionada_com_atributos);
    RUN_TEST(test_grafo_for_each_aresta_saida_visita_adjacencias);
    RUN_TEST(test_grafo_atualizar_vm_regiao_altera_arestas_internas);
    RUN_TEST(test_grafo_calcular_componentes_lentos_retorna_bboxes);
    return UNITY_END();
}
