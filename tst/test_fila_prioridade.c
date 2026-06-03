#include "unity.h"
#include "fila_prioridade.h"

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

void test_fila_prioridade_create_define_estado_vazio(void) {
    FilaPrioridade fila = fila_prioridade_create(0);
    int valor = -1;
    double prioridade = -1.0;

    TEST_ASSERT_NOT_NULL(fila);
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_is_empty(fila));
    TEST_ASSERT_EQUAL_INT(0, (int)fila_prioridade_size(fila));
    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_peek(fila, &valor, &prioridade));
    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_pop(fila, &valor, &prioridade));

    fila_prioridade_destroy(fila);
}

void test_fila_prioridade_pop_remove_menor_prioridade(void) {
    FilaPrioridade fila = fila_prioridade_create(3);
    int valor = -1;
    double prioridade = -1.0;

    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_push(fila, 10, 30.0));
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_push(fila, 20, 10.0));
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_push(fila, 30, 20.0));
    TEST_ASSERT_EQUAL_INT(3, (int)fila_prioridade_size(fila));
    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_is_empty(fila));

    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_pop(fila, &valor, &prioridade));
    TEST_ASSERT_EQUAL_INT(20, valor);
    assert_double_near(10.0, prioridade);

    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_pop(fila, &valor, &prioridade));
    TEST_ASSERT_EQUAL_INT(30, valor);
    assert_double_near(20.0, prioridade);

    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_pop(fila, &valor, &prioridade));
    TEST_ASSERT_EQUAL_INT(10, valor);
    assert_double_near(30.0, prioridade);
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_is_empty(fila));

    fila_prioridade_destroy(fila);
}

void test_fila_prioridade_peek_nao_remove_elemento(void) {
    FilaPrioridade fila = fila_prioridade_create(2);
    int valor = -1;
    double prioridade = -1.0;

    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_push(fila, 7, 5.0));
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_push(fila, 9, 3.0));

    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_peek(fila, &valor, &prioridade));
    TEST_ASSERT_EQUAL_INT(9, valor);
    assert_double_near(3.0, prioridade);
    TEST_ASSERT_EQUAL_INT(2, (int)fila_prioridade_size(fila));

    valor = -1;
    prioridade = -1.0;
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_pop(fila, &valor, &prioridade));
    TEST_ASSERT_EQUAL_INT(9, valor);
    assert_double_near(3.0, prioridade);

    fila_prioridade_destroy(fila);
}

void test_fila_prioridade_cresce_alem_da_capacidade_inicial(void) {
    FilaPrioridade fila = fila_prioridade_create(1);
    int valor = -1;
    double prioridade = -1.0;

    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_push(fila, 1, 4.0));
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_push(fila, 2, 2.0));
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_push(fila, 3, 6.0));
    TEST_ASSERT_EQUAL_INT(3, (int)fila_prioridade_size(fila));

    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_pop(fila, &valor, &prioridade));
    TEST_ASSERT_EQUAL_INT(2, valor);
    assert_double_near(2.0, prioridade);

    fila_prioridade_destroy(fila);
}

void test_fila_prioridade_rejeita_parametros_invalidos(void) {
    FilaPrioridade fila = fila_prioridade_create(1);
    int valor = -1;
    double prioridade = -1.0;

    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_push(NULL, 1, 1.0));
    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_pop(NULL, &valor, &prioridade));
    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_peek(NULL, &valor, &prioridade));
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_is_empty(NULL));
    TEST_ASSERT_EQUAL_INT(0, (int)fila_prioridade_size(NULL));

    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_pop(fila, NULL, &prioridade));
    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_pop(fila, &valor, NULL));
    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_peek(fila, NULL, &prioridade));
    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_peek(fila, &valor, NULL));

    fila_prioridade_destroy(fila);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_fila_prioridade_create_define_estado_vazio);
    RUN_TEST(test_fila_prioridade_pop_remove_menor_prioridade);
    RUN_TEST(test_fila_prioridade_peek_nao_remove_elemento);
    RUN_TEST(test_fila_prioridade_cresce_alem_da_capacidade_inicial);
    RUN_TEST(test_fila_prioridade_rejeita_parametros_invalidos);
    return UNITY_END();
}
