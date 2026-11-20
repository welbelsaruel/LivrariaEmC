#include <stdio.h>
#include <string.h>

#include "Emprestimo.h"
#include "User.h"
//#include "Livro.h"

typedef struct {
    int id;
    int pessoa_id;
    int livro_id;
    char data_emprestimo[11];
    char data_devolucao[11];
} Emprestimo;


// Busca empréstimos pelo nome da pessoa
void buscarEmprestimosPorNome(const char *nome) {

    User pessoa;

    if (!buscarPessoaPorNome(nome, &pessoa)) {
        printf("Pessoa nao encontrada!\n");
        return;
    }

    FILE *arquivo = fopen("dados/emprestimos.csv", "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir emprestimos.csv\n");
        return;
    }

    char linha[300];
    Emprestimo emprestimo;

    fgets(linha, sizeof(linha), arquivo);

    int encontrou = 0;

    printf("\nEmprestimos de %s:\n\n", pessoa.nome);

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        if (sscanf(linha, "%d,%d,%d,%10[^,],%10[^\n]",
                   &emprestimo.id,
                   &emprestimo.pessoa_id,
                   &emprestimo.livro_id,
                   emprestimo.data_emprestimo,
                   emprestimo.data_devolucao) == 5) {

            if (emprestimo.pessoa_id == pessoa.matricula) {

                //Livro livro;

                if (buscarLivroPorId(emprestimo.livro_id, &livro)) {

                    printf("Livro: %s\n", livro.titulo);
                    printf("Autor: %s\n", livro.autor);

                    printf("Emprestimo: %s\n",
                           emprestimo.data_emprestimo);

                    if (strcmp(emprestimo.data_devolucao, "-") == 0)
                        printf("Situacao: Emprestado\n");
                    else
                        printf("Devolvido em: %s\n",
                               emprestimo.data_devolucao);

                    printf("-------------------------\n");

                    encontrou = 1;
                }
            }
        }
    }

    if (!encontrou)
        printf("Nenhum emprestimo encontrado.\n");

    fclose(arquivo);
}


// Busca empréstimos pelo título do livro
void buscarEmprestimosPorTitulo(const char *titulo) {

    Livro livro;

    if (!buscarLivroPorTitulo(titulo, &livro)) {
        printf("Livro nao encontrado!\n");
        return;
    }

    FILE *arquivo = fopen("dados/emprestimos.csv", "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir emprestimos.csv\n");
        return;
    }

    char linha[300];
    Emprestimo emprestimo;

    fgets(linha, sizeof(linha), arquivo);

    int encontrou = 0;

    printf("\nHistorico de emprestimos de %s:\n\n",
           livro.titulo);

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        if (sscanf(linha, "%d,%d,%d,%10[^,],%10[^\n]",
                   &emprestimo.id,
                   &emprestimo.pessoa_id,
                   &emprestimo.livro_id,
                   emprestimo.data_emprestimo,
                   emprestimo.data_devolucao) == 5) {

            if (emprestimo.livro_id == livro.id) {

                Pessoa pessoa;

                if (buscarPessoaPorId(emprestimo.pessoa_id, &pessoa)) {

                    printf("Pessoa: %s\n", pessoa.nome);

                    printf("Emprestimo: %s\n",
                           emprestimo.data_emprestimo);

                    if (strcmp(emprestimo.data_devolucao, "-") == 0)
                        printf("Situacao: Emprestado\n");
                    else
                        printf("Devolvido em: %s\n",
                               emprestimo.data_devolucao);

                    printf("-------------------------\n");

                    encontrou = 1;
                }
            }
        }
    }

    if (!encontrou)
        printf("Nenhum emprestimo encontrado.\n");

    fclose(arquivo);
}