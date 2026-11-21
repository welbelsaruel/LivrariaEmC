#ifndef LIVRO_H
#define LIVRO_H

typedef struct {
    int id;
    int qtd;
    char titulo[150];
    char autor[100];
} Livro;

int buscarLivroPorTitulo(const char *titulo, Livro *livro);

#endif