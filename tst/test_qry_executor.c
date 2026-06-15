#include "unity.h"
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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_qry_executor_resolve_origens_armazena_endereco);
    RUN_TEST(test_qry_executor_resolve_origens_rejeita_referencias_invalidas);
    RUN_TEST(test_qry_executor_resolve_origens_rejeita_parametros_invalidos);
    return UNITY_END();
}
