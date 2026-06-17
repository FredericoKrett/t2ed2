#include "unity.h"
#include "caminho.h"
#include "grafo.h"
#include "qry_executor.h"
#include "qry_parser.h"
#include "quadra.h"
#include "quadra_store.h"
#include "registradores.h"

#include <stdio.h>

#define TEST_QRY_EXECUTOR_FILE "test_qry_executor.qry"
#define EPSILON 0.001

static void assert_double_near(double expected, double actual) {
    double diff = expected - actual;
    if (diff < 0.0) {
        diff = -diff;
    }
    TEST_ASSERT_TRUE(diff <= EPSILON);
}

static void write_file(const char *content) {
    FILE *file = fopen(TEST_QRY_EXECUTOR_FILE, "w");
    TEST_ASSERT_NOT_NULL(file);
    fputs(content, file);
    fclose(file);
}

static QuadraStore criar_quadras_basicas(void) {
    QuadraStore quadras = quadra_store_create(2);
    Quadra quadra = quadra_create("cep1", 10.0, 20.0, 100.0, 80.0,
                                  1.0, "white", "black");

    TEST_ASSERT_NOT_NULL(quadras);
    TEST_ASSERT_NOT_NULL(quadra);
    TEST_ASSERT_EQUAL_INT(1, quadra_store_insert(quadras, quadra));

    return quadras;
}

struct grafo_rotas {
    Grafo grafo;
    GrafoVertice origem;
    GrafoVertice destino;
    GrafoAresta curto_1;
    GrafoAresta curto_2;
    GrafoAresta rapido_1;
    GrafoAresta rapido_2;
};

static struct grafo_rotas criar_grafo_com_rotas(void) {
    struct grafo_rotas rotas;
    GrafoVertice intermediario_curto;
    GrafoVertice intermediario_rapido;

    rotas.grafo = grafo_create(4);
    TEST_ASSERT_NOT_NULL(rotas.grafo);

    rotas.origem = grafo_add_vertice(rotas.grafo, "origem", 0.0, 0.0);
    intermediario_curto = grafo_add_vertice(rotas.grafo, "curto", 10.0, 0.0);
    intermediario_rapido = grafo_add_vertice(rotas.grafo, "rapido", 0.0, 10.0);
    rotas.destino = grafo_add_vertice(rotas.grafo, "destino", 10.0, 10.0);

    TEST_ASSERT_TRUE(rotas.origem >= 0);
    TEST_ASSERT_TRUE(intermediario_curto >= 0);
    TEST_ASSERT_TRUE(intermediario_rapido >= 0);
    TEST_ASSERT_TRUE(rotas.destino >= 0);

    rotas.curto_1 = grafo_add_aresta(rotas.grafo, rotas.origem,
                                     intermediario_curto, "-", "-",
                                     10.0, 1.0, "Rua_Curta_1");
    rotas.curto_2 = grafo_add_aresta(rotas.grafo, intermediario_curto,
                                     rotas.destino, "-", "-",
                                     10.0, 1.0, "Rua_Curta_2");
    rotas.rapido_1 = grafo_add_aresta(rotas.grafo, rotas.origem,
                                      intermediario_rapido, "-", "-",
                                      30.0, 30.0, "Rua_Rapida_1");
    rotas.rapido_2 = grafo_add_aresta(rotas.grafo, intermediario_rapido,
                                      rotas.destino, "-", "-",
                                      30.0, 30.0, "Rua_Rapida_2");

    TEST_ASSERT_NOT_NULL(rotas.curto_1);
    TEST_ASSERT_NOT_NULL(rotas.curto_2);
    TEST_ASSERT_NOT_NULL(rotas.rapido_1);
    TEST_ASSERT_NOT_NULL(rotas.rapido_2);

    return rotas;
}

void setUp(void) {
    remove(TEST_QRY_EXECUTOR_FILE);
}

void tearDown(void) {
    remove(TEST_QRY_EXECUTOR_FILE);
}

void test_qry_executor_resolve_origens_armazena_endereco(void) {
    QuadraStore quadras = criar_quadras_basicas();
    Registradores registradores = registradores_create();
    QryComandos comandos;
    double x = 0.0;
    double y = 0.0;

    write_file(
        "@o? R3 cep1 N 30.0\n"
        "regs 5.0\n"
    );

    comandos = qry_parser_parse_file(TEST_QRY_EXECUTOR_FILE);

    TEST_ASSERT_NOT_NULL(registradores);
    TEST_ASSERT_NOT_NULL(comandos);
    TEST_ASSERT_EQUAL_INT(1, qry_executor_resolve_origens(comandos, quadras,
                                                          registradores));
    TEST_ASSERT_EQUAL_INT(1, registradores_get(registradores, 3, &x, &y));
    assert_double_near(40.0, x);
    assert_double_near(100.0, y);

    qry_comandos_destroy(comandos);
    registradores_destroy(registradores);
    quadra_store_destroy(quadras);
}

void test_qry_executor_resolve_origem_permite_sobrescrever_registrador(void) {
    QuadraStore quadras = criar_quadras_basicas();
    Registradores registradores = registradores_create();
    QryComandos comandos;
    double x = 0.0;
    double y = 0.0;

    write_file(
        "@o? R3 cep1 N 30.0\n"
        "@o? R3 cep1 S 45.0\n"
    );

    comandos = qry_parser_parse_file(TEST_QRY_EXECUTOR_FILE);

    TEST_ASSERT_NOT_NULL(registradores);
    TEST_ASSERT_NOT_NULL(comandos);
    TEST_ASSERT_EQUAL_INT(1, qry_executor_resolve_origem(
                              qry_comandos_get(comandos, 0), quadras,
                              registradores));
    TEST_ASSERT_EQUAL_INT(1, registradores_get(registradores, 3, &x, &y));
    assert_double_near(40.0, x);
    assert_double_near(100.0, y);

    TEST_ASSERT_EQUAL_INT(1, qry_executor_resolve_origem(
                              qry_comandos_get(comandos, 1), quadras,
                              registradores));
    TEST_ASSERT_EQUAL_INT(1, registradores_get(registradores, 3, &x, &y));
    assert_double_near(55.0, x);
    assert_double_near(20.0, y);

    qry_comandos_destroy(comandos);
    registradores_destroy(registradores);
    quadra_store_destroy(quadras);
}

void test_qry_executor_resolve_origens_rejeita_referencias_invalidas(void) {
    QuadraStore quadras = criar_quadras_basicas();
    Registradores registradores = registradores_create();
    QryComandos comandos;

    write_file("@o? R11 cep1 N 30.0\n");
    comandos = qry_parser_parse_file(TEST_QRY_EXECUTOR_FILE);

    TEST_ASSERT_NOT_NULL(registradores);
    TEST_ASSERT_NOT_NULL(comandos);
    TEST_ASSERT_EQUAL_INT(0, qry_executor_resolve_origens(comandos, quadras,
                                                          registradores));

    qry_comandos_destroy(comandos);

    write_file("@o? R2 cep_inexistente N 30.0\n");
    comandos = qry_parser_parse_file(TEST_QRY_EXECUTOR_FILE);

    TEST_ASSERT_NOT_NULL(comandos);
    TEST_ASSERT_EQUAL_INT(0, qry_executor_resolve_origens(comandos, quadras,
                                                          registradores));

    qry_comandos_destroy(comandos);
    registradores_destroy(registradores);
    quadra_store_destroy(quadras);
}

void test_qry_executor_resolve_origens_rejeita_parametros_invalidos(void) {
    QuadraStore quadras = criar_quadras_basicas();
    Registradores registradores = registradores_create();

    TEST_ASSERT_EQUAL_INT(0, qry_executor_resolve_origens(NULL, quadras,
                                                          registradores));
    TEST_ASSERT_EQUAL_INT(0, qry_executor_resolve_origens(NULL, NULL, NULL));

    registradores_destroy(registradores);
    quadra_store_destroy(quadras);
}

void test_qry_executor_aplicar_mvm_atualiza_arestas_na_regiao(void) {
    Grafo grafo = grafo_create(3);
    GrafoVertice v1 = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    GrafoVertice v2 = grafo_add_vertice(grafo, "v2", 10.0, 0.0);
    GrafoVertice v3 = grafo_add_vertice(grafo, "v3", 20.0, 0.0);
    GrafoAresta interna = grafo_add_aresta(grafo, v1, v2, "-", "-",
                                           10.0, 5.0, "Rua_A");
    GrafoAresta externa = grafo_add_aresta(grafo, v2, v3, "-", "-",
                                           10.0, 8.0, "Rua_B");
    QryComandos comandos;

    write_file("mvm 2.5 -1.0 -1.0 12.0 2.0\n");
    comandos = qry_parser_parse_file(TEST_QRY_EXECUTOR_FILE);

    TEST_ASSERT_NOT_NULL(grafo);
    TEST_ASSERT_NOT_NULL(interna);
    TEST_ASSERT_NOT_NULL(externa);
    TEST_ASSERT_NOT_NULL(comandos);
    TEST_ASSERT_EQUAL_INT(1, qry_executor_aplicar_mvm(
                              qry_comandos_get(comandos, 0), grafo));
    assert_double_near(2.5, grafo_aresta_get_vm(interna));
    assert_double_near(8.0, grafo_aresta_get_vm(externa));

    qry_comandos_destroy(comandos);
    grafo_destroy(grafo);
}

void test_qry_executor_calcular_regs_retorna_componentes_lentos(void) {
    Grafo grafo = grafo_create(3);
    GrafoVertice v1 = grafo_add_vertice(grafo, "v1", 0.0, 0.0);
    GrafoVertice v2 = grafo_add_vertice(grafo, "v2", 10.0, 0.0);
    GrafoVertice v3 = grafo_add_vertice(grafo, "v3", 20.0, 0.0);
    QryComandos comandos;
    GrafoComponentes componentes;
    double x = 0.0;
    double y = 0.0;
    double w = 0.0;
    double h = 0.0;

    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, v1, v2, "-", "-",
                                          10.0, 2.0, "Rua_A"));
    TEST_ASSERT_NOT_NULL(grafo_add_aresta(grafo, v2, v3, "-", "-",
                                          10.0, 8.0, "Rua_B"));

    write_file("regs 5.0\n");
    comandos = qry_parser_parse_file(TEST_QRY_EXECUTOR_FILE);

    TEST_ASSERT_NOT_NULL(comandos);
    componentes = qry_executor_calcular_regs(qry_comandos_get(comandos, 0),
                                             grafo);

    TEST_ASSERT_NOT_NULL(componentes);
    TEST_ASSERT_EQUAL_INT(1, (int)grafo_componentes_count(componentes));
    TEST_ASSERT_EQUAL_INT(1, grafo_componentes_get_bbox(componentes, 0,
                                                        &x, &y, &w, &h));
    assert_double_near(0.0, x);
    assert_double_near(0.0, y);
    assert_double_near(10.0, w);
    assert_double_near(0.0, h);

    grafo_componentes_destroy(componentes);
    qry_comandos_destroy(comandos);
    grafo_destroy(grafo);
}

void test_qry_executor_calcular_percurso_retorna_curto_e_rapido(void) {
    struct grafo_rotas rotas = criar_grafo_com_rotas();
    Registradores registradores = registradores_create();
    QryComandos comandos;
    QryComando comando;
    Caminho curto = NULL;
    Caminho rapido = NULL;

    write_file("p? R1 R2 red blue\n");
    comandos = qry_parser_parse_file(TEST_QRY_EXECUTOR_FILE);
    comando = qry_comandos_get(comandos, 0);

    TEST_ASSERT_NOT_NULL(registradores);
    TEST_ASSERT_EQUAL_INT(1, registradores_set(registradores, 1, 0.0, 0.0));
    TEST_ASSERT_EQUAL_INT(1, registradores_set(registradores, 2, 10.0, 10.0));

    TEST_ASSERT_EQUAL_INT(1, qry_executor_calcular_percurso(
                              comando, rotas.grafo, registradores,
                              &curto, &rapido));

    TEST_ASSERT_NOT_NULL(curto);
    TEST_ASSERT_NOT_NULL(rapido);
    TEST_ASSERT_EQUAL_INT(1, caminho_existe(curto));
    TEST_ASSERT_EQUAL_INT(1, caminho_existe(rapido));
    TEST_ASSERT_EQUAL_INT(CAMINHO_CRITERIO_COMPRIMENTO,
                          caminho_get_criterio(curto));
    TEST_ASSERT_EQUAL_INT(CAMINHO_CRITERIO_TEMPO,
                          caminho_get_criterio(rapido));
    TEST_ASSERT_EQUAL_PTR(rotas.curto_1, caminho_get_aresta(curto, 0));
    TEST_ASSERT_EQUAL_PTR(rotas.curto_2, caminho_get_aresta(curto, 1));
    TEST_ASSERT_EQUAL_PTR(rotas.rapido_1, caminho_get_aresta(rapido, 0));
    TEST_ASSERT_EQUAL_PTR(rotas.rapido_2, caminho_get_aresta(rapido, 1));

    caminho_destroy(curto);
    caminho_destroy(rapido);
    qry_comandos_destroy(comandos);
    registradores_destroy(registradores);
    grafo_destroy(rotas.grafo);
}

void test_qry_executor_calcular_percurso_rejeita_registrador_ausente(void) {
    struct grafo_rotas rotas = criar_grafo_com_rotas();
    Registradores registradores = registradores_create();
    QryComandos comandos;
    Caminho curto = NULL;
    Caminho rapido = NULL;

    write_file("p? R1 R2 red blue\n");
    comandos = qry_parser_parse_file(TEST_QRY_EXECUTOR_FILE);

    TEST_ASSERT_NOT_NULL(comandos);
    TEST_ASSERT_EQUAL_INT(1, registradores_set(registradores, 1, 0.0, 0.0));
    TEST_ASSERT_EQUAL_INT(0, qry_executor_calcular_percurso(
                              qry_comandos_get(comandos, 0), rotas.grafo,
                              registradores, &curto, &rapido));
    TEST_ASSERT_NULL(curto);
    TEST_ASSERT_NULL(rapido);

    qry_comandos_destroy(comandos);
    registradores_destroy(registradores);
    grafo_destroy(rotas.grafo);
}

void test_qry_executor_calcular_percurso_rejeita_comando_invalido(void) {
    struct grafo_rotas rotas = criar_grafo_com_rotas();
    Registradores registradores = registradores_create();
    QryComandos comandos;
    Caminho curto = NULL;
    Caminho rapido = NULL;

    write_file("regs 2.0\n");
    comandos = qry_parser_parse_file(TEST_QRY_EXECUTOR_FILE);

    TEST_ASSERT_NOT_NULL(comandos);
    TEST_ASSERT_EQUAL_INT(0, qry_executor_calcular_percurso(
                              qry_comandos_get(comandos, 0), rotas.grafo,
                              registradores, &curto, &rapido));

    qry_comandos_destroy(comandos);
    registradores_destroy(registradores);
    grafo_destroy(rotas.grafo);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_qry_executor_resolve_origens_armazena_endereco);
    RUN_TEST(test_qry_executor_resolve_origem_permite_sobrescrever_registrador);
    RUN_TEST(test_qry_executor_resolve_origens_rejeita_referencias_invalidas);
    RUN_TEST(test_qry_executor_resolve_origens_rejeita_parametros_invalidos);
    RUN_TEST(test_qry_executor_aplicar_mvm_atualiza_arestas_na_regiao);
    RUN_TEST(test_qry_executor_calcular_regs_retorna_componentes_lentos);
    RUN_TEST(test_qry_executor_calcular_percurso_retorna_curto_e_rapido);
    RUN_TEST(test_qry_executor_calcular_percurso_rejeita_registrador_ausente);
    RUN_TEST(test_qry_executor_calcular_percurso_rejeita_comando_invalido);
    return UNITY_END();
}
