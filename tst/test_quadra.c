#include "unity.h"
#include "quadra.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_quadra_create_guarda_dados_basicos(void) {
    Quadra quadra = quadra_create("cep10", 10.0, 20.0, 100.0, 80.0,
                                  2.5, "lightgray", "black");

    TEST_ASSERT_NOT_NULL(quadra);
    TEST_ASSERT_EQUAL_STRING("cep10", quadra_get_cep(quadra));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 10.0, quadra_get_x(quadra));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 20.0, quadra_get_y(quadra));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 100.0, quadra_get_w(quadra));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 80.0, quadra_get_h(quadra));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 2.5, quadra_get_sw(quadra));
    TEST_ASSERT_EQUAL_STRING("lightgray", quadra_get_cfill(quadra));
    TEST_ASSERT_EQUAL_STRING("black", quadra_get_cstrk(quadra));

    quadra_destroy(quadra);
}

void test_quadra_get_anchor_retorna_canto_sudeste(void) {
    Quadra quadra = quadra_create("cep20", 10.0, 20.0, 100.0, 80.0,
                                  1.0, "white", "blue");
    double x = 0.0;
    double y = 0.0;

    quadra_get_anchor(quadra, &x, &y);

    TEST_ASSERT_DOUBLE_WITHIN(0.001, 110.0, x);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 100.0, y);

    quadra_destroy(quadra);
}

void test_quadra_get_address_point_calcula_faces(void) {
    Quadra quadra = quadra_create("cep30", 10.0, 20.0, 100.0, 80.0,
                                  1.0, "white", "blue");
    double x = 0.0;
    double y = 0.0;

    TEST_ASSERT_EQUAL_INT(1, quadra_get_address_point(quadra, 'N', 30.0, &x, &y));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 40.0, x);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 100.0, y);

    TEST_ASSERT_EQUAL_INT(1, quadra_get_address_point(quadra, 'S', 30.0, &x, &y));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 40.0, x);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 20.0, y);

    TEST_ASSERT_EQUAL_INT(1, quadra_get_address_point(quadra, 'L', 30.0, &x, &y));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 10.0, x);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 50.0, y);

    TEST_ASSERT_EQUAL_INT(1, quadra_get_address_point(quadra, 'O', 30.0, &x, &y));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 110.0, x);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 50.0, y);

    TEST_ASSERT_EQUAL_INT(0, quadra_get_address_point(quadra, 'X', 30.0, &x, &y));

    quadra_destroy(quadra);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_quadra_create_guarda_dados_basicos);
    RUN_TEST(test_quadra_get_anchor_retorna_canto_sudeste);
    RUN_TEST(test_quadra_get_address_point_calcula_faces);
    return UNITY_END();
}
