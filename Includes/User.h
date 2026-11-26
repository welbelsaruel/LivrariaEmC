#ifndef USER_H
#define USER_H
#include "Funcao.h"

typedef struct {
    char matricula[11];
    char *nome;
    char *curso;
} User;

/* Retornos e contrato de memoria definidos em Funcao.h. */
int criarUser(User registro);
int buscarUser(const char *nome, FiltroRegistro filtro, void *contexto);
int removerUser(const char *matricula);
#endif
