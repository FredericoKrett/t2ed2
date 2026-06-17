#include "svg.h"

#include "quadra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SVG_MARGIN 10.0
#define SVG_DEFAULT_SIZE 1000.0
#define REGIAO_COR_COUNT 6

static const char *REGIAO_CORES[REGIAO_COR_COUNT] = {
    "#ff0000", "#00aaff", "#ffaa00", "#8a2be2", "#00aa55", "#ff66cc"};

struct svg_bbox {
    double min_x;
    double min_y;
    double max_x;
    double max_y;
    int has_item;
};

struct svg_file_context {
    FILE *file;
    int ok;
};

struct svg_percurso_node {
    Caminho caminho;
    char *cor;
    struct svg_percurso_node *next;
};

struct svg_percursos {
    struct svg_percurso_node *head;
    struct svg_percurso_node *tail;
};

struct svg_regiao_node {
    double x;
    double y;
    double w;
    double h;
    char *cor;
    struct svg_regiao_node *next;
};

struct svg_regioes {
    struct svg_regiao_node *head;
    struct svg_regiao_node *tail;
    size_t count;
};

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

SvgPercursos svg_percursos_create(void) {
    return calloc(1, sizeof(struct svg_percursos));
}

int svg_percursos_add(SvgPercursos percursos_ref, Caminho caminho,
                      const char *cor) {
    struct svg_percursos *percursos = (struct svg_percursos *)percursos_ref;
    struct svg_percurso_node *node;

    if (percursos == NULL || caminho == NULL || cor == NULL) {
        return 0;
    }

    node = (struct svg_percurso_node *)calloc(1,
                                             sizeof(struct svg_percurso_node));
    if (node == NULL) {
        return 0;
    }

    node->cor = copy_text(cor);
    if (node->cor == NULL) {
        free(node);
        return 0;
    }

    node->caminho = caminho;
    if (percursos->tail == NULL) {
        percursos->head = node;
        percursos->tail = node;
    } else {
        percursos->tail->next = node;
        percursos->tail = node;
    }

    return 1;
}

void svg_percursos_destroy(SvgPercursos percursos_ref) {
    struct svg_percursos *percursos = (struct svg_percursos *)percursos_ref;
    struct svg_percurso_node *node;

    if (percursos == NULL) {
        return;
    }

    node = percursos->head;
    while (node != NULL) {
        struct svg_percurso_node *next = node->next;
        caminho_destroy(node->caminho);
        free(node->cor);
        free(node);
        node = next;
    }

    free(percursos);
}

SvgRegioes svg_regioes_create(void) {
    return calloc(1, sizeof(struct svg_regioes));
}

static int svg_regioes_add_rect(SvgRegioes regioes_ref, double x, double y,
                                double w, double h, const char *cor) {
    struct svg_regioes *regioes = (struct svg_regioes *)regioes_ref;
    struct svg_regiao_node *node;

    if (regioes == NULL || cor == NULL || w < 0.0 || h < 0.0) {
        return 0;
    }

    node = (struct svg_regiao_node *)calloc(1, sizeof(struct svg_regiao_node));
    if (node == NULL) {
        return 0;
    }

    node->cor = copy_text(cor);
    if (node->cor == NULL) {
        free(node);
        return 0;
    }

    node->x = x;
    node->y = y;
    node->w = w;
    node->h = h;

    if (regioes->tail == NULL) {
        regioes->head = node;
        regioes->tail = node;
    } else {
        regioes->tail->next = node;
        regioes->tail = node;
    }
    regioes->count++;

    return 1;
}

int svg_regioes_add_componentes(SvgRegioes regioes_ref,
                                GrafoComponentes componentes) {
    struct svg_regioes *regioes = (struct svg_regioes *)regioes_ref;
    size_t count;

    if (regioes == NULL || componentes == NULL) {
        return 0;
    }

    count = grafo_componentes_count(componentes);
    for (size_t i = 0; i < count; i++) {
        double x;
        double y;
        double w;
        double h;
        const char *cor = REGIAO_CORES[regioes->count % REGIAO_COR_COUNT];

        if (!grafo_componentes_get_bbox(componentes, i, &x, &y, &w, &h) ||
            !svg_regioes_add_rect(regioes, x, y, w, h, cor)) {
            return 0;
        }
    }

    return 1;
}

void svg_regioes_destroy(SvgRegioes regioes_ref) {
    struct svg_regioes *regioes = (struct svg_regioes *)regioes_ref;
    struct svg_regiao_node *node;

    if (regioes == NULL) {
        return;
    }

    node = regioes->head;
    while (node != NULL) {
        struct svg_regiao_node *next = node->next;
        free(node->cor);
        free(node);
        node = next;
    }

    free(regioes);
}

static void bbox_include_point(struct svg_bbox *bbox, double x, double y) {
    if (!bbox->has_item) {
        bbox->min_x = x;
        bbox->max_x = x;
        bbox->min_y = y;
        bbox->max_y = y;
        bbox->has_item = 1;
        return;
    }

    if (x < bbox->min_x) {
        bbox->min_x = x;
    }
    if (y < bbox->min_y) {
        bbox->min_y = y;
    }
    if (x > bbox->max_x) {
        bbox->max_x = x;
    }
    if (y > bbox->max_y) {
        bbox->max_y = y;
    }
}

static void bbox_include_rect(struct svg_bbox *bbox,
                              double x, double y, double w, double h) {
    bbox_include_point(bbox, x, y);
    bbox_include_point(bbox, x + w, y + h);
}

static void bbox_include_quadra(Quadra quadra, void *context) {
    struct svg_bbox *bbox = (struct svg_bbox *)context;

    bbox_include_rect(bbox, quadra_get_x(quadra), quadra_get_y(quadra),
                      quadra_get_w(quadra), quadra_get_h(quadra));
}

static void bbox_include_grafo(Grafo grafo, struct svg_bbox *bbox) {
    size_t count;

    if (grafo == NULL) {
        return;
    }

    count = grafo_get_vertice_count(grafo);
    for (size_t i = 0; i < count; i++) {
        double x;
        double y;

        if (grafo_get_vertice_coords(grafo, (GrafoVertice)i, &x, &y)) {
            bbox_include_point(bbox, x, y);
        }
    }
}

static void bbox_finish(struct svg_bbox *bbox) {
    if (!bbox->has_item) {
        bbox->min_x = 0.0;
        bbox->min_y = 0.0;
        bbox->max_x = SVG_DEFAULT_SIZE;
        bbox->max_y = SVG_DEFAULT_SIZE;
        bbox->has_item = 1;
        return;
    }

    bbox->min_x -= SVG_MARGIN;
    bbox->min_y -= SVG_MARGIN;
    bbox->max_x += SVG_MARGIN;
    bbox->max_y += SVG_MARGIN;
}

static void write_escaped(FILE *file, const char *text) {
    if (text == NULL) {
        return;
    }

    while (*text != '\0') {
        if (*text == '&') {
            fputs("&amp;", file);
        } else if (*text == '<') {
            fputs("&lt;", file);
        } else if (*text == '>') {
            fputs("&gt;", file);
        } else if (*text == '"') {
            fputs("&quot;", file);
        } else {
            fputc(*text, file);
        }
        text++;
    }
}

static void draw_caminho(Grafo grafo, Caminho caminho, const char *cor,
                         struct svg_file_context *ctx) {
    size_t count;

    if (grafo == NULL || caminho == NULL || cor == NULL || !ctx->ok ||
        !caminho_existe(caminho)) {
        return;
    }

    count = caminho_get_aresta_count(caminho);
    for (size_t i = 0; i < count; i++) {
        GrafoAresta aresta = caminho_get_aresta(caminho, i);
        double x1;
        double y1;
        double x2;
        double y2;

        if (!grafo_get_vertice_coords(grafo, grafo_aresta_get_origem(aresta),
                                      &x1, &y1) ||
            !grafo_get_vertice_coords(grafo, grafo_aresta_get_destino(aresta),
                                      &x2, &y2)) {
            ctx->ok = 0;
            return;
        }

        fprintf(ctx->file,
                "  <svg:line x1=\"%.6f\" y1=\"%.6f\" x2=\"%.6f\" "
                "y2=\"%.6f\" stroke=\"",
                x1, y1, x2, y2);
        write_escaped(ctx->file, cor);
        fputs("\" stroke-width=\"4\" stroke-opacity=\"1.000000\" />\n",
              ctx->file);

        if (ferror(ctx->file)) {
            ctx->ok = 0;
            return;
        }
    }
}

static void draw_percursos(Grafo grafo, SvgPercursos percursos_ref,
                           struct svg_file_context *ctx) {
    struct svg_percursos *percursos = (struct svg_percursos *)percursos_ref;
    struct svg_percurso_node *node;

    if (grafo == NULL || percursos == NULL || percursos->head == NULL ||
        !ctx->ok) {
        return;
    }

    fputs("<svg:g id=\"percursos\">\n", ctx->file);
    node = percursos->head;
    while (node != NULL) {
        draw_caminho(grafo, node->caminho, node->cor, ctx);
        node = node->next;
    }
    fputs("</svg:g>\n", ctx->file);

    if (ferror(ctx->file)) {
        ctx->ok = 0;
    }
}

static void draw_regioes(SvgRegioes regioes_ref,
                         struct svg_file_context *ctx) {
    struct svg_regioes *regioes = (struct svg_regioes *)regioes_ref;
    struct svg_regiao_node *node;

    if (regioes == NULL || regioes->head == NULL || !ctx->ok) {
        return;
    }

    fputs("<svg:g id=\"regs\">\n", ctx->file);
    node = regioes->head;
    while (node != NULL) {
        fprintf(ctx->file,
                "  <svg:rect x=\"%.6f\" y=\"%.6f\" width=\"%.6f\" "
                "height=\"%.6f\" fill=\"",
                node->x, node->y, node->w, node->h);
        write_escaped(ctx->file, node->cor);
        fputs("\" stroke=\"", ctx->file);
        write_escaped(ctx->file, node->cor);
        fputs("\" stroke-width=\"3\" fill-opacity=\"0.500000\" "
              "stroke-opacity=\"0.900000\" />\n",
              ctx->file);
        node = node->next;
    }
    fputs("</svg:g>\n", ctx->file);

    if (ferror(ctx->file)) {
        ctx->ok = 0;
    }
}

static void draw_quadra(Quadra quadra, void *context) {
    struct svg_file_context *ctx = (struct svg_file_context *)context;
    FILE *file = ctx->file;
    const char *cep = quadra_get_cep(quadra);

    if (!ctx->ok) {
        return;
    }

    fputs("  <svg:rect id=\"", file);
    write_escaped(file, cep);
    fprintf(file,
            "\" x=\"%.6f\" y=\"%.6f\" width=\"%.6f\" height=\"%.6f\" "
            "fill=\"",
            quadra_get_x(quadra), quadra_get_y(quadra),
            quadra_get_w(quadra), quadra_get_h(quadra));
    write_escaped(file, quadra_get_cfill(quadra));
    fputs("\" stroke=\"", file);
    write_escaped(file, quadra_get_cstrk(quadra));
    fprintf(file, "\" stroke-width=\"%.6f\" fill-opacity=\"0.8\" />\n",
            quadra_get_sw(quadra));

    fprintf(file, "  <svg:text x=\"%.6f\" y=\"%.6f\" fill=\"black\" "
                  "font-size=\"10\">",
            quadra_get_x(quadra) + 4.0, quadra_get_y(quadra) + 12.0);
    write_escaped(file, cep);
    fputs("</svg:text>\n", file);

    if (ferror(file)) {
        ctx->ok = 0;
    }
}

static void draw_aresta(Grafo grafo, GrafoAresta aresta, void *context) {
    struct svg_file_context *ctx = (struct svg_file_context *)context;
    double x1;
    double y1;
    double x2;
    double y2;

    if (!ctx->ok) {
        return;
    }

    if (!grafo_get_vertice_coords(grafo, grafo_aresta_get_origem(aresta),
                                  &x1, &y1) ||
        !grafo_get_vertice_coords(grafo, grafo_aresta_get_destino(aresta),
                                  &x2, &y2)) {
        ctx->ok = 0;
        return;
    }

    fprintf(ctx->file,
            "  <svg:line x1=\"%.6f\" y1=\"%.6f\" x2=\"%.6f\" y2=\"%.6f\" "
            "stroke=\"black\" stroke-width=\"1\" marker-end=\"url(#mArrow)\" />\n",
            x1, y1, x2, y2);

    if (ferror(ctx->file)) {
        ctx->ok = 0;
    }
}

static void draw_grafo(Grafo grafo, struct svg_file_context *ctx) {
    size_t count;

    if (grafo == NULL || !ctx->ok) {
        return;
    }

    fputs("<svg:g id=\"via\">\n", ctx->file);

    count = grafo_get_vertice_count(grafo);
    for (size_t i = 0; i < count; i++) {
        grafo_for_each_aresta_saida(grafo, (GrafoVertice)i, draw_aresta, ctx);
    }

    for (size_t i = 0; i < count; i++) {
        double x;
        double y;
        const char *id = grafo_get_vertice_id(grafo, (GrafoVertice)i);

        if (!grafo_get_vertice_coords(grafo, (GrafoVertice)i, &x, &y)) {
            ctx->ok = 0;
            break;
        }

        fputs("  <svg:circle id=\"", ctx->file);
        write_escaped(ctx->file, id);
        fprintf(ctx->file,
                "\" cx=\"%.6f\" cy=\"%.6f\" r=\"4.000000\" fill=\"blue\" "
                "stroke=\"black\" fill-opacity=\"0.5\" />\n",
                x, y);
        fprintf(ctx->file,
                "  <svg:text x=\"%.6f\" y=\"%.6f\" fill=\"blue\" "
                "font-size=\"4\" text-anchor=\"middle\">",
                x, y);
        write_escaped(ctx->file, id);
        fputs("</svg:text>\n", ctx->file);
    }

    fputs("</svg:g>\n", ctx->file);
    if (ferror(ctx->file)) {
        ctx->ok = 0;
    }
}

static void draw_quadras(QuadraStore quadras, struct svg_file_context *ctx) {
    if (quadras == NULL || !ctx->ok) {
        return;
    }

    fputs("<svg:g id=\"quadras\">\n", ctx->file);
    quadra_store_for_each(quadras, draw_quadra, ctx);
    fputs("</svg:g>\n", ctx->file);
    if (ferror(ctx->file)) {
        ctx->ok = 0;
    }
}

int svg_render_base(const char *filepath, QuadraStore quadras, Grafo grafo) {
    return svg_render_com_anotacoes(filepath, quadras, grafo, NULL, NULL);
}

int svg_render_com_percursos(const char *filepath, QuadraStore quadras,
                             Grafo grafo, SvgPercursos percursos) {
    return svg_render_com_anotacoes(filepath, quadras, grafo, percursos, NULL);
}

int svg_render_com_anotacoes(const char *filepath, QuadraStore quadras,
                             Grafo grafo, SvgPercursos percursos,
                             SvgRegioes regioes) {
    struct svg_bbox bbox = {0.0, 0.0, 0.0, 0.0, 0};
    struct svg_file_context ctx;

    if (filepath == NULL) {
        return 0;
    }

    if (quadras != NULL) {
        quadra_store_for_each(quadras, bbox_include_quadra, &bbox);
    }
    bbox_include_grafo(grafo, &bbox);
    bbox_finish(&bbox);

    ctx.file = fopen(filepath, "w");
    ctx.ok = ctx.file != NULL;
    if (!ctx.ok) {
        return 0;
    }

    fprintf(ctx.file,
            "<?xml version='1.0' encoding='utf-8'?>\n"
            "<svg:svg xmlns:svg=\"http://www.w3.org/2000/svg\" "
            "viewBox=\"%.6f %.6f %.6f %.6f\">\n"
            "<svg:defs>\n"
            "  <svg:marker id=\"mArrow\" markerWidth=\"4\" markerHeight=\"4\" "
            "refX=\"4.000000\" refY=\"2.000000\" orient=\"auto\">\n"
            "    <svg:path d=\"M0,0 L0,4.000000 L4.000000,2.000000 z\" "
            "style=\"fill: #000000;\" />\n"
            "  </svg:marker>\n"
            "</svg:defs>\n",
            bbox.min_x, bbox.min_y, bbox.max_x - bbox.min_x,
            bbox.max_y - bbox.min_y);

    draw_grafo(grafo, &ctx);
    draw_quadras(quadras, &ctx);
    draw_regioes(regioes, &ctx);
    draw_percursos(grafo, percursos, &ctx);
    fputs("</svg:svg>\n", ctx.file);

    if (fclose(ctx.file) != 0) {
        ctx.ok = 0;
    }

    return ctx.ok;
}
