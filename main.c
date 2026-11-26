#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Include/Aluno.h"
#include "Include/Emprestimo.h"
#include "Include/Livro.h"

#define TAMANHO_ENTRADA 256

/* Le uma entrada curta, remove a quebra de linha e rejeita texto vazio ou maior que o buffer. */
static int lerEntrada(
    const char *mensagem,
    char destino[],
    int capacidade
) {
    int caractere;
    int tamanho;

    fputs(mensagem, stdout);
    fflush(stdout);

    if (fgets(destino, capacidade, stdin) == NULL) {
        return 0;
    }

    tamanho = (int)strlen(destino);
    if (tamanho > 0 && destino[tamanho - 1] == '\n') {
        destino[tamanho - 1] = '\0';
    } else if (!feof(stdin)) {
        while ((caractere = getchar()) != '\n' && caractere != EOF) {
        }
        puts("Entrada muito grande.");
        return 0;
    }

    if (destino[0] == '\0') {
        puts("A entrada nao pode ficar vazia.");
        return 0;
    }

    return 1;
}

/* Converte uma sequencia de digitos em int sem aceitar negativos ou valores acima do limite. */
static int converterQuantidade(const char *texto, int *quantidade) {
    unsigned int valor = 0;
    unsigned int maximo = (~0u) >> 1;
    int indice = 0;

    if (texto[0] == '\0') {
        return 0;
    }

    while (texto[indice] != '\0') {
        unsigned int digito;

        if (texto[indice] < '0' || texto[indice] > '9') {
            return 0;
        }

        digito = (unsigned int)(texto[indice] - '0');
        if (valor > (maximo - digito) / 10) {
            return 0;
        }

        valor = valor * 10 + digito;
        indice++;
    }

    *quantidade = (int)valor;
    return 1;
}

/* Le uma entrada de qualquer tamanho e aumenta o bloco de memoria conforme necessario. */
static char *lerEntradaDinamica(const char *mensagem) {
    size_t capacidade = 32;
    size_t tamanho = 0;
    char *texto = malloc(capacidade);
    int caractere;

    fputs(mensagem, stdout);
    fflush(stdout);

    if (texto == NULL) {
        return NULL;
    }

    while ((caractere = getchar()) != '\n' && caractere != EOF) {
        if (tamanho + 1 >= capacidade) {
            size_t novaCapacidade;
            char *novoTexto;

            if (capacidade > (size_t)-1 / 2) {
                free(texto);
                return NULL;
            }

            novaCapacidade = capacidade * 2;
            novoTexto = realloc(texto, novaCapacidade);
            if (novoTexto == NULL) {
                free(texto);
                return NULL;
            }

            texto = novoTexto;
            capacidade = novaCapacidade;
        }

        texto[tamanho] = (char)caractere;
        tamanho++;
    }

    if (caractere == EOF && tamanho == 0) {
        free(texto);
        return NULL;
    }

    if (tamanho == 0) {
        puts("A entrada nao pode ficar vazia.");
        free(texto);
        return NULL;
    }

    texto[tamanho] = '\0';

    {
        char *textoAjustado = realloc(texto, tamanho + 1);
        if (textoAjustado != NULL) {
            texto = textoAjustado;
        }
    }

    return texto;
}

/* Coleta os dados dinamicamente, solicita a insercao e libera toda a memoria reservada. */
static void inserirNovoAluno(void) {
    Aluno aluno = {NULL, NULL, NULL};
    int resultado;

    aluno.matricula = lerEntradaDinamica("Matricula: ");
    if (aluno.matricula != NULL) {
        aluno.nome = lerEntradaDinamica("Nome: ");
    }
    if (aluno.nome != NULL) {
        aluno.turma = lerEntradaDinamica("Turma: ");
    }

    if (aluno.matricula == NULL
        || aluno.nome == NULL
        || aluno.turma == NULL) {
        puts("Cadastro cancelado.");
    } else {
        resultado = inserirAluno(&aluno);

        if (resultado == 1) {
            puts("Aluno inserido com sucesso.");
        } else if (resultado == 0) {
            puts("Nao foi possivel inserir o aluno: a matricula ja existe.");
        } else {
            puts("Nao foi possivel inserir o aluno.");
        }
    }

    free(aluno.matricula);
    free(aluno.nome);
    free(aluno.turma);
}

/* Solicita uma matricula e informa o resultado da remocao do aluno. */
static void excluirAluno(void) {
    char matricula[TAMANHO_ENTRADA];
    int resultado;

    if (!lerEntrada("Matricula: ", matricula, TAMANHO_ENTRADA)) {
        return;
    }

    resultado = removerAluno(matricula);
    if (resultado == 1) {
        puts("Aluno removido com sucesso.");
    } else if (resultado == 0) {
        puts("Aluno nao encontrado.");
    } else {
        puts("Nao foi possivel remover o aluno.");
    }
}

/* Exibe as operacoes de aluno e direciona a opcao escolhida para o modulo adequado. */
static void menuAlunos(void) {
    char opcao[TAMANHO_ENTRADA];

    while (1) {
        puts("\n--- ALUNOS ---");
        puts("1 - Inserir");
        puts("2 - Buscar por nome");
        puts("3 - Buscar por matricula");
        puts("4 - Remover");
        puts("0 - Voltar");

        if (!lerEntrada("Opcao: ", opcao, TAMANHO_ENTRADA)) {
            return;
        }

        if (strcmp(opcao, "1") == 0) {
            inserirNovoAluno();
        } else if (strcmp(opcao, "2") == 0) {
            buscarAlunoNome();
        } else if (strcmp(opcao, "3") == 0) {
            buscarAlunoMatricula();
        } else if (strcmp(opcao, "4") == 0) {
            excluirAluno();
        } else if (strcmp(opcao, "0") == 0) {
            return;
        } else {
            puts("Opcao invalida.");
        }
    }
}

/* Coleta os campos do livro, valida a quantidade e solicita sua insercao no CSV. */
static void inserirNovoLivro(void) {
    char codigo[TAMANHO_ENTRADA];
    char titulo[TAMANHO_ENTRADA];
    char autor[TAMANHO_ENTRADA];
    char quantidadeTexto[TAMANHO_ENTRADA];
    Livro livro;
    int resultado;

    if (!lerEntrada("Codigo: ", codigo, TAMANHO_ENTRADA)
        || !lerEntrada("Titulo: ", titulo, TAMANHO_ENTRADA)
        || !lerEntrada("Autor: ", autor, TAMANHO_ENTRADA)
        || !lerEntrada(
            "Quantidade: ",
            quantidadeTexto,
            TAMANHO_ENTRADA
        )) {
        puts("Cadastro cancelado.");
        return;
    }

    if (!converterQuantidade(quantidadeTexto, &livro.quantidade)) {
        puts("Quantidade invalida.");
        return;
    }

    livro.codigo = codigo;
    livro.titulo = titulo;
    livro.autor = autor;
    resultado = inserirLivro(&livro);

    if (resultado == 1) {
        puts("Livro inserido com sucesso.");
    } else if (resultado == 0) {
        puts("Nao foi possivel inserir o livro: o codigo ja existe.");
    } else {
        puts("Nao foi possivel inserir o livro.");
    }
}

/* Solicita um titulo e apresenta todos os livros correspondentes. */
static void buscarLivroTitulo(void) {
    char titulo[TAMANHO_ENTRADA];
    int resultado;

    if (!lerEntrada("Titulo: ", titulo, TAMANHO_ENTRADA)) {
        return;
    }

    resultado = buscarLivroPorTitulo(titulo);
    if (resultado == 0) {
        puts("Nenhum livro encontrado.");
    } else if (resultado < 0) {
        puts("Nao foi possivel buscar os livros.");
    }
}

// Solicita um autor e apresenta todos os livros correspondentes.
static void buscarLivroAutor(void) {
    char autor[TAMANHO_ENTRADA];
    int resultado;

    if (!lerEntrada("Autor: ", autor, TAMANHO_ENTRADA)) {
        return;
    }

    resultado = buscarLivroPorAutor(autor);
    if (resultado == 0) {
        puts("Nenhum livro encontrado.");
    } else if (resultado < 0) {
        puts("Nao foi possivel buscar os livros.");
    }
}

// Solicita um codigo e informa o resultado da remocao do livro.
static void excluirLivro(void) {
    char codigo[TAMANHO_ENTRADA];
    int resultado;

    if (!lerEntrada("Codigo: ", codigo, TAMANHO_ENTRADA)) {
        return;
    }

    resultado = removerLivro(codigo);
    if (resultado == 1) {
        puts("Livro removido com sucesso.");
    } else if (resultado == 0) {
        puts("Livro nao encontrado.");
    } else {
        puts("Nao foi possivel remover o livro.");
    }
}

/* Exibe as operacoes de livro e direciona a opcao escolhida para a funcao adequada. */
static void menuLivros(void) {
    char opcao[TAMANHO_ENTRADA];

    while (1) {
        puts("\n--- LIVROS ---");
        puts("1 - Inserir");
        puts("2 - Buscar por titulo");
        puts("3 - Buscar por autor");
        puts("4 - Remover");
        puts("0 - Voltar");

        if (!lerEntrada("Opcao: ", opcao, TAMANHO_ENTRADA)) {
            return;
        }

        if (strcmp(opcao, "1") == 0) {
            inserirNovoLivro();
        } else if (strcmp(opcao, "2") == 0) {
            buscarLivroTitulo();
        } else if (strcmp(opcao, "3") == 0) {
            buscarLivroAutor();
        } else if (strcmp(opcao, "4") == 0) {
            excluirLivro();
        } else if (strcmp(opcao, "0") == 0) {
            return;
        } else {
            puts("Opcao invalida.");
        }
    }
}

/* Coleta os identificadores e a data para registrar um novo emprestimo. */
static void registrarNovoEmprestimo(void) {
    char matricula[TAMANHO_ENTRADA];
    char codigo[TAMANHO_ENTRADA];
    char data[TAMANHO_ENTRADA];
    int resultado;

    if (!lerEntrada("Matricula do aluno: ", matricula, TAMANHO_ENTRADA)
        || !lerEntrada("Codigo do livro: ", codigo, TAMANHO_ENTRADA)
        || !lerEntrada("Data do emprestimo: ", data, TAMANHO_ENTRADA)) {
        puts("Registro cancelado.");
        return;
    }

    resultado = registrarEmprestimo(matricula, codigo, data);
    if (resultado == 1) {
        puts("Emprestimo registrado com sucesso.");
    } else if (resultado == 0) {
        puts("Aluno nao encontrado.");
    } else if (resultado == -2) {
        puts("Livro nao encontrado.");
    } else {
        puts("Nao foi possivel registrar o emprestimo.");
    }
}

/* Mantem o menu principal ativo e executa as operacoes escolhidas pelo usuario. */
int main(void) {
    char opcao[TAMANHO_ENTRADA];

    while (1) {
        int resultado;

        puts("\n===== BIBLIOTECA =====");
        puts("1 - Alunos");
        puts("2 - Livros");
        puts("3 - Registrar emprestimo");
        puts("4 - Listar emprestimos");
        puts("0 - Sair");

        if (!lerEntrada("Opcao: ", opcao, TAMANHO_ENTRADA)) {
            return 0;
        }

        if (strcmp(opcao, "1") == 0) {
            menuAlunos();
        } else if (strcmp(opcao, "2") == 0) {
            menuLivros();
        } else if (strcmp(opcao, "3") == 0) {
            registrarNovoEmprestimo();
        } else if (strcmp(opcao, "4") == 0) {
            resultado = listarEmprestimos();
            if (resultado == 0) {
                puts("Nenhum emprestimo registrado.");
            } else if (resultado < 0) {
                puts("Nao foi possivel listar os emprestimos.");
            }
        } else if (strcmp(opcao, "0") == 0) {
            return 0;
        } else {
            puts("Opcao invalida.");
        }
    }
}
