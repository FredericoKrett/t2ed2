#include "fila_prioridade.h"

#include <stdlib.h>

#define FILA_PRIORIDADE_CAPACIDADE_PADRAO 16

struct fila_prioridade_item {
    int valor;
    double prioridade;
};

struct fila_prioridade {
    struct fila_prioridade_item *itens;
    size_t tamanho;
    size_t capacidade;
};

static size_t capacidade_inicial(size_t capacity_hint) {
    if (capacity_hint == 0) {
        return FILA_PRIORIDADE_CAPACIDADE_PADRAO;
    }

    return capacity_hint;
}

static void troca_itens(struct fila_prioridade_item *a,
                        struct fila_prioridade_item *b) {
    struct fila_prioridade_item tmp = *a;
    *a = *b;
    *b = tmp;
}

static int item_tem_menor_prioridade(struct fila_prioridade_item a,
                                     struct fila_prioridade_item b) {
    return a.prioridade < b.prioridade;
}

static int garantir_capacidade(struct fila_prioridade *fila) {
    size_t nova_capacidade;
    struct fila_prioridade_item *novos_itens;

    if (fila->tamanho < fila->capacidade) {
        return 1;
    }

    nova_capacidade = fila->capacidade * 2;
    if (nova_capacidade == 0) {
        nova_capacidade = FILA_PRIORIDADE_CAPACIDADE_PADRAO;
    }

    novos_itens = (struct fila_prioridade_item *)realloc(
        fila->itens, nova_capacidade * sizeof(struct fila_prioridade_item));
    if (novos_itens == NULL) {
        return 0;
    }

    fila->itens = novos_itens;
    fila->capacidade = nova_capacidade;
    return 1;
}

static void sobe_item(struct fila_prioridade *fila, size_t indice) {
    while (indice > 0) {
        size_t pai = (indice - 1) / 2;

        if (!item_tem_menor_prioridade(fila->itens[indice],
                                       fila->itens[pai])) {
            return;
        }

        troca_itens(&fila->itens[indice], &fila->itens[pai]);
        indice = pai;
    }
}

static void desce_item(struct fila_prioridade *fila, size_t indice) {
    while (1) {
        size_t esquerda = indice * 2 + 1;
        size_t direita = esquerda + 1;
        size_t menor = indice;

        if (esquerda < fila->tamanho &&
            item_tem_menor_prioridade(fila->itens[esquerda],
                                      fila->itens[menor])) {
            menor = esquerda;
        }
        if (direita < fila->tamanho &&
            item_tem_menor_prioridade(fila->itens[direita],
                                      fila->itens[menor])) {
            menor = direita;
        }
        if (menor == indice) {
            return;
        }

        troca_itens(&fila->itens[indice], &fila->itens[menor]);
        indice = menor;
    }
}

FilaPrioridade fila_prioridade_create(size_t capacity_hint) {
    struct fila_prioridade *fila =
        (struct fila_prioridade *)calloc(1, sizeof(struct fila_prioridade));
    if (fila == NULL) {
        return NULL;
    }

    fila->capacidade = capacidade_inicial(capacity_hint);
    fila->itens = (struct fila_prioridade_item *)malloc(
        fila->capacidade * sizeof(struct fila_prioridade_item));
    if (fila->itens == NULL) {
        fila_prioridade_destroy(fila);
        return NULL;
    }

    return fila;
}

int fila_prioridade_push(FilaPrioridade fila_ref, int valor, double prioridade) {
    struct fila_prioridade *fila = (struct fila_prioridade *)fila_ref;
    struct fila_prioridade_item item;

    if (fila == NULL || !garantir_capacidade(fila)) {
        return 0;
    }

    item.valor = valor;
    item.prioridade = prioridade;
    fila->itens[fila->tamanho] = item;
    sobe_item(fila, fila->tamanho);
    fila->tamanho++;
    return 1;
}

int fila_prioridade_pop(FilaPrioridade fila_ref, int *out_valor,
                        double *out_prioridade) {
    struct fila_prioridade *fila = (struct fila_prioridade *)fila_ref;
    struct fila_prioridade_item item;

    if (fila == NULL || fila->tamanho == 0 || out_valor == NULL ||
        out_prioridade == NULL) {
        return 0;
    }

    item = fila->itens[0];
    fila->tamanho--;
    if (fila->tamanho > 0) {
        fila->itens[0] = fila->itens[fila->tamanho];
        desce_item(fila, 0);
    }

    *out_valor = item.valor;
    *out_prioridade = item.prioridade;
    return 1;
}

int fila_prioridade_peek(FilaPrioridade fila_ref, int *out_valor,
                         double *out_prioridade) {
    struct fila_prioridade *fila = (struct fila_prioridade *)fila_ref;

    if (fila == NULL || fila->tamanho == 0 || out_valor == NULL ||
        out_prioridade == NULL) {
        return 0;
    }

    *out_valor = fila->itens[0].valor;
    *out_prioridade = fila->itens[0].prioridade;
    return 1;
}

int fila_prioridade_is_empty(FilaPrioridade fila_ref) {
    struct fila_prioridade *fila = (struct fila_prioridade *)fila_ref;
    return fila == NULL || fila->tamanho == 0;
}

size_t fila_prioridade_size(FilaPrioridade fila_ref) {
    struct fila_prioridade *fila = (struct fila_prioridade *)fila_ref;
    return fila != NULL ? fila->tamanho : 0;
}

void fila_prioridade_destroy(FilaPrioridade fila_ref) {
    struct fila_prioridade *fila = (struct fila_prioridade *)fila_ref;

    if (fila == NULL) {
        return;
    }

    free(fila->itens);
    free(fila);
}
