#include "geo_parser.h"

#include <stdio.h>
#include <string.h>

#define LINE_SIZE 512
#define TOKEN_SIZE 64

struct geo_style {
    double sw;
    char cfill[TOKEN_SIZE];
    char cstrk[TOKEN_SIZE];
};

static void style_init(struct geo_style *style) {
    style->sw = 1.0;
    strcpy(style->cfill, "white");
    strcpy(style->cstrk, "black");
}

static int is_blank_line(const char *line) {
    while (*line != '\0') {
        if (*line != ' ' && *line != '\t' && *line != '\n' && *line != '\r') {
            return 0;
        }
        line++;
    }
    return 1;
}

static int parse_cq(const char *line, struct geo_style *style) {
    char extra[TOKEN_SIZE];
    int read = sscanf(line, "%*s %lf %63s %63s %63s",
                      &style->sw, style->cfill, style->cstrk, extra);

    return read == 3;
}

static int parse_q(const char *line, QuadraStore store,
                   const struct geo_style *style) {
    char cep[TOKEN_SIZE];
    char extra[TOKEN_SIZE];
    double x;
    double y;
    double w;
    double h;
    int read = sscanf(line, "%*s %63s %lf %lf %lf %lf %63s",
                      cep, &x, &y, &w, &h, extra);
    Quadra quadra;

    if (read != 5) {
        return -1;
    }

    quadra = quadra_create(cep, x, y, w, h, style->sw,
                           style->cfill, style->cstrk);
    if (quadra == NULL) {
        return -1;
    }

    if (!quadra_store_insert(store, quadra)) {
        quadra_destroy(quadra);
        return -1;
    }

    return 1;
}

int geo_parser_parse_file(const char *filepath, QuadraStore store) {
    FILE *file;
    char line[LINE_SIZE];
    struct geo_style style;
    int inserted = 0;

    if (filepath == NULL || store == NULL) {
        return -1;
    }

    file = fopen(filepath, "r");
    if (file == NULL) {
        return -1;
    }

    style_init(&style);

    while (fgets(line, sizeof(line), file) != NULL) {
        char command[TOKEN_SIZE];

        if (is_blank_line(line)) {
            continue;
        }

        if (sscanf(line, "%63s", command) != 1) {
            fclose(file);
            return -1;
        }

        if (strcmp(command, "cq") == 0) {
            if (!parse_cq(line, &style)) {
                fclose(file);
                return -1;
            }
        } else if (strcmp(command, "q") == 0) {
            int result = parse_q(line, store, &style);
            if (result < 0) {
                fclose(file);
                return -1;
            }
            inserted += result;
        } else {
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return inserted;
}
