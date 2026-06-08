#include "unity.h"
#include "caminho.h"

#define EPSILON 0.001

struct caminho_fixture {
    Grafo grafo;
    GrafoVertice v1;
    GrafoVertice v2;
    GrafoVertice v3;
    GrafoVertice v4;
    GrafoAresta curto_1;
    GrafoAresta curto_2;
    GrafoAresta rapido_1;
    GrafoAresta rapido_2;
};

static void assert_double_near(double expected, double actual) {
    double diff = expected - actual;
    if (diff < 0.0) {
        diff = -diff;
    }
    TEST_ASSERT_TRUE(diff <= EPSILON);
}

static struct caminho_fixture criar_grafo_com_rotas(void) {
    struct caminho_fixture fixture;

    fixture.grafo = grafo_create(4);
    TEST_ASSERT_NOT_NULL(fixture.grafo);

    fixture.v1 = grafo_add_vertice(fixture.grafo, "v1", 0.0, 0.0);
    fixture.v2 = grafo_add_vertice(fixture.grafo, "v2", 10.0, 0.0);
    fixture.v3 = grafo_add_vertice(fixture.grafo, "v3", 0.0, 10.0);
    fixture.v4 = grafo_add_vertice(fixture.grafo, "v4", 10.0, 10.0);

    TEST_ASSERT_TRUE(fixture.v1 >= 0);
    TEST_ASSERT_TRUE(fixture.v2 >= 0);
    TEST_ASSERT_TRUE(fixture.v3 >= 0);
    TEST_ASSERT_TRUE(fixture.v4 >= 0);

    fixture.curto_1 = grafo_add_aresta(fixture.grafo, fixture.v1, fixture.v2,
                                       "-", "cep1", 10.0, 1.0, "Rua_Curta_1");
    fixture.curto_2 = grafo_add_aresta(fixture.grafo, fixture.v2, fixture.v4,
                                       "cep2", "-", 10.0, 1.0, "Rua_Curta_2");
    fixture.rapido_1 = grafo_add_aresta(fixture.grafo, fixture.v1, fixture.v3,
                                        "-", "cep3", 30.0, 30.0, "Rua_Rapida_1");
    fixture.rapido_2 = grafo_add_aresta(fixture.grafo, fixture.v3, fixture.v4,
                                        "cep4", "-", 30.0, 30.0, "Rua_Rapida_2");

    TEST_ASSERT_NOT_NULL(fixture.curto_1);
    TEST_ASSERT_NOT_NULL(fixture.curto_2);
    TEST_ASSERT_NOT_NULL(fixture.rapido_1);
    TEST_ASSERT_NOT_NULL(fixture.rapido_2);

    return fixture;
}

void setUp(void) {
}

void tearDown(void) {
}

void test_caminho_calcular_por_comprimento_escolhe_menor_distancia(void) {
    struct caminho_fixture fixture = criar_grafo_com_rotas();
    Caminho caminho = caminho_calcular(fixture.grafo, fixture.v1, fixture.v4,
                                       CAMINHO_CRITERIO_COMPRIMENTO);

    TEST_ASSERT_NOT_NULL(caminho);
    TEST_ASSERT_EQUAL_INT(1, caminho_existe(caminho));
    TEST_ASSERT_EQUAL_INT(CAMINHO_CRITERIO_COMPRIMENTO,
                          caminho_get_criterio(caminho));
    TEST_ASSERT_EQUAL_INT(fixture.v1, caminho_get_origem(caminho));
    TEST_ASSERT_EQUAL_INT(fixture.v4, caminho_get_destino(caminho));
    assert_double_near(20.0, caminho_get_custo(caminho));
    TEST_ASSERT_EQUAL_INT(2, (int)caminho_get_aresta_count(caminho));
    TEST_ASSERT_EQUAL_PTR(fixture.curto_1, caminho_get_aresta(caminho, 0));
    TEST_ASSERT_EQUAL_PTR(fixture.curto_2, caminho_get_aresta(caminho, 1));
    TEST_ASSERT_NULL(caminho_get_aresta(caminho, 2));

    caminho_destroy(caminho);
    grafo_destroy(fixture.grafo);
}

void test_caminho_calcular_por_tempo_escolhe_menor_tempo(void) {
    struct caminho_fixture fixture = criar_grafo_com_rotas();
    Caminho caminho = caminho_calcular(fixture.grafo, fixture.v1, fixture.v4,
                                       CAMINHO_CRITERIO_TEMPO);

    TEST_ASSERT_NOT_NULL(caminho);
    TEST_ASSERT_EQUAL_INT(1, caminho_existe(caminho));
    TEST_ASSERT_EQUAL_INT(CAMINHO_CRITERIO_TEMPO, caminho_get_criterio(caminho));
    assert_double_near(2.0, caminho_get_custo(caminho));
    TEST_ASSERT_EQUAL_INT(2, (int)caminho_get_aresta_count(caminho));
    TEST_ASSERT_EQUAL_PTR(fixture.rapido_1, caminho_get_aresta(caminho, 0));
    TEST_ASSERT_EQUAL_PTR(fixture.rapido_2, caminho_get_aresta(caminho, 1));

    caminho_destroy(caminho);
    grafo_destroy(fixture.grafo);
}

void test_caminho_calcular_representa_destino_inalcancavel(void) {
    Grafo grafo = grafo_create(3);
    GrafoVertice v1 = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    GrafoVertice v2 = grafo_add_vertice(grafo, "v2", 10.0, 0.0);
    GrafoVertice v3 = grafo_add_vertice(grafo, "v3", 20.0, 0.0);
    Caminho caminho;

    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, v1, v2, "-", "-", 10.0, 5.0,
                                          "Rua_A"));

    caminho = caminho_calcular(grafo, v1, v3, CAMINHO_CRITERIO_COMPRIMENTO);

    TEST_ASSERT_NOT_NULL(caminho);
    TEST_ASSERT_EQUAL_INT(0, caminho_existe(caminho));
    TEST_ASSERT_EQUAL_INT(v1, caminho_get_origem(caminho));
    TEST_ASSERT_EQUAL_INT(v3, caminho_get_destino(caminho));
    assert_double_near(0.0, caminho_get_custo(caminho));
    TEST_ASSERT_EQUAL_INT(0, (int)caminho_get_aresta_count(caminho));
    TEST_ASSERT_NULL(caminho_get_aresta(caminho, 0));

    caminho_destroy(caminho);
    grafo_destroy(grafo);
}

void test_caminho_calcular_origem_igual_destino_tem_custo_zero(void) {
    Grafo grafo = grafo_create(1);
    GrafoVertice v1 = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    Caminho caminho = caminho_calcular(grafo, v1, v1,
                                       CAMINHO_CRITERIO_COMPRIMENTO);

    TEST_ASSERT_NOT_NULL(caminho);
    TEST_ASSERT_EQUAL_INT(1, caminho_existe(caminho));
    TEST_ASSERT_EQUAL_INT(v1, caminho_get_origem(caminho));
    TEST_ASSERT_EQUAL_INT(v1, caminho_get_destino(caminho));
    assert_double_near(0.0, caminho_get_custo(caminho));
    TEST_ASSERT_EQUAL_INT(0, (int)caminho_get_aresta_count(caminho));

    caminho_destroy(caminho);
    grafo_destroy(grafo);
}

void test_caminho_rejeita_parametros_invalidos(void) {
    Grafo grafo = grafo_create(2);
    GrafoVertice v1 = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    GrafoVertice v2 = grafo_add_vertice(grafo, "v2", 10.0, 0.0);

    TEST_ASSERT_NULL(caminho_calcular(NULL, v1, v2,
                                      CAMINHO_CRITERIO_COMPRIMENTO));
    TEST_ASSERT_NULL(caminho_calcular(grafo, -1, v2,
                                      CAMINHO_CRITERIO_COMPRIMENTO));
    TEST_ASSERT_NULL(caminho_calcular(grafo, v1, -1,
                                      CAMINHO_CRITERIO_COMPRIMENTO));
    TEST_ASSERT_NULL(caminho_calcular(grafo, v1, v2, (CaminhoCriterio)99));
    TEST_ASSERT_EQUAL_INT(0, caminho_existe(NULL));
    TEST_ASSERT_EQUAL_INT(-1, caminho_get_origem(NULL));
    TEST_ASSERT_EQUAL_INT(-1, caminho_get_destino(NULL));
    TEST_ASSERT_EQUAL_INT(CAMINHO_CRITERIO_COMPRIMENTO,
                          caminho_get_criterio(NULL));
    assert_double_near(0.0, caminho_get_custo(NULL));
    TEST_ASSERT_EQUAL_INT(0, (int)caminho_get_aresta_count(NULL));
    TEST_ASSERT_NULL(caminho_get_aresta(NULL, 0));

    grafo_destroy(grafo);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_caminho_calcular_por_comprimento_escolhe_menor_distancia);
    RUN_TEST(test_caminho_calcular_por_tempo_escolhe_menor_tempo);
    RUN_TEST(test_caminho_calcular_representa_destino_inalcancavel);
    RUN_TEST(test_caminho_calcular_origem_igual_destino_tem_custo_zero);
    RUN_TEST(test_caminho_rejeita_parametros_invalidos);
    return UNITY_END();
}
