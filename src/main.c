#include "config.h"
#include "geo_parser.h"
#include "grafo.h"
#include "qry_parser.h"
#include "quadra_store.h"
#include "via_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *copy_text(const char *text) {
    char *copy;
    size_t length;

    if (text == NULL) {
        return NULL;
    }

    length = strlen(text) + 1;
    copy = (char *)malloc(length);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, text, length);
    return copy;
}

static int is_absolute_path(const char *path) {
    return path != NULL && path[0] == '/';
}

static int has_trailing_separator(const char *path) {
    size_t length;

    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    length = strlen(path);
    return path[length - 1] == '/' || path[length - 1] == '\\';
}

static char *join_path(const char *base_dir, const char *filename) {
    char *path;
    size_t base_length;
    size_t filename_length;
    int needs_separator;

    if (filename == NULL) {
        return NULL;
    }
    if (is_absolute_path(filename) || base_dir == NULL || base_dir[0] == '\0') {
        return copy_text(filename);
    }

    base_length = strlen(base_dir);
    filename_length = strlen(filename);
    needs_separator = !has_trailing_separator(base_dir);

    path = (char *)malloc(base_length + (size_t)needs_separator +
                          filename_length + 1);
    if (path == NULL) {
        return NULL;
    }

    memcpy(path, base_dir, base_length);
    if (needs_separator) {
        path[base_length] = '/';
    }
    memcpy(path + base_length + (size_t)needs_separator,
           filename, filename_length + 1);
    return path;
}

static int carregar_geo(const Config config, QuadraStore quadras) {
    char *geo_path;
    int result;

    geo_path = join_path(config_get_input_dir(config), config_get_geo_file(config));
    if (geo_path == NULL) {
        fprintf(stderr, "ted: nao foi possivel montar o caminho do .geo\n");
        return 0;
    }

    result = geo_parser_parse_file(geo_path, quadras);
    if (result < 0) {
        fprintf(stderr, "ted: erro ao ler arquivo geo: %s\n", geo_path);
        free(geo_path);
        return 0;
    }

    free(geo_path);
    return 1;
}

static Grafo carregar_via(const Config config) {
    const char *via_file = config_get_via_file(config);
    char *via_path;
    Grafo grafo;

    if (via_file == NULL) {
        return NULL;
    }

    via_path = join_path(config_get_input_dir(config), via_file);
    if (via_path == NULL) {
        fprintf(stderr, "ted: nao foi possivel montar o caminho do .via\n");
        return NULL;
    }

    grafo = via_parser_parse_file(via_path);
    if (grafo == NULL) {
        fprintf(stderr, "ted: erro ao ler arquivo via: %s\n", via_path);
    }

    free(via_path);
    return grafo;
}

static QryComandos carregar_qry(const Config config) {
    const char *qry_file = config_get_qry_file(config);
    char *qry_path;
    QryComandos comandos;

    if (qry_file == NULL) {
        return NULL;
    }

    qry_path = join_path(config_get_input_dir(config), qry_file);
    if (qry_path == NULL) {
        fprintf(stderr, "ted: nao foi possivel montar o caminho do .qry\n");
        return NULL;
    }

    comandos = qry_parser_parse_file(qry_path);
    if (comandos == NULL) {
        fprintf(stderr, "ted: erro ao ler arquivo qry: %s\n", qry_path);
    }

    free(qry_path);
    return comandos;
}

int main(int argc, char *argv[]) {
    Config config;
    QuadraStore quadras = NULL;
    Grafo grafo = NULL;
    QryComandos comandos = NULL;
    int status = 1;

    config = config_create();
    if (config == NULL) {
        fprintf(stderr, "ted: erro de memoria ao criar configuracao\n");
        return 1;
    }

    if (!config_parse_args(config, argc, argv)) {
        fprintf(stderr, "uso: ted [-e dir] -f arq.geo [-q arq.qry] "
                        "[-v arq.via] -o dir\n");
        config_destroy(config);
        return 1;
    }

    quadras = quadra_store_create(0);
    if (quadras == NULL) {
        fprintf(stderr, "ted: erro de memoria ao criar tabela de quadras\n");
        goto cleanup;
    }

    if (!carregar_geo(config, quadras)) {
        goto cleanup;
    }

    grafo = carregar_via(config);
    if (config_get_via_file(config) != NULL && grafo == NULL) {
        goto cleanup;
    }

    comandos = carregar_qry(config);
    if (config_get_qry_file(config) != NULL && comandos == NULL) {
        goto cleanup;
    }

    (void)config_get_output_dir(config);
    status = 0;

cleanup:
    qry_comandos_destroy(comandos);
    grafo_destroy(grafo);
    quadra_store_destroy(quadras);
    config_destroy(config);
    return status;
}
