#include "qry_parser.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 512
#define TOKEN_SIZE 128
#define QRY_COMANDOS_CAPACIDADE_INICIAL 8

struct qry_comando {
    QryComandoTipo tipo;
    char reg1[TOKEN_SIZE];
    char reg2[TOKEN_SIZE];
    char cep[TOKEN_SIZE];
    char face;
    char cor_curto[TOKEN_SIZE];
    char cor_rapido[TOKEN_SIZE];
    double num;
    double velocidade;
    double limite_velocidade;
    double x;
    double y;
    double w;
    double h;
};

struct qry_comandos {
    struct qry_comando *itens;
    size_t tamanho;
    size_t capacidade;
};

static int is_blank_line(const char *line) {
    while (*line != '\0') {
        if (*line != ' ' && *line != '\t' && *line != '\n' && *line != '\r') {
            return 0;
        }
        line++;
    }
    return 1;
}

static int is_face_valida(char face) {
    return face == 'N' || face == 'S' || face == 'L' || face == 'O';
}

static int garantir_capacidade(struct qry_comandos *comandos) {
    struct qry_comando *novos_itens;
    size_t nova_capacidade;

    if (comandos->tamanho < comandos->capacidade) {
        return 1;
    }

    nova_capacidade = comandos->capacidade * 2;
    if (nova_capacidade == 0) {
        nova_capacidade = QRY_COMANDOS_CAPACIDADE_INICIAL;
    }

    novos_itens = (struct qry_comando *)realloc(
        comandos->itens, nova_capacidade * sizeof(struct qry_comando));
    if (novos_itens == NULL) {
        return 0;
    }

    comandos->itens = novos_itens;
    comandos->capacidade = nova_capacidade;
    return 1;
}

static struct qry_comandos *comandos_create(void) {
    struct qry_comandos *comandos =
        (struct qry_comandos *)calloc(1, sizeof(struct qry_comandos));
    if (comandos == NULL) {
        return NULL;
    }

    comandos->capacidade = QRY_COMANDOS_CAPACIDADE_INICIAL;
    comandos->itens = (struct qry_comando *)malloc(
        comandos->capacidade * sizeof(struct qry_comando));
    if (comandos->itens == NULL) {
        free(comandos);
        return NULL;
    }

    return comandos;
}

static int comandos_append(struct qry_comandos *comandos,
                           const struct qry_comando *comando) {
    if (!garantir_capacidade(comandos)) {
        return 0;
    }

    comandos->itens[comandos->tamanho] = *comando;
    comandos->tamanho++;
    return 1;
}

static void comando_init(struct qry_comando *comando, QryComandoTipo tipo) {
    memset(comando, 0, sizeof(struct qry_comando));
    comando->tipo = tipo;
}

static int parse_origem(const char *line, struct qry_comando *comando) {
    char face_texto[TOKEN_SIZE];
    char extra[TOKEN_SIZE];
    int read;

    comando_init(comando, QRY_COMANDO_ORIGEM);

    read = sscanf(line, "%*s %127s %127s %127s %lf %127s",
                  comando->reg1, comando->cep, face_texto,
                  &comando->num, extra);
    if (read != 4 || face_texto[1] != '\0' ||
        !is_face_valida(face_texto[0]) || !isfinite(comando->num)) {
        return 0;
    }

    comando->face = face_texto[0];
    return 1;
}

static int parse_mvm(const char *line, struct qry_comando *comando) {
    char extra[TOKEN_SIZE];
    int read;

    comando_init(comando, QRY_COMANDO_MVM);

    read = sscanf(line, "%*s %lf %lf %lf %lf %lf %127s",
                  &comando->velocidade, &comando->x, &comando->y,
                  &comando->w, &comando->h, extra);
    return read == 5 && isfinite(comando->velocidade) &&
           isfinite(comando->x) && isfinite(comando->y) &&
           isfinite(comando->w) && isfinite(comando->h);
}

static int parse_limite_velocidade(const char *line,
                                   QryComandoTipo tipo,
                                   struct qry_comando *comando) {
    char extra[TOKEN_SIZE];
    int read;

    comando_init(comando, tipo);

    read = sscanf(line, "%*s %lf %127s", &comando->limite_velocidade, extra);
    return read == 1 && isfinite(comando->limite_velocidade);
}

static int parse_percurso(const char *line, struct qry_comando *comando) {
    char extra[TOKEN_SIZE];
    int read;

    comando_init(comando, QRY_COMANDO_PERCURSO);

    read = sscanf(line, "%*s %127s %127s %127s %127s %127s",
                  comando->reg1, comando->reg2, comando->cor_curto,
                  comando->cor_rapido, extra);
    return read == 4;
}

static int parse_line(const char *line, struct qry_comando *comando) {
    char command[TOKEN_SIZE];

    if (sscanf(line, "%127s", command) != 1) {
        return 0;
    }

    if (strcmp(command, "@o?") == 0) {
        return parse_origem(line, comando);
    }
    if (strcmp(command, "mvm") == 0) {
        return parse_mvm(line, comando);
    }
    if (strcmp(command, "regs") == 0) {
        return parse_limite_velocidade(line, QRY_COMANDO_REGS, comando);
    }
    if (strcmp(command, "exp") == 0) {
        return parse_limite_velocidade(line, QRY_COMANDO_EXP, comando);
    }
    if (strcmp(command, "p?") == 0) {
        return parse_percurso(line, comando);
    }

    return 0;
}

QryComandos qry_parser_parse_file(const char *filepath) {
    FILE *file;
    char line[LINE_SIZE];
    struct qry_comandos *comandos;

    if (filepath == NULL) {
        return NULL;
    }

    file = fopen(filepath, "r");
    if (file == NULL) {
        return NULL;
    }

    comandos = comandos_create();
    if (comandos == NULL) {
        fclose(file);
        return NULL;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        struct qry_comando comando;

        if (is_blank_line(line)) {
            continue;
        }

        if (!parse_line(line, &comando) || !comandos_append(comandos, &comando)) {
            qry_comandos_destroy(comandos);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    return comandos;
}

size_t qry_comandos_count(QryComandos comandos_ref) {
    struct qry_comandos *comandos = (struct qry_comandos *)comandos_ref;
    return comandos != NULL ? comandos->tamanho : 0;
}

QryComando qry_comandos_get(QryComandos comandos_ref, size_t indice) {
    struct qry_comandos *comandos = (struct qry_comandos *)comandos_ref;

    if (comandos == NULL || indice >= comandos->tamanho) {
        return NULL;
    }

    return &comandos->itens[indice];
}

QryComandoTipo qry_comando_get_tipo(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL ? comando->tipo : QRY_COMANDO_ORIGEM;
}

const char *qry_comando_get_reg1(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL && comando->reg1[0] != '\0' ? comando->reg1 : NULL;
}

const char *qry_comando_get_reg2(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL && comando->reg2[0] != '\0' ? comando->reg2 : NULL;
}

const char *qry_comando_get_cep(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL && comando->cep[0] != '\0' ? comando->cep : NULL;
}

char qry_comando_get_face(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL ? comando->face : '\0';
}

double qry_comando_get_num(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL ? comando->num : 0.0;
}

double qry_comando_get_velocidade(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL ? comando->velocidade : 0.0;
}

double qry_comando_get_limite_velocidade(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL ? comando->limite_velocidade : 0.0;
}

double qry_comando_get_x(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL ? comando->x : 0.0;
}

double qry_comando_get_y(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL ? comando->y : 0.0;
}

double qry_comando_get_w(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL ? comando->w : 0.0;
}

double qry_comando_get_h(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL ? comando->h : 0.0;
}

const char *qry_comando_get_cor_curto(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL && comando->cor_curto[0] != '\0'
               ? comando->cor_curto
               : NULL;
}

const char *qry_comando_get_cor_rapido(QryComando comando_ref) {
    struct qry_comando *comando = (struct qry_comando *)comando_ref;
    return comando != NULL && comando->cor_rapido[0] != '\0'
               ? comando->cor_rapido
               : NULL;
}

void qry_comandos_destroy(QryComandos comandos_ref) {
    struct qry_comandos *comandos = (struct qry_comandos *)comandos_ref;

    if (comandos == NULL) {
        return;
    }

    free(comandos->itens);
    free(comandos);
}
