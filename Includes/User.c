#include "User.h"

int criarUser(User registro) {
    User modelo = {0};
    const Campo campos[] = {
        {TEXTO_FIXO, (size_t)((char *)&modelo.matricula - (char *)&modelo), sizeof modelo.matricula},
        {TEXTO_DINAMICO, (size_t)((char *)&modelo.nome - (char *)&modelo), 0},
        {TEXTO_DINAMICO, (size_t)((char *)&modelo.curso - (char *)&modelo), 0},
    };
    const TipoRegistro tipo = {"Files/user.bin", "USR2", sizeof(User),
        campos, sizeof campos / sizeof campos[0], 0, 1, 1};
    return CRIAR(&tipo, &registro);
}

int buscarUser(const char *nome, FiltroRegistro filtro, void *contexto) {
    User modelo = {0};
    const Campo campos[] = {
        {TEXTO_FIXO, (size_t)((char *)&modelo.matricula - (char *)&modelo), sizeof modelo.matricula},
        {TEXTO_DINAMICO, (size_t)((char *)&modelo.nome - (char *)&modelo), 0},
        {TEXTO_DINAMICO, (size_t)((char *)&modelo.curso - (char *)&modelo), 0},
    };
    const TipoRegistro tipo = {"Files/user.bin", "USR2", sizeof(User),
        campos, sizeof campos / sizeof campos[0], 0, 1, 1};
    return BUSCAR(&tipo, nome, filtro, contexto);
}

int removerUser(const char *matricula) {
    User modelo = {0};
    const Campo campos[] = {
        {TEXTO_FIXO, (size_t)((char *)&modelo.matricula - (char *)&modelo), sizeof modelo.matricula},
        {TEXTO_DINAMICO, (size_t)((char *)&modelo.nome - (char *)&modelo), 0},
        {TEXTO_DINAMICO, (size_t)((char *)&modelo.curso - (char *)&modelo), 0},
    };
    const TipoRegistro tipo = {"Files/user.bin", "USR2", sizeof(User),
        campos, sizeof campos / sizeof campos[0], 0, 1, 1};
    return REMOVER(&tipo, matricula);
}