#ifndef LIVRO_H
#define LIVRO_H
#include "Funcao.h"

typedef struct {
    char matricula[11];
    char *nome;
    char *curso;
    int quantidade;
} Livro;

/* Retornos e contrato de memoria definidos em Funcao.h. */
int criarLivro(Livro registro);
int buscarLivro(const char *nome, FiltroRegistro filtro, void *contexto);
int removerLivro(const char *matricula);
#endif
