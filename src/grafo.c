#include "grafo.h"

#include <stdlib.h>
#include <string.h>

#define DEFAULT_CAPACITY 16

struct grafo_aresta {
    GrafoVertice origem;
    GrafoVertice destino;
    char *ldir;
    char *lesq;
    double cmp;
    double vm;
    char *nome;
    struct grafo_aresta *next;
};

struct grafo_vertice {
    char *id;
    double x;
    double y;
    struct grafo_aresta *arestas_saida;
    size_t out_degree;
};

struct grafo {
    struct grafo_vertice *vertices;
    size_t capacity;
    size_t vertice_count;
    size_t aresta_count;
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

static size_t normalize_capacity(size_t capacity_hint) {
    return capacity_hint == 0 ? DEFAULT_CAPACITY : capacity_hint;
}

static int is_valid_vertice(const struct grafo *grafo, GrafoVertice vertice) {
    return grafo != NULL && vertice >= 0 && (size_t)vertice < grafo->vertice_count;
}

static int ensure_capacity(struct grafo *grafo) {
    struct grafo_vertice *new_vertices;
    size_t old_capacity;
    size_t new_capacity;

    if (grafo->vertice_count < grafo->capacity) {
        return 1;
    }

    old_capacity = grafo->capacity;
    new_capacity = old_capacity * 2;
    if (new_capacity == 0) {
        new_capacity = DEFAULT_CAPACITY;
    }

    new_vertices = (struct grafo_vertice *)realloc(grafo->vertices,
                                                   new_capacity * sizeof(struct grafo_vertice));
    if (new_vertices == NULL) {
        return 0;
    }

    grafo->vertices = new_vertices;
    for (size_t i = old_capacity; i < new_capacity; i++) {
        grafo->vertices[i].id = NULL;
        grafo->vertices[i].x = 0.0;
        grafo->vertices[i].y = 0.0;
        grafo->vertices[i].arestas_saida = NULL;
        grafo->vertices[i].out_degree = 0;
    }
    grafo->capacity = new_capacity;

    return 1;
}

static void destroy_aresta(struct grafo_aresta *aresta) {
    if (aresta == NULL) {
        return;
    }

    free(aresta->ldir);
    free(aresta->lesq);
    free(aresta->nome);
    free(aresta);
}

Grafo grafo_create(size_t capacity_hint) {
    struct grafo *grafo = (struct grafo *)calloc(1, sizeof(struct grafo));
    if (grafo == NULL) {
        return NULL;
    }

    grafo->capacity = normalize_capacity(capacity_hint);
    grafo->vertices = (struct grafo_vertice *)calloc(grafo->capacity,
                                                     sizeof(struct grafo_vertice));
    if (grafo->vertices == NULL) {
        free(grafo);
        return NULL;
    }

    return grafo;
}

GrafoVertice grafo_add_vertice(Grafo grafo_ref, const char *id, double x, double y) {
    struct grafo *grafo = (struct grafo *)grafo_ref;
    struct grafo_vertice *vertice;

    if (grafo == NULL || id == NULL || grafo_find_vertice(grafo, id) >= 0) {
        return -1;
    }

    if (!ensure_capacity(grafo)) {
        return -1;
    }

    vertice = &grafo->vertices[grafo->vertice_count];
    vertice->id = copy_text(id);
    if (vertice->id == NULL) {
        return -1;
    }

    vertice->x = x;
    vertice->y = y;
    vertice->arestas_saida = NULL;
    vertice->out_degree = 0;
    grafo->vertice_count++;

    return (GrafoVertice)(grafo->vertice_count - 1);
}

GrafoVertice grafo_find_vertice(Grafo grafo_ref, const char *id) {
    struct grafo *grafo = (struct grafo *)grafo_ref;

    if (grafo == NULL || id == NULL) {
        return -1;
    }

    for (size_t i = 0; i < grafo->vertice_count; i++) {
        if (grafo->vertices[i].id != NULL && strcmp(grafo->vertices[i].id, id) == 0) {
            return (GrafoVertice)i;
        }
    }

    return -1;
}

const char *grafo_get_vertice_id(Grafo grafo_ref, GrafoVertice vertice) {
    struct grafo *grafo = (struct grafo *)grafo_ref;
    return is_valid_vertice(grafo, vertice) ? grafo->vertices[vertice].id : NULL;
}

int grafo_get_vertice_coords(Grafo grafo_ref, GrafoVertice vertice,
                             double *out_x, double *out_y) {
    struct grafo *grafo = (struct grafo *)grafo_ref;

    if (!is_valid_vertice(grafo, vertice)) {
        return 0;
    }

    if (out_x != NULL) {
        *out_x = grafo->vertices[vertice].x;
    }
    if (out_y != NULL) {
        *out_y = grafo->vertices[vertice].y;
    }

    return 1;
}

GrafoAresta grafo_add_aresta(Grafo grafo_ref, GrafoVertice origem,
                             GrafoVertice destino, const char *ldir,
                             const char *lesq, double cmp, double vm,
                             const char *nome) {
    struct grafo *grafo = (struct grafo *)grafo_ref;
    struct grafo_aresta *aresta;

    if (!is_valid_vertice(grafo, origem) || !is_valid_vertice(grafo, destino) ||
        ldir == NULL || lesq == NULL || nome == NULL || cmp < 0.0 || vm < 0.0) {
        return NULL;
    }

    aresta = (struct grafo_aresta *)calloc(1, sizeof(struct grafo_aresta));
    if (aresta == NULL) {
        return NULL;
    }

    aresta->ldir = copy_text(ldir);
    aresta->lesq = copy_text(lesq);
    aresta->nome = copy_text(nome);
    if (aresta->ldir == NULL || aresta->lesq == NULL || aresta->nome == NULL) {
        destroy_aresta(aresta);
        return NULL;
    }

    aresta->origem = origem;
    aresta->destino = destino;
    aresta->cmp = cmp;
    aresta->vm = vm;
    aresta->next = grafo->vertices[origem].arestas_saida;
    grafo->vertices[origem].arestas_saida = aresta;
    grafo->vertices[origem].out_degree++;
    grafo->aresta_count++;

    return aresta;
}

size_t grafo_get_vertice_count(Grafo grafo_ref) {
    struct grafo *grafo = (struct grafo *)grafo_ref;
    return grafo != NULL ? grafo->vertice_count : 0;
}

size_t grafo_get_aresta_count(Grafo grafo_ref) {
    struct grafo *grafo = (struct grafo *)grafo_ref;
    return grafo != NULL ? grafo->aresta_count : 0;
}

size_t grafo_get_out_degree(Grafo grafo_ref, GrafoVertice vertice) {
    struct grafo *grafo = (struct grafo *)grafo_ref;
    return is_valid_vertice(grafo, vertice) ? grafo->vertices[vertice].out_degree : 0;
}

void grafo_for_each_aresta_saida(Grafo grafo_ref, GrafoVertice origem,
                                 GrafoArestaVisitor visitor, void *context) {
    struct grafo *grafo = (struct grafo *)grafo_ref;

    if (!is_valid_vertice(grafo, origem) || visitor == NULL) {
        return;
    }

    for (struct grafo_aresta *aresta = grafo->vertices[origem].arestas_saida;
         aresta != NULL;
         aresta = aresta->next) {
        visitor(grafo_ref, aresta, context);
    }
}

GrafoVertice grafo_aresta_get_origem(GrafoAresta aresta_ref) {
    struct grafo_aresta *aresta = (struct grafo_aresta *)aresta_ref;
    return aresta != NULL ? aresta->origem : -1;
}

GrafoVertice grafo_aresta_get_destino(GrafoAresta aresta_ref) {
    struct grafo_aresta *aresta = (struct grafo_aresta *)aresta_ref;
    return aresta != NULL ? aresta->destino : -1;
}

const char *grafo_aresta_get_ldir(GrafoAresta aresta_ref) {
    struct grafo_aresta *aresta = (struct grafo_aresta *)aresta_ref;
    return aresta != NULL ? aresta->ldir : NULL;
}

const char *grafo_aresta_get_lesq(GrafoAresta aresta_ref) {
    struct grafo_aresta *aresta = (struct grafo_aresta *)aresta_ref;
    return aresta != NULL ? aresta->lesq : NULL;
}

double grafo_aresta_get_cmp(GrafoAresta aresta_ref) {
    struct grafo_aresta *aresta = (struct grafo_aresta *)aresta_ref;
    return aresta != NULL ? aresta->cmp : 0.0;
}

double grafo_aresta_get_vm(GrafoAresta aresta_ref) {
    struct grafo_aresta *aresta = (struct grafo_aresta *)aresta_ref;
    return aresta != NULL ? aresta->vm : 0.0;
}

void grafo_aresta_set_vm(GrafoAresta aresta_ref, double vm) {
    struct grafo_aresta *aresta = (struct grafo_aresta *)aresta_ref;
    if (aresta != NULL && vm >= 0.0) {
        aresta->vm = vm;
    }
}

const char *grafo_aresta_get_nome(GrafoAresta aresta_ref) {
    struct grafo_aresta *aresta = (struct grafo_aresta *)aresta_ref;
    return aresta != NULL ? aresta->nome : NULL;
}

void grafo_destroy(Grafo grafo_ref) {
    struct grafo *grafo = (struct grafo *)grafo_ref;
    if (grafo == NULL) {
        return;
    }

    for (size_t i = 0; i < grafo->vertice_count; i++) {
        struct grafo_aresta *aresta = grafo->vertices[i].arestas_saida;
        while (aresta != NULL) {
            struct grafo_aresta *next = aresta->next;
            destroy_aresta(aresta);
            aresta = next;
        }
        free(grafo->vertices[i].id);
    }

    free(grafo->vertices);
    free(grafo);
}
