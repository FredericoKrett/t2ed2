#include "unity.h"
#include "quadra_store.h"

void setUp(void) {
}

void tearDown(void) {
}

static Quadra nova_quadra(const char *cep) {
    return quadra_create(cep, 10.0, 20.0, 30.0, 40.0, 1.0,
                         "white", "black");
}

static void contar_quadras(Quadra quadra, void *context) {
    int *count = (int *)context;

    TEST_ASSERT_NOT_NULL(quadra);
    (*count)++;
}

void test_quadra_store_insert_find_e_count(void) {
    QuadraStore store = quadra_store_create(2);
    Quadra q1 = nova_quadra("cep1");
    Quadra q2 = nova_quadra("cep2");

    TEST_ASSERT_NOT_NULL(store);
    TEST_ASSERT_EQUAL_INT(0, (int)quadra_store_count(store));

    TEST_ASSERT_EQUAL_INT(1, quadra_store_insert(store, q1));
    TEST_ASSERT_EQUAL_INT(1, quadra_store_insert(store, q2));

    TEST_ASSERT_EQUAL_INT(2, (int)quadra_store_count(store));
    TEST_ASSERT_EQUAL_PTR(q1, quadra_store_find(store, "cep1"));
    TEST_ASSERT_EQUAL_PTR(q2, quadra_store_find(store, "cep2"));
    TEST_ASSERT_NULL(quadra_store_find(store, "ausente"));

    quadra_store_destroy(store);
}

void test_quadra_store_rejeita_cep_duplicado(void) {
    QuadraStore store = quadra_store_create(1);
    Quadra original = nova_quadra("cep1");
    Quadra duplicada = nova_quadra("cep1");

    TEST_ASSERT_EQUAL_INT(1, quadra_store_insert(store, original));
    TEST_ASSERT_EQUAL_INT(0, quadra_store_insert(store, duplicada));
    TEST_ASSERT_EQUAL_INT(1, (int)quadra_store_count(store));
    TEST_ASSERT_EQUAL_PTR(original, quadra_store_find(store, "cep1"));

    quadra_destroy(duplicada);
    quadra_store_destroy(store);
}

void test_quadra_store_remove_transfere_posse_da_quadra(void) {
    QuadraStore store = quadra_store_create(2);
    Quadra q1 = nova_quadra("cep1");
    Quadra removida;

    TEST_ASSERT_EQUAL_INT(1, quadra_store_insert(store, q1));

    removida = quadra_store_remove(store, "cep1");

    TEST_ASSERT_EQUAL_PTR(q1, removida);
    TEST_ASSERT_EQUAL_INT(0, (int)quadra_store_count(store));
    TEST_ASSERT_NULL(quadra_store_find(store, "cep1"));
    TEST_ASSERT_NULL(quadra_store_remove(store, "cep1"));

    quadra_destroy(removida);
    quadra_store_destroy(store);
}

void test_quadra_store_for_each_visita_todas_as_quadras(void) {
    QuadraStore store = quadra_store_create(2);
    int count = 0;

    TEST_ASSERT_EQUAL_INT(1, quadra_store_insert(store, nova_quadra("cep1")));
    TEST_ASSERT_EQUAL_INT(1, quadra_store_insert(store, nova_quadra("cep2")));
    TEST_ASSERT_EQUAL_INT(1, quadra_store_insert(store, nova_quadra("cep3")));

    quadra_store_for_each(store, contar_quadras, &count);

    TEST_ASSERT_EQUAL_INT(3, count);

    quadra_store_destroy(store);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_quadra_store_insert_find_e_count);
    RUN_TEST(test_quadra_store_rejeita_cep_duplicado);
    RUN_TEST(test_quadra_store_remove_transfere_posse_da_quadra);
    RUN_TEST(test_quadra_store_for_each_visita_todas_as_quadras);
    return UNITY_END();
}
