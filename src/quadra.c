#include "quadra.h"

#include <stdlib.h>
#include <string.h>

struct quadra {
    char *cep;
    double x;
    double y;
    double w;
    double h;
    double sw;
    char *cfill;
    char *cstrk;
};

static char *copy_text(const char *text) {
    if (text == NULL) {
        return NULL;
    }

    size_t length = strlen(text) + 1;
    char *copy = (char *)malloc(length);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, text, length);
    return copy;
}

static int is_valid_dimension(double value) {
    return value >= 0.0;
}

Quadra quadra_create(const char *cep, double x, double y, double w, double h,
                     double sw, const char *cfill, const char *cstrk) {
    if (cep == NULL || cfill == NULL || cstrk == NULL ||
        !is_valid_dimension(w) || !is_valid_dimension(h) || sw < 0.0) {
        return NULL;
    }

    struct quadra *quadra = (struct quadra *)calloc(1, sizeof(struct quadra));
    if (quadra == NULL) {
        return NULL;
    }

    quadra->cep = copy_text(cep);
    quadra->cfill = copy_text(cfill);
    quadra->cstrk = copy_text(cstrk);
    if (quadra->cep == NULL || quadra->cfill == NULL || quadra->cstrk == NULL) {
        quadra_destroy(quadra);
        return NULL;
    }

    quadra->x = x;
    quadra->y = y;
    quadra->w = w;
    quadra->h = h;
    quadra->sw = sw;

    return quadra;
}

void quadra_destroy(Quadra quadra_ref) {
    struct quadra *quadra = (struct quadra *)quadra_ref;
    if (quadra == NULL) {
        return;
    }

    free(quadra->cep);
    free(quadra->cfill);
    free(quadra->cstrk);
    free(quadra);
}

const char *quadra_get_cep(Quadra quadra_ref) {
    struct quadra *quadra = (struct quadra *)quadra_ref;
    return quadra != NULL ? quadra->cep : NULL;
}

double quadra_get_x(Quadra quadra_ref) {
    struct quadra *quadra = (struct quadra *)quadra_ref;
    return quadra != NULL ? quadra->x : 0.0;
}

double quadra_get_y(Quadra quadra_ref) {
    struct quadra *quadra = (struct quadra *)quadra_ref;
    return quadra != NULL ? quadra->y : 0.0;
}

double quadra_get_w(Quadra quadra_ref) {
    struct quadra *quadra = (struct quadra *)quadra_ref;
    return quadra != NULL ? quadra->w : 0.0;
}

double quadra_get_h(Quadra quadra_ref) {
    struct quadra *quadra = (struct quadra *)quadra_ref;
    return quadra != NULL ? quadra->h : 0.0;
}

double quadra_get_sw(Quadra quadra_ref) {
    struct quadra *quadra = (struct quadra *)quadra_ref;
    return quadra != NULL ? quadra->sw : 0.0;
}

const char *quadra_get_cfill(Quadra quadra_ref) {
    struct quadra *quadra = (struct quadra *)quadra_ref;
    return quadra != NULL ? quadra->cfill : NULL;
}

const char *quadra_get_cstrk(Quadra quadra_ref) {
    struct quadra *quadra = (struct quadra *)quadra_ref;
    return quadra != NULL ? quadra->cstrk : NULL;
}

void quadra_get_anchor(Quadra quadra_ref, double *out_x, double *out_y) {
    struct quadra *quadra = (struct quadra *)quadra_ref;
    if (quadra == NULL) {
        return;
    }

    if (out_x != NULL) {
        *out_x = quadra->x + quadra->w;
    }
    if (out_y != NULL) {
        *out_y = quadra->y + quadra->h;
    }
}

int quadra_get_address_point(Quadra quadra_ref, char face, double num,
                             double *out_x, double *out_y) {
    struct quadra *quadra = (struct quadra *)quadra_ref;
    double x;
    double y;

    if (quadra == NULL) {
        return 0;
    }

    switch (face) {
    case 'N':
        x = quadra->x + num;
        y = quadra->y + quadra->h;
        break;
    case 'S':
        x = quadra->x + num;
        y = quadra->y;
        break;
    case 'L':
        x = quadra->x;
        y = quadra->y + num;
        break;
    case 'O':
        x = quadra->x + quadra->w;
        y = quadra->y + num;
        break;
    default:
        return 0;
    }

    if (out_x != NULL) {
        *out_x = x;
    }
    if (out_y != NULL) {
        *out_y = y;
    }

    return 1;
}
