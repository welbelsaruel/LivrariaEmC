#include "Livro.h"

int criarLivro(Livro registro) {
    Livro modelo = {0};
    const Campo campos[] = {
        {TEXTO_FIXO, (size_t)((char *)&modelo.matricula - (char *)&modelo), sizeof modelo.matricula},
        {TEXTO_DINAMICO, (size_t)((char *)&modelo.nome - (char *)&modelo), 0},
        {TEXTO_DINAMICO, (size_t)((char *)&modelo.curso - (char *)&modelo), 0},
        {INTEIRO, (size_t)((char *)&modelo.quantidade - (char *)&modelo), sizeof(int)},
    };
    const TipoRegistro tipo = {"Files/livro.bin", "LVR2", sizeof(Livro),
        campos, sizeof campos / sizeof campos[0], 0, 1, 0};
    if (registro.quantidade < 0) return -1;
    return CRIAR(&tipo, &registro);
}

int buscarLivro(const char *nome, FiltroRegistro filtro, void *contexto) {
    Livro modelo = {0};
    const Campo campos[] = {
        {TEXTO_FIXO, (size_t)((char *)&modelo.matricula - (char *)&modelo), sizeof modelo.matricula},
        {TEXTO_DINAMICO, (size_t)((char *)&modelo.nome - (char *)&modelo), 0},
        {TEXTO_DINAMICO, (size_t)((char *)&modelo.curso - (char *)&modelo), 0},
        {INTEIRO, (size_t)((char *)&modelo.quantidade - (char *)&modelo), sizeof(int)},
    };
    const TipoRegistro tipo = {"Files/livro.bin", "LVR2", sizeof(Livro),
        campos, sizeof campos / sizeof campos[0], 0, 1, 0};
    return BUSCAR(&tipo, nome, filtro, contexto);
}

int removerLivro(const char *matricula) {
    Livro modelo = {0};
    const Campo campos[] = {
        {TEXTO_FIXO, (size_t)((char *)&modelo.matricula - (char *)&modelo), sizeof modelo.matricula},
        {TEXTO_DINAMICO, (size_t)((char *)&modelo.nome - (char *)&modelo), 0},
        {TEXTO_DINAMICO, (size_t)((char *)&modelo.curso - (char *)&modelo), 0},
        {INTEIRO, (size_t)((char *)&modelo.quantidade - (char *)&modelo), sizeof(int)},
    };
    const TipoRegistro tipo = {"Files/livro.bin", "LVR2", sizeof(Livro),
        campos, sizeof campos / sizeof campos[0], 0, 1, 0};
    return REMOVER(&tipo, matricula);
}

