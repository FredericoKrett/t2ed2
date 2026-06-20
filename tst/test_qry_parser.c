#include "unity.h"
#include "qry_parser.h"

#include <stdio.h>

#define EPSILON 0.001
#define TEST_QRY_FILE "test_qry_parser.qry"

static void assert_double_near(double expected, double actual) {
    double diff = expected - actual;
    if (diff < 0.0) {
        diff = -diff;
    }
    TEST_ASSERT_TRUE(diff <= EPSILON);
}

static void write_file(const char *content) {
    FILE *file = fopen(TEST_QRY_FILE, "w");
    TEST_ASSERT_NOT_NULL(file);
    fputs(content, file);
    fclose(file);
}

void setUp(void) {
    remove(TEST_QRY_FILE);
}

void tearDown(void) {
    remove(TEST_QRY_FILE);
}

void test_qry_parser_parse_file_carrega_comandos_em_ordem(void) {
    QryComandos comandos;
    QryComando origem;
    QryComando mvm;
    QryComando regs;
    QryComando exp;
    QryComando percurso;

    write_file(
        "@o? R1 cep15 S 45.0\n"
        "mvm 7.5 10.0 20.0 30.0 40.0\n"
        "regs 4.2\n"
        "exp 3.1\n"
        "p? R1 R2 red blue\n"
    );

    comandos = qry_parser_parse_file(TEST_QRY_FILE);

    TEST_ASSERT_NOT_NULL(comandos);
    TEST_ASSERT_EQUAL_INT(5, (int)qry_comandos_count(comandos));

    origem = qry_comandos_get(comandos, 0);
    mvm = qry_comandos_get(comandos, 1);
    regs = qry_comandos_get(comandos, 2);
    exp = qry_comandos_get(comandos, 3);
    percurso = qry_comandos_get(comandos, 4);

    TEST_ASSERT_EQUAL_INT(QRY_COMANDO_ORIGEM, qry_comando_get_tipo(origem));
    TEST_ASSERT_EQUAL_STRING("R1", qry_comando_get_reg1(origem));
    TEST_ASSERT_EQUAL_STRING("cep15", qry_comando_get_cep(origem));
    TEST_ASSERT_EQUAL_INT('S', qry_comando_get_face(origem));
    assert_double_near(45.0, qry_comando_get_num(origem));

    TEST_ASSERT_EQUAL_INT(QRY_COMANDO_MVM, qry_comando_get_tipo(mvm));
    assert_double_near(7.5, qry_comando_get_velocidade(mvm));
    assert_double_near(10.0, qry_comando_get_x(mvm));
    assert_double_near(20.0, qry_comando_get_y(mvm));
    assert_double_near(30.0, qry_comando_get_w(mvm));
    assert_double_near(40.0, qry_comando_get_h(mvm));

    TEST_ASSERT_EQUAL_INT(QRY_COMANDO_REGS, qry_comando_get_tipo(regs));
    assert_double_near(4.2, qry_comando_get_limite_velocidade(regs));

    TEST_ASSERT_EQUAL_INT(QRY_COMANDO_EXP, qry_comando_get_tipo(exp));
    assert_double_near(3.1, qry_comando_get_limite_velocidade(exp));

    TEST_ASSERT_EQUAL_INT(QRY_COMANDO_PERCURSO, qry_comando_get_tipo(percurso));
    TEST_ASSERT_EQUAL_STRING("R1", qry_comando_get_reg1(percurso));
    TEST_ASSERT_EQUAL_STRING("R2", qry_comando_get_reg2(percurso));
    TEST_ASSERT_EQUAL_STRING("red", qry_comando_get_cor_curto(percurso));
    TEST_ASSERT_EQUAL_STRING("blue", qry_comando_get_cor_rapido(percurso));
    TEST_ASSERT_NULL(qry_comandos_get(comandos, 5));

    qry_comandos_destroy(comandos);
}

void test_qry_parser_parse_file_ignora_linhas_vazias(void) {
    QryComandos comandos;

    write_file(
        "\n"
        "   \n"
        "@o? R0 cep1 N 10.0\n"
        "\t\n"
        "regs 3.0\n"
    );

    comandos = qry_parser_parse_file(TEST_QRY_FILE);

    TEST_ASSERT_NOT_NULL(comandos);
    TEST_ASSERT_EQUAL_INT(2, (int)qry_comandos_count(comandos));
    TEST_ASSERT_EQUAL_INT(QRY_COMANDO_ORIGEM,
                          qry_comando_get_tipo(qry_comandos_get(comandos, 0)));
    TEST_ASSERT_EQUAL_INT(QRY_COMANDO_REGS,
                          qry_comando_get_tipo(qry_comandos_get(comandos, 1)));

    qry_comandos_destroy(comandos);
}

void test_qry_parser_parse_file_rejeita_arquivo_inexistente(void) {
    TEST_ASSERT_NULL(qry_parser_parse_file("arquivo_inexistente.qry"));
}

void test_qry_parser_parse_file_rejeita_comando_malformado(void) {
    write_file("@o? R1 cep15 S\n");
    TEST_ASSERT_NULL(qry_parser_parse_file(TEST_QRY_FILE));

    write_file("regs 10.0 extra\n");
    TEST_ASSERT_NULL(qry_parser_parse_file(TEST_QRY_FILE));

    write_file("comando_desconhecido\n");
    TEST_ASSERT_NULL(qry_parser_parse_file(TEST_QRY_FILE));
}

void test_qry_parser_rejeita_valor_nao_finito(void) {
    write_file("mvm nan 10.0 20.0 30.0 40.0\n");

    TEST_ASSERT_NULL(qry_parser_parse_file(TEST_QRY_FILE));
}

void test_qry_parser_rejeita_parametros_invalidos(void) {
    TEST_ASSERT_NULL(qry_parser_parse_file(NULL));
    TEST_ASSERT_EQUAL_INT(0, (int)qry_comandos_count(NULL));
    TEST_ASSERT_NULL(qry_comandos_get(NULL, 0));
    TEST_ASSERT_EQUAL_INT(QRY_COMANDO_ORIGEM, qry_comando_get_tipo(NULL));
    TEST_ASSERT_NULL(qry_comando_get_reg1(NULL));
    TEST_ASSERT_NULL(qry_comando_get_reg2(NULL));
    TEST_ASSERT_NULL(qry_comando_get_cep(NULL));
    TEST_ASSERT_EQUAL_INT('\0', qry_comando_get_face(NULL));
    TEST_ASSERT_NULL(qry_comando_get_cor_curto(NULL));
    TEST_ASSERT_NULL(qry_comando_get_cor_rapido(NULL));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_qry_parser_parse_file_carrega_comandos_em_ordem);
    RUN_TEST(test_qry_parser_parse_file_ignora_linhas_vazias);
    RUN_TEST(test_qry_parser_parse_file_rejeita_arquivo_inexistente);
    RUN_TEST(test_qry_parser_parse_file_rejeita_comando_malformado);
    RUN_TEST(test_qry_parser_rejeita_valor_nao_finito);
    RUN_TEST(test_qry_parser_rejeita_parametros_invalidos);
    return UNITY_END();
}
