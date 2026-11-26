#ifndef LIVRO_H
#define LIVRO_H

typedef struct {
    char *codigo;
    char *titulo;
    char *autor;
    int quantidade;
} Livro;

int inserirLivro(const Livro *livro);
int buscarLivroPorTitulo(const char *titulo);
int buscarLivroPorAutor(const char *autor);
int obterLivroPorCodigo(
    const char *codigo,
    char titulo[],
    int capacidadeTitulo,
    char autor[],
    int capacidadeAutor,
    int *quantidade
);
int removerLivro(const char *codigo);

#endif
