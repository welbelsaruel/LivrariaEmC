#include <stdio.h>
#include <string.h>
#include "Livro.h"

int buscarLivroPorTitulo(const char *titulo, Livro *livro) {

    FILE *arquivo = fopen("dados/livros.csv", "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir livros.csv\n");
        return 0;
    }

    char linha[300];

    fgets(linha, sizeof(linha), arquivo);

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        if (sscanf(linha, "%d,%149[^,],%99[^\n]",
                   &livro->id,
                   livro->titulo,
                   livro->autor) == 3) {

            if (strcmp(livro->titulo, titulo) == 0) {
                fclose(arquivo);
                return 1;
            }
        }
    }

    fclose(arquivo);
    return 0;
}