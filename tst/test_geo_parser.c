#include "unity.h"
#include "geo_parser.h"

#include <stdio.h>
#include <stdlib.h>

#define TEST_GEO_FILE "test_geo_parser.geo"

void setUp(void) {
    remove(TEST_GEO_FILE);
}

void tearDown(void) {
    remove(TEST_GEO_FILE);
}

static void write_file(const char *content) {
    FILE *file = fopen(TEST_GEO_FILE, "w");
    TEST_ASSERT_NOT_NULL(file);
    fputs(content, file);
    fclose(file);
}

void test_geo_parser_parse_file_carrega_quadras_com_estilo(void) {
    QuadraStore store = quadra_store_create(4);
    Quadra q1;
    Quadra q2;

    write_file(
        "cq 2.5 lightgray black\n"
        "q cep1 10 20 30 40\n"
        "cq 1.0 white blue\n"
        "q cep2 50 60 70 80\n"
    );

    TEST_ASSERT_EQUAL_INT(2, geo_parser_parse_file(TEST_GEO_FILE, store));
    TEST_ASSERT_EQUAL_INT(2, (int)quadra_store_count(store));

    q1 = quadra_store_find(store, "cep1");
    q2 = quadra_store_find(store, "cep2");

    TEST_ASSERT_NOT_NULL(q1);
    TEST_ASSERT_NOT_NULL(q2);
    TEST_ASSERT_EQUAL_STRING("lightgray", quadra_get_cfill(q1));
    TEST_ASSERT_EQUAL_STRING("black", quadra_get_cstrk(q1));
    TEST_ASSERT_EQUAL_STRING("white", quadra_get_cfill(q2));
    TEST_ASSERT_EQUAL_STRING("blue", quadra_get_cstrk(q2));

    quadra_store_destroy(store);
}

void test_geo_parser_parse_file_rejeita_arquivo_inexistente(void) {
    QuadraStore store = quadra_store_create(1);

    TEST_ASSERT_EQUAL_INT(-1, geo_parser_parse_file("arquivo_inexistente.geo", store));
    TEST_ASSERT_EQUAL_INT(0, (int)quadra_store_count(store));

    quadra_store_destroy(store);
}

void test_geo_parser_parse_file_rejeita_comando_malformado(void) {
    QuadraStore store = quadra_store_create(1);

    write_file(
        "cq 2.0 gray black\n"
        "q cep1 10 20\n"
    );

    TEST_ASSERT_EQUAL_INT(-1, geo_parser_parse_file(TEST_GEO_FILE, store));

    quadra_store_destroy(store);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_geo_parser_parse_file_carrega_quadras_com_estilo);
    RUN_TEST(test_geo_parser_parse_file_rejeita_arquivo_inexistente);
    RUN_TEST(test_geo_parser_parse_file_rejeita_comando_malformado);
    return UNITY_END();
}
