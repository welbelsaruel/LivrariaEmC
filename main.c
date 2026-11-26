#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes/User.h"
#include "Includes/Livro.h"

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

static int lerLinhaDinamica(const char *mensagem, char **destino) {
    size_t capacidade = 32, tamanho = 0;
    char *texto = malloc(capacidade);
    int caractere;
    fputs(mensagem, stdout);
    fflush(stdout);
    if (texto == NULL) return 0;
    while ((caractere = getchar()) != '\n' && caractere != EOF) {
        if (tamanho == capacidade - 1) {
            char *novo = NULL;
            if (capacidade <= (size_t)-1 / 2)
                novo = realloc(texto, capacidade * 2);
            if (novo == NULL) {
                free(texto);
                while ((caractere = getchar()) != '\n' && caractere != EOF) {}
                return 0;
            }
            texto = novo;
            capacidade *= 2;
        }
        texto[tamanho++] = (char)caractere;
    }
    if (tamanho == 0 || ferror(stdin)) {
        free(texto);
        return 0;
    }
    texto[tamanho] = '\0';
    char *ajustado = realloc(texto, tamanho + 1);
    *destino = ajustado != NULL ? ajustado : texto;
    return 1;
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

typedef struct {
    const char *descricao;
    const char *trecho;
    int campo;
    size_t encontrados;
} ContextoBusca;

static int mostrarCorrespondencia(const void *registro, void *contexto) {
    const User *usuario = registro;
    ContextoBusca *busca = contexto;
    const char *valor = busca->campo == 1 ? usuario->matricula :
                        busca->campo == 2 ? usuario->nome : usuario->curso;
    if (!contemTexto(valor, busca->trecho)) return 0;
    printf("%s: %s | Matricula: %s\n", busca->descricao, valor, usuario->matricula);
    busca->encontrados++;
    return 1;
}

static void buscarNomes(const char *descricao, int campo) {
    char *trecho = NULL;
    printf("\n--- Buscar por %s ---\n", descricao);
    printf("Digite parte do %s: ", descricao);
    if (!lerLinhaDinamica("", &trecho)) return;
    ContextoBusca contexto = {descricao, trecho, campo, 0};
    int resultado = buscarUser(NULL, mostrarCorrespondencia, &contexto);
    free(trecho);
    if (resultado < 0) puts("Nao foi possivel ler a base de usuarios.");
    else if (!contexto.encontrados) puts("Nenhum usuario encontrado.");
    else printf("Total encontrado: %zu\n", contexto.encontrados);
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

    if (removerUser(matricula) == 1) {
        puts("Usuario excluido com sucesso!");
    } else {
        puts("Usuario nao encontrado ou nao foi possivel exclui-lo.");
    }
}

static int mostrarLivro(const void *registro, void *contexto) {
    const Livro *livro = registro;
    ContextoBusca *busca = contexto;
    const char *valor = busca->campo == 1 ? livro->codigo :
                        busca->campo == 2 ? livro->nome : livro->autor;
    if (!contemTexto(valor, busca->trecho)) return 0;
    printf("Nome: %s | Matricula: %s | Curso: %s | Quantidade: %d\n",
           livro->nome, livro->codigo, livro->autor, livro->quantidade);
    busca->encontrados++;
    return 1;
}

static void menuBuscarLivro(void) {
    char opcao[TAMANHO_LINHA];
    puts("\n--- Buscar livro ---");
    puts("1 - Por codigo");
    puts("2 - Por nome");
    puts("3 - Por autor");
    puts("0 - Voltar");
    if (!lerLinha("Opcao: ", opcao, sizeof opcao) || !strcmp(opcao, "0")) return;
    int campo;
    if (!strcmp(opcao, "1")) campo = 1;
    else if (!strcmp(opcao, "2")) campo = 2;
    else if (!strcmp(opcao, "3")) campo = 3;
    else { puts("Opcao invalida."); return; }
    const char *descricao = campo == 1 ? "matricula" : campo == 2 ? "nome" : "curso";
    char *trecho = NULL;
    printf("Digite parte do %s: ", descricao);
    if (!lerLinhaDinamica("", &trecho)) return;
    ContextoBusca contexto = {descricao, trecho, campo, 0};
    int resultado = buscarLivro(NULL, mostrarLivro, &contexto);
    free(trecho);
    if (resultado < 0) puts("Nao foi possivel ler a base de livros.");
    else if (!contexto.encontrados) puts("Nenhum livro encontrado.");
    else printf("Total encontrado: %zu\n", contexto.encontrados);
}

static void cadastrarLivro(void) {
    Livro livro = {0};
    char entrada[TAMANHO_LINHA];
    puts("\n--- Criar livro ---");
    if (!lerLinha("Codigo: ", livro.codigo, sizeof livro.codigo) ||
        !lerLinhaDinamica("Nome: ", &livro.nome) ||
        !lerLinhaDinamica("Autor: ", &livro.autor) ||
        !lerLinha("Quantidade: ", entrada, sizeof entrada)) goto fim;
    /* Converte somente digitos e verifica o limite antes de multiplicar. */
    unsigned int quantidade = 0;
    const unsigned int maximo = (~0u) >> 1;
    for (size_t i = 0; entrada[i]; i++) {
        if (entrada[i] < '0' || entrada[i] > '9' ||
            quantidade > (maximo - (unsigned int)(entrada[i] - '0')) / 10) {
            puts("Quantidade invalida. Digite um inteiro nao negativo dentro do limite de int.");
            goto fim;
        }
        quantidade = quantidade * 10 + (unsigned int)(entrada[i] - '0');
    }
    livro.quantidade = (int)quantidade;
    int resultado = criarLivro(livro);
    if (resultado == 1) puts("Livro criado com sucesso!");
    else if (resultado == 0) puts("Ja existe um livro com esse codigo.");
    else puts("Nao foi possivel gravar o livro.");
fim:
    free(livro.nome);
    free(livro.autor);
}

static void excluirLivro(void) {
    char matricula[sizeof(((Livro *)0)->codigo)];
    puts("\n--- Excluir livro ---");
    if (!lerLinha("Matricula: ", matricula, sizeof matricula)) return;
    int resultado = removerLivro(matricula);
    if (resultado == 1) puts("Livro excluido com sucesso!");
    else if (resultado == 0) puts("Livro nao encontrado.");
    else puts("Nao foi possivel excluir o livro.");
}

int main(void) {
    char opcao[TAMANHO_LINHA];

    for (;;) {
        puts("\n========== LIVRARIA ==========");
        puts("1 - Buscar usuario");
        puts("2 - Criar usuario");
        puts("3 - Excluir usuario");
        puts("4 - Buscar livro");
        puts("5 - Criar livro");
        puts("6 - Excluir livro");
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
                lerLinhaDinamica("Nome: ", &usuario.nome) &&
                lerLinhaDinamica("Curso: ", &usuario.curso)) {
                int resultado = criarUser(usuario);
                if (resultado == 1) {
                    puts("Usuario criado com sucesso!");
                } else if (resultado == 0) {
                    puts("Ja existe um usuario com essa matricula.");
                } else {
                    puts("Nao foi possivel gravar o usuario.");
                }
            }
            free(usuario.nome);
            free(usuario.curso);
        } else if (strcmp(opcao, "3") == 0) {
            excluirUsuario();
        } else if (strcmp(opcao, "4") == 0) {
            menuBuscarLivro();
        } else if (strcmp(opcao, "5") == 0) {
            cadastrarLivro();
        } else if (strcmp(opcao, "6") == 0) {
            excluirLivro();
        } else if (strcmp(opcao, "0") == 0) {
            return EXIT_SUCCESS;
        } else {
            puts("Opcao invalida.");
        }
    }
}
