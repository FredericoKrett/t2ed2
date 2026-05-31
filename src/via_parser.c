#include "via_parser.h"

#include <stdio.h>
#include <string.h>

#define LINE_SIZE 512
#define TOKEN_SIZE 128

static int is_blank_line(const char *line) {
    while (*line != '\0') {
        if (*line != ' ' && *line != '\t' && *line != '\n' && *line != '\r') {
            return 0;
        }
        line++;
    }
    return 1;
}

static int parse_vertex_line(const char *line, Grafo grafo) {
    char id[TOKEN_SIZE];
    char extra[TOKEN_SIZE];
    double x;
    double y;
    int read = sscanf(line, "%*s %127s %lf %lf %127s", id, &x, &y, extra);

    if (read != 3) {
        return 0;
    }

    return grafo_add_vertice(grafo, id, x, y) >= 0;
}

static int parse_edge_line(const char *line, Grafo grafo) {
    char origem_id[TOKEN_SIZE];
    char destino_id[TOKEN_SIZE];
    char ldir[TOKEN_SIZE];
    char lesq[TOKEN_SIZE];
    char nome[TOKEN_SIZE];
    char extra[TOKEN_SIZE];
    double cmp;
    double vm;
    GrafoVertice origem;
    GrafoVertice destino;
    int read = sscanf(line, "%*s %127s %127s %127s %127s %lf %lf %127s %127s",
                      origem_id, destino_id, ldir, lesq, &cmp, &vm, nome, extra);

    if (read != 7) {
        return 0;
    }

    origem = grafo_find_vertice(grafo, origem_id);
    destino = grafo_find_vertice(grafo, destino_id);
    if (origem < 0 || destino < 0) {
        return 0;
    }

    return grafo_add_aresta(grafo, origem, destino, ldir, lesq, cmp, vm, nome) != NULL;
}

static int read_vertex_capacity(FILE *file, size_t *out_capacity) {
    char line[LINE_SIZE];
    long vertex_count;
    char extra[TOKEN_SIZE];
    int read;

    if (fgets(line, sizeof(line), file) == NULL) {
        return 0;
    }

    read = sscanf(line, "%ld %127s", &vertex_count, extra);
    if (read != 1 || vertex_count < 0) {
        return 0;
    }

    *out_capacity = (size_t)vertex_count;
    return 1;
}

Grafo via_parser_parse_file(const char *filepath) {
    FILE *file;
    Grafo grafo;
    char line[LINE_SIZE];
    size_t capacity;
    int edge_section_started = 0;

    if (filepath == NULL) {
        return NULL;
    }

    file = fopen(filepath, "r");
    if (file == NULL) {
        return NULL;
    }

    if (!read_vertex_capacity(file, &capacity)) {
        fclose(file);
        return NULL;
    }

    grafo = grafo_create(capacity);
    if (grafo == NULL) {
        fclose(file);
        return NULL;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char command[TOKEN_SIZE];

        if (is_blank_line(line)) {
            continue;
        }

        if (sscanf(line, "%127s", command) != 1) {
            grafo_destroy(grafo);
            fclose(file);
            return NULL;
        }

        if (strcmp(command, "v") == 0) {
            if (edge_section_started || !parse_vertex_line(line, grafo)) {
                grafo_destroy(grafo);
                fclose(file);
                return NULL;
            }
        } else if (strcmp(command, "e") == 0) {
            edge_section_started = 1;
            if (!parse_edge_line(line, grafo)) {
                grafo_destroy(grafo);
                fclose(file);
                return NULL;
            }
        } else {
            grafo_destroy(grafo);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    return grafo;
}
