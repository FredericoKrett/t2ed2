#include "caminho.h"
#include "fila_prioridade.h"

#include <float.h>
#include <stdlib.h>

struct caminho {
    GrafoVertice origem;
    GrafoVertice destino;
    CaminhoCriterio criterio;
    int existe;
    double custo;
    GrafoAresta *arestas;
    size_t aresta_count;
};

struct dijkstra_context {
    CaminhoCriterio criterio;
    double *distancias;
    GrafoAresta *anteriores;
    int *visitados;
    FilaPrioridade fila;
    GrafoVertice atual;
    size_t vertice_count;
    int falha;
};

static int criterio_valido(CaminhoCriterio criterio) {
    return criterio == CAMINHO_CRITERIO_COMPRIMENTO ||
           criterio == CAMINHO_CRITERIO_TEMPO;
}

static int vertice_valido(Grafo grafo, GrafoVertice vertice) {
    return grafo != NULL && vertice >= 0 &&
           (size_t)vertice < grafo_get_vertice_count(grafo);
}

static double peso_aresta(GrafoAresta aresta, CaminhoCriterio criterio,
                          int *valido) {
    double cmp = grafo_aresta_get_cmp(aresta);
    double vm;

    *valido = 1;
    if (criterio == CAMINHO_CRITERIO_COMPRIMENTO) {
        return cmp;
    }

    vm = grafo_aresta_get_vm(aresta);
    if (vm <= 0.0) {
        *valido = 0;
        return 0.0;
    }

    return cmp / vm;
}

static void relaxar_aresta(Grafo grafo, GrafoAresta aresta, void *context) {
    struct dijkstra_context *ctx = (struct dijkstra_context *)context;
    GrafoVertice destino;
    double peso;
    double nova_distancia;
    int valido;

    (void)grafo;

    if (ctx->falha) {
        return;
    }

    destino = grafo_aresta_get_destino(aresta);
    if (destino < 0 || (size_t)destino >= ctx->vertice_count ||
        ctx->visitados[destino]) {
        return;
    }

    peso = peso_aresta(aresta, ctx->criterio, &valido);
    if (!valido) {
        return;
    }

    nova_distancia = ctx->distancias[ctx->atual] + peso;
    if (nova_distancia < ctx->distancias[destino]) {
        ctx->distancias[destino] = nova_distancia;
        ctx->anteriores[destino] = aresta;
        if (!fila_prioridade_push(ctx->fila, destino, nova_distancia)) {
            ctx->falha = 1;
        }
    }
}

static struct caminho *criar_caminho(GrafoVertice origem, GrafoVertice destino,
                                     CaminhoCriterio criterio) {
    struct caminho *caminho = (struct caminho *)calloc(1, sizeof(struct caminho));
    if (caminho == NULL) {
        return NULL;
    }

    caminho->origem = origem;
    caminho->destino = destino;
    caminho->criterio = criterio;
    return caminho;
}

static int reconstruir_caminho(struct caminho *caminho,
                               GrafoAresta *anteriores) {
    GrafoVertice atual = caminho->destino;
    size_t count = 0;

    while (atual != caminho->origem) {
        GrafoAresta aresta = anteriores[atual];

        if (aresta == NULL) {
            return 0;
        }

        count++;
        atual = grafo_aresta_get_origem(aresta);
    }

    caminho->arestas = (GrafoAresta *)malloc(count * sizeof(GrafoAresta));
    if (count > 0 && caminho->arestas == NULL) {
        return 0;
    }

    caminho->aresta_count = count;
    atual = caminho->destino;
    while (atual != caminho->origem) {
        GrafoAresta aresta = anteriores[atual];

        caminho->arestas[--count] = aresta;
        atual = grafo_aresta_get_origem(aresta);
    }

    return 1;
}

static int executar_dijkstra(Grafo grafo, struct caminho *caminho,
                             double *distancias, GrafoAresta *anteriores,
                             int *visitados, FilaPrioridade fila,
                             size_t vertice_count) {
    struct dijkstra_context ctx;
    int valor;
    double prioridade;

    ctx.criterio = caminho->criterio;
    ctx.distancias = distancias;
    ctx.anteriores = anteriores;
    ctx.visitados = visitados;
    ctx.fila = fila;
    ctx.vertice_count = vertice_count;
    ctx.falha = 0;

    distancias[caminho->origem] = 0.0;
    if (!fila_prioridade_push(fila, caminho->origem, 0.0)) {
        return 0;
    }

    while (!fila_prioridade_is_empty(fila)) {
        if (!fila_prioridade_pop(fila, &valor, &prioridade)) {
            return 0;
        }

        if (valor < 0 || (size_t)valor >= vertice_count || visitados[valor] ||
            prioridade > distancias[valor]) {
            continue;
        }

        visitados[valor] = 1;
        if (valor == caminho->destino) {
            break;
        }

        ctx.atual = valor;
        grafo_for_each_aresta_saida(grafo, valor, relaxar_aresta, &ctx);
        if (ctx.falha) {
            return 0;
        }
    }

    return 1;
}

Caminho caminho_calcular(Grafo grafo, GrafoVertice origem,
                         GrafoVertice destino, CaminhoCriterio criterio) {
    struct caminho *caminho;
    double *distancias = NULL;
    GrafoAresta *anteriores = NULL;
    int *visitados = NULL;
    FilaPrioridade fila = NULL;
    size_t vertice_count;

    if (!criterio_valido(criterio) || !vertice_valido(grafo, origem) ||
        !vertice_valido(grafo, destino)) {
        return NULL;
    }

    caminho = criar_caminho(origem, destino, criterio);
    if (caminho == NULL) {
        return NULL;
    }

    vertice_count = grafo_get_vertice_count(grafo);
    distancias = (double *)malloc(vertice_count * sizeof(double));
    anteriores = (GrafoAresta *)calloc(vertice_count, sizeof(GrafoAresta));
    visitados = (int *)calloc(vertice_count, sizeof(int));
    fila = fila_prioridade_create(vertice_count);
    if (distancias == NULL || anteriores == NULL || visitados == NULL ||
        fila == NULL) {
        caminho_destroy(caminho);
        free(distancias);
        free(anteriores);
        free(visitados);
        fila_prioridade_destroy(fila);
        return NULL;
    }

    for (size_t i = 0; i < vertice_count; i++) {
        distancias[i] = DBL_MAX;
    }

    if (!executar_dijkstra(grafo, caminho, distancias, anteriores, visitados,
                           fila, vertice_count)) {
        caminho_destroy(caminho);
        caminho = NULL;
    } else if (distancias[destino] < DBL_MAX) {
        caminho->existe = 1;
        caminho->custo = distancias[destino];
        if (!reconstruir_caminho(caminho, anteriores)) {
            caminho_destroy(caminho);
            caminho = NULL;
        }
    }

    free(distancias);
    free(anteriores);
    free(visitados);
    fila_prioridade_destroy(fila);
    return caminho;
}

int caminho_existe(Caminho caminho_ref) {
    struct caminho *caminho = (struct caminho *)caminho_ref;
    return caminho != NULL && caminho->existe;
}

CaminhoCriterio caminho_get_criterio(Caminho caminho_ref) {
    struct caminho *caminho = (struct caminho *)caminho_ref;
    return caminho != NULL ? caminho->criterio : CAMINHO_CRITERIO_COMPRIMENTO;
}

GrafoVertice caminho_get_origem(Caminho caminho_ref) {
    struct caminho *caminho = (struct caminho *)caminho_ref;
    return caminho != NULL ? caminho->origem : -1;
}

GrafoVertice caminho_get_destino(Caminho caminho_ref) {
    struct caminho *caminho = (struct caminho *)caminho_ref;
    return caminho != NULL ? caminho->destino : -1;
}

double caminho_get_custo(Caminho caminho_ref) {
    struct caminho *caminho = (struct caminho *)caminho_ref;
    return caminho != NULL && caminho->existe ? caminho->custo : 0.0;
}

size_t caminho_get_aresta_count(Caminho caminho_ref) {
    struct caminho *caminho = (struct caminho *)caminho_ref;
    return caminho != NULL ? caminho->aresta_count : 0;
}

GrafoAresta caminho_get_aresta(Caminho caminho_ref, size_t indice) {
    struct caminho *caminho = (struct caminho *)caminho_ref;

    if (caminho == NULL || indice >= caminho->aresta_count) {
        return NULL;
    }

    return caminho->arestas[indice];
}

void caminho_destroy(Caminho caminho_ref) {
    struct caminho *caminho = (struct caminho *)caminho_ref;

    if (caminho == NULL) {
        return;
    }

    free(caminho->arestas);
    free(caminho);
}
