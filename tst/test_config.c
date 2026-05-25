#include "unity.h"
#include "config.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_config_create_define_valores_padrao(void) {
    Config config = config_create();

    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_EQUAL_STRING(".", config_get_input_dir(config));
    TEST_ASSERT_NULL(config_get_geo_file(config));
    TEST_ASSERT_NULL(config_get_qry_file(config));
    TEST_ASSERT_NULL(config_get_via_file(config));
    TEST_ASSERT_NULL(config_get_output_dir(config));

    config_destroy(config);
}

void test_config_parse_args_aceita_parametros_em_qualquer_ordem(void) {
    char *argv[] = {
        "ted",
        "-o", "saida",
        "-v", "mapa.via",
        "-f", "cidade.geo",
        "-q", "consultas.qry",
        "-e", "entrada"
    };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    Config config = config_create();

    TEST_ASSERT_TRUE(config_parse_args(config, argc, argv));
    TEST_ASSERT_EQUAL_STRING("entrada", config_get_input_dir(config));
    TEST_ASSERT_EQUAL_STRING("saida", config_get_output_dir(config));
    TEST_ASSERT_EQUAL_STRING("cidade.geo", config_get_geo_file(config));
    TEST_ASSERT_EQUAL_STRING("consultas.qry", config_get_qry_file(config));
    TEST_ASSERT_EQUAL_STRING("mapa.via", config_get_via_file(config));

    config_destroy(config);
}

void test_config_parse_args_rejeita_obrigatorios_ausentes(void) {
    char *sem_geo[] = {"ted", "-o", "saida"};
    char *sem_saida[] = {"ted", "-f", "cidade.geo"};
    Config config = config_create();

    TEST_ASSERT_FALSE(config_parse_args(config, 3, sem_geo));
    TEST_ASSERT_FALSE(config_parse_args(config, 3, sem_saida));

    config_destroy(config);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_config_create_define_valores_padrao);
    RUN_TEST(test_config_parse_args_aceita_parametros_em_qualquer_ordem);
    RUN_TEST(test_config_parse_args_rejeita_obrigatorios_ausentes);
    return UNITY_END();
}
