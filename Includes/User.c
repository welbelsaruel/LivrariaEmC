#include <stdio.h>
#include <string.h>
#include "User.h"

int buscarPessoaPorNome(const char *nome, User *usuario) {

    FILE *arquivo = fopen("../Sources/db.csv", "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir pessoas.csv\n");
        return 0;
    }

    char linha[200];

    // Ignora o cabeçalho
    fgets(linha, sizeof(linha), arquivo);

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        if (sscanf(linha, "%d,%99[^\n]",
                   &usuario->matricula,
                   usuario->nome) == 2) {

            if (strcmp(usuario->nome, nome) == 0) {
                fclose(arquivo);
                return 1;
            }
        }
    }

    fclose(arquivo);
    return 0;
}