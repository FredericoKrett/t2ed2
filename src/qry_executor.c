#include "qry_executor.h"

#include "quadra.h"

#include <stddef.h>

static int executar_origem(QryComando comando, QuadraStore quadras,
                           Registradores registradores) {
    const char *reg_nome = qry_comando_get_reg1(comando);
    const char *cep = qry_comando_get_cep(comando);
    int indice = registradores_parse_nome(reg_nome);
    Quadra quadra;
    double x;
    double y;

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
            !executar_origem(comando, quadras, registradores)) {
            return 0;
        }
    }

    return 1;
}
