#include "config.h"
#include "geo_parser.h"
#include "grafo.h"
#include "qry_executor.h"
#include "qry_parser.h"
#include "quadra_store.h"
#include "registradores.h"
#include "svg.h"
#include "via_parser.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

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

static const char *filename_part(const char *path) {
    const char *last_slash;
    const char *last_backslash;

    if (path == NULL) {
        return NULL;
    }

    last_slash = strrchr(path, '/');
    last_backslash = strrchr(path, '\\');
    if (last_slash == NULL && last_backslash == NULL) {
        return path;
    }
    if (last_backslash == NULL || last_slash > last_backslash) {
        return last_slash + 1;
    }
    return last_backslash + 1;
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

static char *make_name_without_extension(const char *filepath) {
    const char *name = filename_part(filepath);
    const char *extension;
    char *base_name;
    size_t base_length;

    if (name == NULL || name[0] == '\0') {
        return NULL;
    }

    extension = strrchr(name, '.');
    base_length = extension != NULL && extension != name
                      ? (size_t)(extension - name)
                      : strlen(name);

    base_name = (char *)malloc(base_length + 1);
    if (base_name == NULL) {
        return NULL;
    }

    memcpy(base_name, name, base_length);
    base_name[base_length] = '\0';
    return base_name;
}

static char *make_svg_filename(const char *geo_file, const char *qry_file) {
    char *geo_name;
    char *qry_name = NULL;
    char *svg_name;
    size_t length;

    geo_name = make_name_without_extension(geo_file);
    if (geo_name == NULL) {
        return NULL;
    }

    if (qry_file != NULL) {
        qry_name = make_name_without_extension(qry_file);
        if (qry_name == NULL) {
            free(geo_name);
            return NULL;
        }
    }

    length = strlen(geo_name) + strlen(".svg") + 1;
    if (qry_name != NULL) {
        length += strlen(qry_name) + 1;
    }

    svg_name = (char *)malloc(length);
    if (svg_name == NULL) {
        free(qry_name);
        free(geo_name);
        return NULL;
    }

    if (qry_name != NULL) {
        sprintf(svg_name, "%s-%s.svg", geo_name, qry_name);
    } else {
        sprintf(svg_name, "%s.svg", geo_name);
    }

    free(qry_name);
    free(geo_name);
    return svg_name;
}

static int ensure_output_dir(const char *path) {
    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    if (mkdir(path, 0775) == 0) {
        return 1;
    }

    return errno == EEXIST;
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

static int gerar_svg_base(const Config config, QuadraStore quadras, Grafo grafo,
                          SvgPercursos percursos) {
    char *svg_name;
    char *svg_path;
    int ok;

    if (!ensure_output_dir(config_get_output_dir(config))) {
        fprintf(stderr, "ted: nao foi possivel criar/acessar diretorio de saida: %s\n",
                config_get_output_dir(config));
        return 0;
    }

    svg_name = make_svg_filename(config_get_geo_file(config),
                                 config_get_qry_file(config));
    if (svg_name == NULL) {
        fprintf(stderr, "ted: nao foi possivel definir o nome do svg\n");
        return 0;
    }

    svg_path = join_path(config_get_output_dir(config), svg_name);
    free(svg_name);
    if (svg_path == NULL) {
        fprintf(stderr, "ted: nao foi possivel montar o caminho do svg\n");
        return 0;
    }

    ok = svg_render_com_percursos(svg_path, quadras, grafo, percursos);
    if (!ok) {
        fprintf(stderr, "ted: erro ao escrever svg: %s\n", svg_path);
    }

    free(svg_path);
    return ok;
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

static int adicionar_percurso_svg(QryComando comando, Grafo grafo,
                                  Registradores registradores,
                                  SvgPercursos percursos) {
    Caminho curto = NULL;
    Caminho rapido = NULL;

    if (grafo == NULL ||
        !qry_executor_calcular_percurso(comando, grafo, registradores,
                                        &curto, &rapido)) {
        return 0;
    }

    if (!svg_percursos_add(percursos, curto,
                           qry_comando_get_cor_curto(comando))) {
        caminho_destroy(curto);
        caminho_destroy(rapido);
        return 0;
    }
    curto = NULL;

    if (!svg_percursos_add(percursos, rapido,
                           qry_comando_get_cor_rapido(comando))) {
        caminho_destroy(rapido);
        return 0;
    }

    return 1;
}

static int executar_qry_para_svg(QryComandos comandos, QuadraStore quadras,
                                 Grafo grafo, Registradores registradores,
                                 SvgPercursos percursos) {
    size_t count;

    if (comandos == NULL) {
        return 1;
    }
    if (quadras == NULL || registradores == NULL || percursos == NULL) {
        return 0;
    }

    count = qry_comandos_count(comandos);
    for (size_t i = 0; i < count; i++) {
        QryComando comando = qry_comandos_get(comandos, i);
        QryComandoTipo tipo;

        if (comando == NULL) {
            return 0;
        }

        tipo = qry_comando_get_tipo(comando);
        if (tipo == QRY_COMANDO_ORIGEM) {
            if (!qry_executor_resolve_origem(comando, quadras,
                                             registradores)) {
                return 0;
            }
        } else if (tipo == QRY_COMANDO_PERCURSO &&
                   !adicionar_percurso_svg(comando, grafo, registradores,
                                           percursos)) {
            return 0;
        }
    }

    return 1;
}

int main(int argc, char *argv[]) {
    Config config;
    QuadraStore quadras = NULL;
    Grafo grafo = NULL;
    QryComandos comandos = NULL;
    Registradores registradores = NULL;
    SvgPercursos percursos = NULL;
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

    if (comandos != NULL) {
        registradores = registradores_create();
        if (registradores == NULL) {
            fprintf(stderr, "ted: erro de memoria ao criar registradores\n");
            goto cleanup;
        }

        percursos = svg_percursos_create();
        if (percursos == NULL) {
            fprintf(stderr, "ted: erro de memoria ao criar percursos do svg\n");
            goto cleanup;
        }

        if (!executar_qry_para_svg(comandos, quadras, grafo, registradores,
                                   percursos)) {
            fprintf(stderr, "ted: erro ao executar consultas do qry\n");
            goto cleanup;
        }
    }

    if (!gerar_svg_base(config, quadras, grafo, percursos)) {
        goto cleanup;
    }

    status = 0;

cleanup:
    svg_percursos_destroy(percursos);
    registradores_destroy(registradores);
    qry_comandos_destroy(comandos);
    grafo_destroy(grafo);
    quadra_store_destroy(quadras);
    config_destroy(config);
    return status;
}
