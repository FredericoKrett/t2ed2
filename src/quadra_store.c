#include "quadra_store.h"

#include <stdlib.h>
#include <string.h>

#define DEFAULT_CAPACITY 17

struct quadra_node {
    Quadra quadra;
    struct quadra_node *next;
};

struct quadra_store {
    struct quadra_node **buckets;
    size_t capacity;
    size_t count;
};

static size_t hash_text(const char *text) {
    size_t hash = 5381u;
    unsigned char c;

    while ((c = (unsigned char)*text) != '\0') {
        hash = ((hash << 5) + hash) + c;
        text++;
    }

    return hash;
}

static size_t normalize_capacity(size_t capacity_hint) {
    if (capacity_hint == 0) {
        return DEFAULT_CAPACITY;
    }

    return capacity_hint;
}

static struct quadra_node *find_node(struct quadra_store *store, const char *cep,
                                     size_t *out_index,
                                     struct quadra_node ***out_link) {
    size_t index;
    struct quadra_node **link;

    if (store == NULL || cep == NULL || store->capacity == 0) {
        return NULL;
    }

    index = hash_text(cep) % store->capacity;
    link = &store->buckets[index];

    while (*link != NULL) {
        const char *node_cep = quadra_get_cep((*link)->quadra);
        if (node_cep != NULL && strcmp(node_cep, cep) == 0) {
            if (out_index != NULL) {
                *out_index = index;
            }
            if (out_link != NULL) {
                *out_link = link;
            }
            return *link;
        }
        link = &(*link)->next;
    }

    if (out_index != NULL) {
        *out_index = index;
    }
    if (out_link != NULL) {
        *out_link = link;
    }
    return NULL;
}

QuadraStore quadra_store_create(size_t capacity_hint) {
    struct quadra_store *store = (struct quadra_store *)calloc(1, sizeof(struct quadra_store));
    if (store == NULL) {
        return NULL;
    }

    store->capacity = normalize_capacity(capacity_hint);
    store->buckets = (struct quadra_node **)calloc(store->capacity, sizeof(struct quadra_node *));
    if (store->buckets == NULL) {
        free(store);
        return NULL;
    }

    return store;
}

int quadra_store_insert(QuadraStore store_ref, Quadra quadra) {
    struct quadra_store *store = (struct quadra_store *)store_ref;
    const char *cep = quadra_get_cep(quadra);
    size_t index;
    struct quadra_node *node;

    if (store == NULL || quadra == NULL || cep == NULL) {
        return 0;
    }

    if (find_node(store, cep, &index, NULL) != NULL) {
        return 0;
    }

    node = (struct quadra_node *)malloc(sizeof(struct quadra_node));
    if (node == NULL) {
        return 0;
    }

    node->quadra = quadra;
    node->next = store->buckets[index];
    store->buckets[index] = node;
    store->count++;

    return 1;
}

Quadra quadra_store_find(QuadraStore store_ref, const char *cep) {
    struct quadra_store *store = (struct quadra_store *)store_ref;
    struct quadra_node *node = find_node(store, cep, NULL, NULL);

    return node != NULL ? node->quadra : NULL;
}

Quadra quadra_store_remove(QuadraStore store_ref, const char *cep) {
    struct quadra_store *store = (struct quadra_store *)store_ref;
    struct quadra_node **link = NULL;
    struct quadra_node *node = find_node(store, cep, NULL, &link);
    Quadra quadra;

    if (node == NULL || link == NULL) {
        return NULL;
    }

    *link = node->next;
    quadra = node->quadra;
    free(node);
    store->count--;

    return quadra;
}

size_t quadra_store_count(QuadraStore store_ref) {
    struct quadra_store *store = (struct quadra_store *)store_ref;
    return store != NULL ? store->count : 0;
}

void quadra_store_for_each(QuadraStore store_ref, QuadraStoreVisitor visitor,
                           void *context) {
    struct quadra_store *store = (struct quadra_store *)store_ref;

    if (store == NULL || visitor == NULL) {
        return;
    }

    for (size_t i = 0; i < store->capacity; i++) {
        struct quadra_node *node = store->buckets[i];
        while (node != NULL) {
            visitor(node->quadra, context);
            node = node->next;
        }
    }
}

void quadra_store_destroy(QuadraStore store_ref) {
    struct quadra_store *store = (struct quadra_store *)store_ref;
    if (store == NULL) {
        return;
    }

    for (size_t i = 0; i < store->capacity; i++) {
        struct quadra_node *node = store->buckets[i];
        while (node != NULL) {
            struct quadra_node *next = node->next;
            quadra_destroy(node->quadra);
            free(node);
            node = next;
        }
    }

    free(store->buckets);
    free(store);
}
