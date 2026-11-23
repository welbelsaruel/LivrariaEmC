#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes/User.h"

#define TAMANHO_LINHA 128

static int lerLinha(const char *mensagem, char *destino, size_t capacidade) {
    size_t tamanho;

    fputs(mensagem, stdout);
    fflush(stdout);

    if (fgets(destino, capacidade, stdin) == NULL) {
        return 0;
    }

    tamanho = strlen(destino);
    if (tamanho > 0 && destino[tamanho - 1] == '\n') {
        destino[tamanho - 1] = '\0';
    } else {
        int caractere;
        while ((caractere = getchar()) != '\n' && caractere != EOF) {
            /* Descarta o restante de uma entrada maior que o limite. */
        }
        fputs("Entrada muito grande. Tente novamente.\n", stdout);
        return 0;
    }

    return destino[0] != '\0';
}

static char minusculo(char caractere) {
    if (caractere >= 'A' && caractere <= 'Z') {
        return (char)(caractere + ('a' - 'A'));
    }
    return caractere;
}

static int contemTexto(const char *texto, const char *trecho) {
    size_t indiceTexto;
    size_t indiceTrecho;
    size_t tamanhoTexto = strlen(texto);
    size_t tamanhoTrecho = strlen(trecho);

    if (tamanhoTrecho == 0 || tamanhoTrecho > tamanhoTexto) {
        return 0;
    }

    for (indiceTexto = 0; indiceTexto + tamanhoTrecho <= tamanhoTexto; indiceTexto++) {
        for (indiceTrecho = 0; indiceTrecho < tamanhoTrecho; indiceTrecho++) {
            if (minusculo(texto[indiceTexto + indiceTrecho]) !=
                minusculo(trecho[indiceTrecho])) {
                break;
            }
        }
        if (indiceTrecho == tamanhoTrecho) {
            return 1;
        }
    }

    return 0;
}

static void buscarNomes(const char *descricao, int campo) {
    char trecho[sizeof(((User *)0)->nome)];
    FILE *arquivo;
    User usuario;
    const char *valorUsuario;
    int encontrados = 0;

    printf("\n--- Buscar por %s ---\n", descricao);
    printf("Digite parte do %s: ", descricao);
    if (!lerLinha("", trecho, sizeof trecho)) {
        return;
    }

    arquivo = fopen("Files/user.bin", "rb");
    if (arquivo == NULL) {
        puts("Nao foi possivel abrir a base de usuarios.");
        return;
    }

    while (fread(&usuario, sizeof usuario, 1, arquivo) == 1) {
        usuario.matricula[sizeof usuario.matricula - 1] = '\0';
        usuario.nome[sizeof usuario.nome - 1] = '\0';
        usuario.curso[sizeof usuario.curso - 1] = '\0';

        if (campo == 1) {
            valorUsuario = usuario.matricula;
        } else if (campo == 2) {
            valorUsuario = usuario.nome;
        } else {
            valorUsuario = usuario.curso;
        }

        if (contemTexto(valorUsuario, trecho)) {
            printf("%s: %s | Matricula: %s\n",
                   descricao, valorUsuario, usuario.matricula);
            encontrados++;
        }
    }

    fclose(arquivo);
    if (encontrados == 0) {
        puts("Nenhum usuario encontrado.");
    } else {
        printf("Total encontrado: %d\n", encontrados);
    }
}

static void menuBuscar(void) {
    char opcao[TAMANHO_LINHA];

    puts("\n--- Buscar usuario ---");
    puts("1 - Por matricula");
    puts("2 - Por nome");
    puts("3 - Por curso");
    puts("0 - Voltar");

    if (!lerLinha("Opcao: ", opcao, sizeof opcao) || strcmp(opcao, "0") == 0) {
        return;
    }

    if (strcmp(opcao, "1") == 0) {
        buscarNomes("matricula", 1);
        return;
    } else if (strcmp(opcao, "2") == 0) {
        buscarNomes("nome", 2);
        return;
    } else if (strcmp(opcao, "3") == 0) {
        buscarNomes("curso", 3);
        return;
    } else {
        puts("Opcao invalida.");
        return;
    }
}

static void excluirUsuario(void) {
    char matricula[sizeof(((User *)0)->matricula)];

    puts("\n--- Excluir usuario ---");
    if (!lerLinha("Matricula: ", matricula, sizeof matricula)) {
        return;
    }

    if (removerUser(matricula)) {
        puts("Usuario excluido com sucesso!");
    } else {
        puts("Usuario nao encontrado ou nao foi possivel exclui-lo.");
    }
}

int main(void) {
    char opcao[TAMANHO_LINHA];

    for (;;) {
        puts("\n========== LIVRARIA ==========");
        puts("1 - Buscar usuario");
        puts("2 - Criar usuario");
        puts("3 - Excluir usuario");
        puts("0 - Sair");

        if (!lerLinha("Opcao: ", opcao, sizeof opcao)) {
            if (feof(stdin)) {
                putchar('\n');
                return EXIT_SUCCESS;
            }
            continue;
        }

        if (strcmp(opcao, "1") == 0) {
            menuBuscar();
        } else if (strcmp(opcao, "2") == 0) {
            User usuario = {0};

            puts("\n--- Criar usuario ---");
            if (lerLinha("Matricula: ", usuario.matricula, sizeof usuario.matricula) &&
                lerLinha("Nome: ", usuario.nome, sizeof usuario.nome) &&
                lerLinha("Curso: ", usuario.curso, sizeof usuario.curso)) {
                if (criarUser(usuario)) {
                    puts("Usuario criado com sucesso!");
                } else {
                    puts("Nao foi possivel criar o usuario. A matricula pode ja existir.");
                }
            }
        } else if (strcmp(opcao, "3") == 0) {
            excluirUsuario();
        } else if (strcmp(opcao, "0") == 0) {
            return EXIT_SUCCESS;
        } else {
            puts("Opcao invalida.");
        }
    }
}
