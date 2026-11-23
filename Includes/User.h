#ifndef USER_H
#define USER_H

typedef struct {
    char matricula[11];
    char nome[100];
    char curso[100];
} User;

int criarUser(User user);

/* As buscas retornam 1 e preenchem pessoa com a primeira correspondencia. */
int buscarUMatricula(const char *matricula, User *pessoa);
int buscarUNome(const char *nome, User *pessoa);
int buscarUCurso(const char *curso, User *pessoa);

typedef int (*FuncaoBuscaUser)(const char *valor, User *pessoa);

/* Passe buscarUMatricula, buscarUNome ou buscarUCurso como funcaoBusca. */
int buscarUser(const char *valor, User *pessoa, FuncaoBuscaUser funcaoBusca);

/* Remove a primeira correspondencia pela matricula. */
int removerUser(const char *matricula);

/* Zera a struct fornecida pelo chamador. */
void liberarUser(User *user);

#endif
