#include "registradores.h"

#include <stdlib.h>

struct registrador {
    int definido;
    double x;
    double y;
};

struct registradores {
    struct registrador itens[REGISTRADORES_TOTAL];
};

static int indice_valido(int indice) {
    return indice >= 0 && indice < REGISTRADORES_TOTAL;
}

Registradores registradores_create(void) {
    return calloc(1, sizeof(struct registradores));
}

int registradores_parse_nome(const char *nome) {
    if (nome == NULL || nome[0] != 'R' || nome[1] == '\0') {
        return -1;
    }

    if (nome[1] >= '0' && nome[1] <= '9' && nome[2] == '\0') {
        return nome[1] - '0';
    }

    if (nome[1] == '1' && nome[2] == '0' && nome[3] == '\0') {
        return 10;
    }

    return -1;
}

int registradores_set(Registradores registradores_ref, int indice,
                      double x, double y) {
    struct registradores *registradores = (struct registradores *)registradores_ref;

    if (registradores == NULL || !indice_valido(indice)) {
        return 0;
    }

    registradores->itens[indice].definido = 1;
    registradores->itens[indice].x = x;
    registradores->itens[indice].y = y;
    return 1;
}

int registradores_is_set(Registradores registradores_ref, int indice) {
    struct registradores *registradores = (struct registradores *)registradores_ref;

    if (registradores == NULL || !indice_valido(indice)) {
        return 0;
    }

    return registradores->itens[indice].definido;
}

int registradores_get(Registradores registradores_ref, int indice,
                      double *out_x, double *out_y) {
    struct registradores *registradores = (struct registradores *)registradores_ref;

    if (registradores == NULL || !indice_valido(indice) ||
        !registradores->itens[indice].definido) {
        return 0;
    }

    if (out_x != NULL) {
        *out_x = registradores->itens[indice].x;
    }
    if (out_y != NULL) {
        *out_y = registradores->itens[indice].y;
    }

    return 1;
}

int registradores_clear(Registradores registradores_ref, int indice) {
    struct registradores *registradores = (struct registradores *)registradores_ref;

    if (registradores == NULL || !indice_valido(indice) ||
        !registradores->itens[indice].definido) {
        return 0;
    }

    registradores->itens[indice].definido = 0;
    registradores->itens[indice].x = 0.0;
    registradores->itens[indice].y = 0.0;
    return 1;
}

void registradores_destroy(Registradores registradores) {
    free(registradores);
}
