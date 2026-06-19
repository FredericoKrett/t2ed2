#include "unity.h"
#include "registradores.h"

#define EPSILON 0.001

static void assert_double_near(double expected, double actual) {
    double diff = expected - actual;
    if (diff < 0.0) {
        diff = -diff;
    }
    TEST_ASSERT_TRUE(diff <= EPSILON);
}

void setUp(void) {
}

void tearDown(void) {
}

void test_registradores_parse_nome_aceita_r0_a_r10(void) {
    TEST_ASSERT_EQUAL_INT(0, registradores_parse_nome("R0"));
    TEST_ASSERT_EQUAL_INT(1, registradores_parse_nome("R1"));
    TEST_ASSERT_EQUAL_INT(10, registradores_parse_nome("R10"));
}

void test_registradores_parse_nome_rejeita_invalidos(void) {
    TEST_ASSERT_EQUAL_INT(-1, registradores_parse_nome(NULL));
    TEST_ASSERT_EQUAL_INT(-1, registradores_parse_nome(""));
    TEST_ASSERT_EQUAL_INT(-1, registradores_parse_nome("R"));
    TEST_ASSERT_EQUAL_INT(-1, registradores_parse_nome("R11"));
    TEST_ASSERT_EQUAL_INT(-1, registradores_parse_nome("R4294967296"));
    TEST_ASSERT_EQUAL_INT(-1, registradores_parse_nome("r1"));
    TEST_ASSERT_EQUAL_INT(-1, registradores_parse_nome("A1"));
}

void test_registradores_set_get_e_is_set(void) {
    Registradores registradores = registradores_create();
    double x = 0.0;
    double y = 0.0;

    TEST_ASSERT_NOT_NULL(registradores);
    TEST_ASSERT_EQUAL_INT(0, registradores_is_set(registradores, 3));
    TEST_ASSERT_EQUAL_INT(0, registradores_get(registradores, 3, &x, &y));

    TEST_ASSERT_EQUAL_INT(1, registradores_set(registradores, 3, 12.5, 99.0));
    TEST_ASSERT_EQUAL_INT(1, registradores_is_set(registradores, 3));
    TEST_ASSERT_EQUAL_INT(1, registradores_get(registradores, 3, &x, &y));
    assert_double_near(12.5, x);
    assert_double_near(99.0, y);

    registradores_destroy(registradores);
}

void test_registradores_clear_limpa_posicao(void) {
    Registradores registradores = registradores_create();

    TEST_ASSERT_EQUAL_INT(1, registradores_set(registradores, 0, 1.0, 2.0));
    TEST_ASSERT_EQUAL_INT(1, registradores_clear(registradores, 0));
    TEST_ASSERT_EQUAL_INT(0, registradores_is_set(registradores, 0));
    TEST_ASSERT_EQUAL_INT(0, registradores_clear(registradores, 0));

    registradores_destroy(registradores);
}

void test_registradores_rejeita_indices_invalidos(void) {
    Registradores registradores = registradores_create();
    double x = 0.0;
    double y = 0.0;

    TEST_ASSERT_EQUAL_INT(0, registradores_set(registradores, -1, 1.0, 2.0));
    TEST_ASSERT_EQUAL_INT(0, registradores_set(registradores, REGISTRADORES_TOTAL, 1.0, 2.0));
    TEST_ASSERT_EQUAL_INT(0, registradores_is_set(registradores, -1));
    TEST_ASSERT_EQUAL_INT(0, registradores_get(registradores, REGISTRADORES_TOTAL, &x, &y));
    TEST_ASSERT_EQUAL_INT(0, registradores_clear(registradores, REGISTRADORES_TOTAL));

    registradores_destroy(registradores);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_registradores_parse_nome_aceita_r0_a_r10);
    RUN_TEST(test_registradores_parse_nome_rejeita_invalidos);
    RUN_TEST(test_registradores_set_get_e_is_set);
    RUN_TEST(test_registradores_clear_limpa_posicao);
    RUN_TEST(test_registradores_rejeita_indices_invalidos);
    return UNITY_END();
}
