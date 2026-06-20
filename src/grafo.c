#include "grafo.h"

#include <float.h>
#include <math.h>
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

struct grafo_componente {
    double min_x;
    double min_y;
    double max_x;
    double max_y;
    int has_item;
};

struct grafo_componentes {
    struct grafo_componente *items;
    size_t count;
};

struct grafo_arestas {
    GrafoAresta *items;
    size_t count;
    size_t capacity;
};

struct mst_candidate {
    struct grafo_aresta *aresta;
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

static GrafoVertice dsu_find(GrafoVertice *parent, GrafoVertice vertice) {
    if (parent[vertice] != vertice) {
        parent[vertice] = dsu_find(parent, parent[vertice]);
    }
    return parent[vertice];
}

static void dsu_union(GrafoVertice *parent, int *rank, GrafoVertice a,
                      GrafoVertice b) {
    GrafoVertice root_a = dsu_find(parent, a);
    GrafoVertice root_b = dsu_find(parent, b);

    if (root_a == root_b) {
        return;
    }

    if (rank[root_a] < rank[root_b]) {
        parent[root_a] = root_b;
    } else if (rank[root_a] > rank[root_b]) {
        parent[root_b] = root_a;
    } else {
        parent[root_b] = root_a;
        rank[root_a]++;
    }
}

static void componente_include_point(struct grafo_componente *componente,
                                     double x, double y) {
    if (!componente->has_item) {
        componente->min_x = x;
        componente->min_y = y;
        componente->max_x = x;
        componente->max_y = y;
        componente->has_item = 1;
        return;
    }

    if (x < componente->min_x) {
        componente->min_x = x;
    }
    if (y < componente->min_y) {
        componente->min_y = y;
    }
    if (x > componente->max_x) {
        componente->max_x = x;
    }
    if (y > componente->max_y) {
        componente->max_y = y;
    }
}

static struct grafo_arestas *grafo_arestas_create(size_t capacity_hint) {
    struct grafo_arestas *arestas =
        (struct grafo_arestas *)calloc(1, sizeof(struct grafo_arestas));
    if (arestas == NULL) {
        return NULL;
    }

    if (capacity_hint > 0) {
        arestas->items = (GrafoAresta *)malloc(capacity_hint *
                                               sizeof(GrafoAresta));
        if (arestas->items == NULL) {
            free(arestas);
            return NULL;
        }
        arestas->capacity = capacity_hint;
    }

    return arestas;
}

static int grafo_arestas_append(struct grafo_arestas *arestas,
                                GrafoAresta aresta) {
    GrafoAresta *new_items;
    size_t new_capacity;

    if (arestas == NULL || aresta == NULL) {
        return 0;
    }

    if (arestas->count >= arestas->capacity) {
        new_capacity = arestas->capacity == 0 ? DEFAULT_CAPACITY
                                              : arestas->capacity * 2;
        new_items = (GrafoAresta *)realloc(arestas->items,
                                           new_capacity * sizeof(GrafoAresta));
        if (new_items == NULL) {
            return 0;
        }
        arestas->items = new_items;
        arestas->capacity = new_capacity;
    }

    arestas->items[arestas->count] = aresta;
    arestas->count++;
    return 1;
}

static int compare_mst_candidate(const void *a, const void *b) {
    const struct mst_candidate *candidate_a =
        (const struct mst_candidate *)a;
    const struct mst_candidate *candidate_b =
        (const struct mst_candidate *)b;
    const struct grafo_aresta *aresta_a = candidate_a->aresta;
    const struct grafo_aresta *aresta_b = candidate_b->aresta;

    if (aresta_a->cmp < aresta_b->cmp) {
        return -1;
    }
    if (aresta_a->cmp > aresta_b->cmp) {
        return 1;
    }
    if (aresta_a->origem != aresta_b->origem) {
        return aresta_a->origem < aresta_b->origem ? -1 : 1;
    }
    if (aresta_a->destino != aresta_b->destino) {
        return aresta_a->destino < aresta_b->destino ? -1 : 1;
    }
    return strcmp(aresta_a->nome, aresta_b->nome);
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

    if (grafo == NULL || id == NULL || !isfinite(x) || !isfinite(y) ||
        grafo_find_vertice(grafo, id) >= 0) {
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

GrafoVertice grafo_find_vertice_mais_proximo(Grafo grafo_ref,
                                             double x, double y) {
    struct grafo *grafo = (struct grafo *)grafo_ref;
    GrafoVertice melhor_vertice = -1;
    double melhor_distancia = DBL_MAX;

    if (grafo == NULL || grafo->vertice_count == 0) {
        return -1;
    }

    for (size_t i = 0; i < grafo->vertice_count; i++) {
        double dx = grafo->vertices[i].x - x;
        double dy = grafo->vertices[i].y - y;
        double distancia = dx * dx + dy * dy;

        if (distancia < melhor_distancia) {
            melhor_distancia = distancia;
            melhor_vertice = (GrafoVertice)i;
        }
    }

    return melhor_vertice;
}

GrafoAresta grafo_add_aresta(Grafo grafo_ref, GrafoVertice origem,
                             GrafoVertice destino, const char *ldir,
                             const char *lesq, double cmp, double vm,
                             const char *nome) {
    struct grafo *grafo = (struct grafo *)grafo_ref;
    struct grafo_aresta *aresta;

    if (!is_valid_vertice(grafo, origem) || !is_valid_vertice(grafo, destino) ||
        ldir == NULL || lesq == NULL || nome == NULL || !isfinite(cmp) ||
        !isfinite(vm) || cmp < 0.0 || vm < 0.0) {
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

static int ponto_em_regiao(double px, double py, double x, double y,
                           double w, double h) {
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

int grafo_atualizar_vm_regiao(Grafo grafo_ref, double vm, double x, double y,
                              double w, double h) {
    struct grafo *grafo = (struct grafo *)grafo_ref;
    int atualizadas = 0;

    if (grafo == NULL || vm < 0.0 || w < 0.0 || h < 0.0) {
        return -1;
    }

    for (size_t i = 0; i < grafo->vertice_count; i++) {
        for (struct grafo_aresta *aresta = grafo->vertices[i].arestas_saida;
             aresta != NULL;
             aresta = aresta->next) {
            struct grafo_vertice *origem = &grafo->vertices[aresta->origem];
            struct grafo_vertice *destino = &grafo->vertices[aresta->destino];

            if (ponto_em_regiao(origem->x, origem->y, x, y, w, h) &&
                ponto_em_regiao(destino->x, destino->y, x, y, w, h)) {
                aresta->vm = vm;
                atualizadas++;
            }
        }
    }

    return atualizadas;
}

GrafoComponentes grafo_calcular_componentes_lentos(Grafo grafo_ref,
                                                   double limite_vm) {
    struct grafo *grafo = (struct grafo *)grafo_ref;
    struct grafo_componentes *componentes;
    GrafoVertice *parent = NULL;
    int *rank = NULL;
    int *ativo = NULL;
    int *component_index = NULL;

    if (grafo == NULL || limite_vm < 0.0) {
        return NULL;
    }

    componentes = (struct grafo_componentes *)calloc(
        1, sizeof(struct grafo_componentes));
    if (componentes == NULL) {
        return NULL;
    }

    if (grafo->vertice_count == 0) {
        return componentes;
    }

    parent = (GrafoVertice *)malloc(grafo->vertice_count * sizeof(GrafoVertice));
    rank = (int *)calloc(grafo->vertice_count, sizeof(int));
    ativo = (int *)calloc(grafo->vertice_count, sizeof(int));
    component_index = (int *)malloc(grafo->vertice_count * sizeof(int));
    componentes->items = (struct grafo_componente *)calloc(
        grafo->vertice_count, sizeof(struct grafo_componente));
    if (parent == NULL || rank == NULL || ativo == NULL ||
        component_index == NULL || componentes->items == NULL) {
        grafo_componentes_destroy(componentes);
        free(parent);
        free(rank);
        free(ativo);
        free(component_index);
        return NULL;
    }

    for (size_t i = 0; i < grafo->vertice_count; i++) {
        parent[i] = (GrafoVertice)i;
        component_index[i] = -1;
    }

    for (size_t i = 0; i < grafo->vertice_count; i++) {
        for (struct grafo_aresta *aresta = grafo->vertices[i].arestas_saida;
             aresta != NULL;
             aresta = aresta->next) {
            if (aresta->vm < limite_vm) {
                ativo[aresta->origem] = 1;
                ativo[aresta->destino] = 1;
                dsu_union(parent, rank, aresta->origem, aresta->destino);
            }
        }
    }

    for (size_t i = 0; i < grafo->vertice_count; i++) {
        GrafoVertice root;
        int indice;

        if (!ativo[i]) {
            continue;
        }

        root = dsu_find(parent, (GrafoVertice)i);
        indice = component_index[root];
        if (indice < 0) {
            indice = (int)componentes->count;
            component_index[root] = indice;
            componentes->count++;
        }

        componente_include_point(&componentes->items[indice],
                                 grafo->vertices[i].x,
                                 grafo->vertices[i].y);
    }

    free(parent);
    free(rank);
    free(ativo);
    free(component_index);
    return componentes;
}

size_t grafo_componentes_count(GrafoComponentes componentes_ref) {
    struct grafo_componentes *componentes =
        (struct grafo_componentes *)componentes_ref;
    return componentes != NULL ? componentes->count : 0;
}

int grafo_componentes_get_bbox(GrafoComponentes componentes_ref, size_t indice,
                               double *out_x, double *out_y,
                               double *out_w, double *out_h) {
    struct grafo_componentes *componentes =
        (struct grafo_componentes *)componentes_ref;
    struct grafo_componente *componente;

    if (componentes == NULL || indice >= componentes->count) {
        return 0;
    }

    componente = &componentes->items[indice];
    if (!componente->has_item) {
        return 0;
    }

    if (out_x != NULL) {
        *out_x = componente->min_x;
    }
    if (out_y != NULL) {
        *out_y = componente->min_y;
    }
    if (out_w != NULL) {
        *out_w = componente->max_x - componente->min_x;
    }
    if (out_h != NULL) {
        *out_h = componente->max_y - componente->min_y;
    }

    return 1;
}

void grafo_componentes_destroy(GrafoComponentes componentes_ref) {
    struct grafo_componentes *componentes =
        (struct grafo_componentes *)componentes_ref;
    if (componentes == NULL) {
        return;
    }

    free(componentes->items);
    free(componentes);
}

GrafoArestas grafo_aplicar_expansao_agm(Grafo grafo_ref, double limite_vm) {
    struct grafo *grafo = (struct grafo *)grafo_ref;
    struct grafo_arestas *selecionadas = NULL;
    struct mst_candidate *candidates = NULL;
    GrafoVertice *parent = NULL;
    int *rank = NULL;
    size_t candidate_count = 0;

    if (grafo == NULL || limite_vm < 0.0) {
        return NULL;
    }

    selecionadas = grafo_arestas_create(0);
    if (selecionadas == NULL) {
        return NULL;
    }

    if (grafo->vertice_count == 0 || grafo->aresta_count == 0) {
        return selecionadas;
    }

    candidates = (struct mst_candidate *)malloc(
        grafo->aresta_count * sizeof(struct mst_candidate));
    parent = (GrafoVertice *)malloc(grafo->vertice_count *
                                    sizeof(GrafoVertice));
    rank = (int *)calloc(grafo->vertice_count, sizeof(int));
    if (candidates == NULL || parent == NULL || rank == NULL) {
        grafo_arestas_destroy(selecionadas);
        free(candidates);
        free(parent);
        free(rank);
        return NULL;
    }

    for (size_t i = 0; i < grafo->vertice_count; i++) {
        parent[i] = (GrafoVertice)i;
    }

    for (size_t i = 0; i < grafo->vertice_count; i++) {
        for (struct grafo_aresta *aresta = grafo->vertices[i].arestas_saida;
             aresta != NULL;
             aresta = aresta->next) {
            candidates[candidate_count].aresta = aresta;
            candidate_count++;
        }
    }

    qsort(candidates, candidate_count, sizeof(struct mst_candidate),
          compare_mst_candidate);

    for (size_t i = 0; i < candidate_count; i++) {
        struct grafo_aresta *aresta = candidates[i].aresta;
        GrafoVertice root_origem = dsu_find(parent, aresta->origem);
        GrafoVertice root_destino = dsu_find(parent, aresta->destino);

        if (root_origem == root_destino) {
            continue;
        }

        dsu_union(parent, rank, root_origem, root_destino);
        if (aresta->vm < limite_vm &&
            !grafo_arestas_append(selecionadas, aresta)) {
            grafo_arestas_destroy(selecionadas);
            free(candidates);
            free(parent);
            free(rank);
            return NULL;
        }
    }

    for (size_t i = 0; i < selecionadas->count; i++) {
        struct grafo_aresta *aresta =
            (struct grafo_aresta *)selecionadas->items[i];
        aresta->vm *= 1.5;
    }

    free(candidates);
    free(parent);
    free(rank);
    return selecionadas;
}

size_t grafo_arestas_count(GrafoArestas arestas_ref) {
    struct grafo_arestas *arestas = (struct grafo_arestas *)arestas_ref;
    return arestas != NULL ? arestas->count : 0;
}

GrafoAresta grafo_arestas_get(GrafoArestas arestas_ref, size_t indice) {
    struct grafo_arestas *arestas = (struct grafo_arestas *)arestas_ref;

    if (arestas == NULL || indice >= arestas->count) {
        return NULL;
    }

    return arestas->items[indice];
}

void grafo_arestas_destroy(GrafoArestas arestas_ref) {
    struct grafo_arestas *arestas = (struct grafo_arestas *)arestas_ref;
    if (arestas == NULL) {
        return;
    }

    free(arestas->items);
    free(arestas);
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
    if (aresta != NULL && isfinite(vm) && vm >= 0.0) {
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
