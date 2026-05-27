#include "unity.h"
#include "quadra.h"

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

void test_quadra_create_guarda_dados_basicos(void) {
    Quadra quadra = quadra_create("cep10", 10.0, 20.0, 100.0, 80.0,
                                  2.5, "lightgray", "black");

    TEST_ASSERT_NOT_NULL(quadra);
    TEST_ASSERT_EQUAL_STRING("cep10", quadra_get_cep(quadra));
    assert_double_near(10.0, quadra_get_x(quadra));
    assert_double_near(20.0, quadra_get_y(quadra));
    assert_double_near(100.0, quadra_get_w(quadra));
    assert_double_near(80.0, quadra_get_h(quadra));
    assert_double_near(2.5, quadra_get_sw(quadra));
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

    assert_double_near(110.0, x);
    assert_double_near(100.0, y);

    quadra_destroy(quadra);
}

void test_quadra_get_address_point_calcula_faces(void) {
    Quadra quadra = quadra_create("cep30", 10.0, 20.0, 100.0, 80.0,
                                  1.0, "white", "blue");
    double x = 0.0;
    double y = 0.0;

    TEST_ASSERT_EQUAL_INT(1, quadra_get_address_point(quadra, 'N', 30.0, &x, &y));
    assert_double_near(40.0, x);
    assert_double_near(100.0, y);

    TEST_ASSERT_EQUAL_INT(1, quadra_get_address_point(quadra, 'S', 30.0, &x, &y));
    assert_double_near(40.0, x);
    assert_double_near(20.0, y);

    TEST_ASSERT_EQUAL_INT(1, quadra_get_address_point(quadra, 'L', 30.0, &x, &y));
    assert_double_near(10.0, x);
    assert_double_near(50.0, y);

    TEST_ASSERT_EQUAL_INT(1, quadra_get_address_point(quadra, 'O', 30.0, &x, &y));
    assert_double_near(110.0, x);
    assert_double_near(50.0, y);

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
