#include "qry_executor.h"

#include "quadra.h"

#include <stddef.h>

int qry_executor_resolve_origem(QryComando comando, QuadraStore quadras,
                                Registradores registradores) {
    const char *reg_nome;
    const char *cep;
    int indice;
    Quadra quadra;
    double x;
    double y;

    if (comando == NULL || quadras == NULL || registradores == NULL ||
        qry_comando_get_tipo(comando) != QRY_COMANDO_ORIGEM) {
        return 0;
    }

    reg_nome = qry_comando_get_reg1(comando);
    cep = qry_comando_get_cep(comando);
    indice = registradores_parse_nome(reg_nome);
    if (indice < 0 || cep == NULL) {
        return 0;
    }

    quadra = quadra_store_find(quadras, cep);
    if (quadra == NULL) {
        return 0;
    }

    if (!quadra_get_address_point(quadra, qry_comando_get_face(comando),
                                  qry_comando_get_num(comando), &x, &y)) {
        return 0;
    }

    return registradores_set(registradores, indice, x, y);
}

static int obter_coordenada_registrador(Registradores registradores,
                                        const char *nome,
                                        double *out_x,
                                        double *out_y) {
    int indice = registradores_parse_nome(nome);

    if (indice < 0) {
        return 0;
    }

    return registradores_get(registradores, indice, out_x, out_y);
}

int qry_executor_resolve_origens(QryComandos comandos, QuadraStore quadras,
                                 Registradores registradores) {
    size_t count;

    if (comandos == NULL || quadras == NULL || registradores == NULL) {
        return 0;
    }

    count = qry_comandos_count(comandos);
    for (size_t i = 0; i < count; i++) {
        QryComando comando = qry_comandos_get(comandos, i);

        if (comando == NULL) {
            return 0;
        }

        if (qry_comando_get_tipo(comando) == QRY_COMANDO_ORIGEM &&
            !qry_executor_resolve_origem(comando, quadras, registradores)) {
            return 0;
        }
    }

    return 1;
}

int qry_executor_aplicar_mvm(QryComando comando, Grafo grafo) {
    if (comando == NULL || grafo == NULL ||
        qry_comando_get_tipo(comando) != QRY_COMANDO_MVM) {
        return 0;
    }

    return grafo_atualizar_vm_regiao(grafo,
                                     qry_comando_get_velocidade(comando),
                                     qry_comando_get_x(comando),
                                     qry_comando_get_y(comando),
                                     qry_comando_get_w(comando),
                                     qry_comando_get_h(comando)) >= 0;
}

GrafoComponentes qry_executor_calcular_regs(QryComando comando, Grafo grafo) {
    if (comando == NULL || grafo == NULL ||
        qry_comando_get_tipo(comando) != QRY_COMANDO_REGS) {
        return NULL;
    }

    return grafo_calcular_componentes_lentos(
        grafo, qry_comando_get_limite_velocidade(comando));
}

GrafoArestas qry_executor_aplicar_exp(QryComando comando, Grafo grafo) {
    if (comando == NULL || grafo == NULL ||
        qry_comando_get_tipo(comando) != QRY_COMANDO_EXP) {
        return NULL;
    }

    return grafo_aplicar_expansao_agm(
        grafo, qry_comando_get_limite_velocidade(comando));
}

int qry_executor_calcular_percurso(QryComando comando, Grafo grafo,
                                   Registradores registradores,
                                   Caminho *out_curto,
                                   Caminho *out_rapido) {
    double origem_x;
    double origem_y;
    double destino_x;
    double destino_y;
    GrafoVertice origem;
    GrafoVertice destino;
    Caminho curto;
    Caminho rapido;

    if (comando == NULL || grafo == NULL || registradores == NULL ||
        out_curto == NULL || out_rapido == NULL ||
        qry_comando_get_tipo(comando) != QRY_COMANDO_PERCURSO) {
        return 0;
    }

    *out_curto = NULL;
    *out_rapido = NULL;

    if (!obter_coordenada_registrador(registradores,
                                      qry_comando_get_reg1(comando),
                                      &origem_x, &origem_y) ||
        !obter_coordenada_registrador(registradores,
                                      qry_comando_get_reg2(comando),
                                      &destino_x, &destino_y)) {
        return 0;
    }

    origem = grafo_find_vertice_mais_proximo(grafo, origem_x, origem_y);
    destino = grafo_find_vertice_mais_proximo(grafo, destino_x, destino_y);
    if (origem < 0 || destino < 0) {
        return 0;
    }

    curto = caminho_calcular(grafo, origem, destino,
                             CAMINHO_CRITERIO_COMPRIMENTO);
    rapido = caminho_calcular(grafo, origem, destino, CAMINHO_CRITERIO_TEMPO);
    if (curto == NULL || rapido == NULL) {
        caminho_destroy(curto);
        caminho_destroy(rapido);
        return 0;
    }

    *out_curto = curto;
    *out_rapido = rapido;
    return 1;
}
