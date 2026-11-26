#include <stdio.h>
#include <string.h>

#include "Aluno.h"

#define ARQUIVO_ALUNOS "Files/alunos.csv"
#define ARQUIVO_ALUNOS_TEMP "Files/alunos.tmp"
#define TAMANHO_LINHA 1024

/* Verifica se o campo existe e se pode ser armazenado no formato CSV adotado. */
static int campoValido(const char *campo) {
    if (campo == NULL || campo[0] == '\0') {
        return 0;
    }

    if (strpbrk(campo, ";\r\n") != NULL) {
        return 0;
    }

    return 1;
}

/* Le um registro do arquivo e rejeita linhas maiores que o buffer disponivel. */
static int lerLinha(FILE *arquivo, char linha[], int capacidade) {
    int caractere;
    int tamanho;

    if (fgets(linha, capacidade, arquivo) == NULL) {
        if (ferror(arquivo)) {
            return -1;
        }
        return 0;
    }

    tamanho = (int)strlen(linha);
    if (tamanho > 0 && linha[tamanho - 1] == '\n') {
        linha[tamanho - 1] = '\0';
        tamanho--;
    } else if (!feof(arquivo)) {
        while ((caractere = fgetc(arquivo)) != '\n' && caractere != EOF) {
        }
        return -1;
    }

    if (tamanho > 0 && linha[tamanho - 1] == '\r') {
        linha[tamanho - 1] = '\0';
    }

    return 1;
}

/* Divide uma linha CSV e faz a estrutura apontar para matricula, nome e turma. */
static int separarAluno(char linha[], Aluno *aluno) {
    char *primeiroSeparador;
    char *segundoSeparador;

    primeiroSeparador = strchr(linha, ';');
    if (primeiroSeparador == NULL) {
        return 0;
    }

    segundoSeparador = strchr(primeiroSeparador + 1, ';');
    if (segundoSeparador == NULL) {
        return 0;
    }

    if (strchr(segundoSeparador + 1, ';') != NULL) {
        return 0;
    }

    *primeiroSeparador = '\0';
    *segundoSeparador = '\0';

    aluno->matricula = linha;
    aluno->nome = primeiroSeparador + 1;
    aluno->turma = segundoSeparador + 1;

    if (!campoValido(aluno->matricula)
        || !campoValido(aluno->nome)
        || !campoValido(aluno->turma)) {
        return 0;
    }

    return 1;
}

/* Grava um aluno no CSV usando ponto e virgula como separador. */
static int escreverAluno(FILE *arquivo, const Aluno *aluno) {
    if (fprintf(
            arquivo,
            "%s;%s;%s\n",
            aluno->matricula,
            aluno->nome,
            aluno->turma
        ) < 0) {
        return 0;
    }

    return 1;
}

/* Mostra todos os atributos do aluno no formato definido para o terminal. */
static void exibirAluno(const Aluno *aluno) {
    printf(
        "Aluno : %s | %s | %s\n",
        aluno->nome,
        aluno->matricula,
        aluno->turma
    );
}

/* Insere por ordem de nome, rejeita matricula repetida e substitui o CSV com seguranca. */
int inserirAluno(const Aluno *aluno) {
    FILE *original;
    FILE *temporario;
    char linha[TAMANHO_LINHA];
    int inserido = 0;
    int duplicado = 0;
    int sucesso = 1;

    if (aluno == NULL
        || !campoValido(aluno->matricula)
        || !campoValido(aluno->nome)
        || !campoValido(aluno->turma)) {
        return -1;
    }

    if (strlen(aluno->matricula) >= TAMANHO_LINHA
        || strlen(aluno->nome) >= TAMANHO_LINHA
        || strlen(aluno->turma) >= TAMANHO_LINHA
        || strlen(aluno->matricula)
            + strlen(aluno->nome)
            + strlen(aluno->turma)
            + 3 >= TAMANHO_LINHA) {
        return -1;
    }

    original = fopen(ARQUIVO_ALUNOS, "r");
    temporario = fopen(ARQUIVO_ALUNOS_TEMP, "w");
    if (temporario == NULL) {
        if (original != NULL) {
            fclose(original);
        }
        return -1;
    }

    while (original != NULL && sucesso && !duplicado) {
        Aluno atual;
        int estado = lerLinha(original, linha, TAMANHO_LINHA);

        if (estado == 0) {
            break;
        }
        if (estado < 0 || !separarAluno(linha, &atual)) {
            sucesso = 0;
            break;
        }

        if (strcmp(atual.matricula, aluno->matricula) == 0) {
            duplicado = 1;
            break;
        }

        if (!inserido && strcmp(atual.nome, aluno->nome) > 0) {
            if (!escreverAluno(temporario, aluno)) {
                sucesso = 0;
                break;
            }
            inserido = 1;
        }

        if (!escreverAluno(temporario, &atual)) {
            sucesso = 0;
        }
    }

    if (sucesso && !duplicado && !inserido) {
        sucesso = escreverAluno(temporario, aluno);
    }

    if (original != NULL && fclose(original) != 0) {
        sucesso = 0;
    }
    if (fclose(temporario) != 0) {
        sucesso = 0;
    }

    if (duplicado) {
        remove(ARQUIVO_ALUNOS_TEMP);
        return 0;
    }
    if (!sucesso) {
        remove(ARQUIVO_ALUNOS_TEMP);
        return -1;
    }
    if (rename(ARQUIVO_ALUNOS_TEMP, ARQUIVO_ALUNOS) != 0) {
        remove(ARQUIVO_ALUNOS_TEMP);
        return -1;
    }

    return 1;
}

/* Percorre o CSV e exibe todos os registros que correspondem ao nome ou a matricula. */
static int buscarAluno(const char *valor, int buscarPorNome) {
    FILE *arquivo;
    char linha[TAMANHO_LINHA];
    int encontrados = 0;
    int sucesso = 1;

    if (!campoValido(valor)) {
        return -1;
    }

    arquivo = fopen(ARQUIVO_ALUNOS, "r");
    if (arquivo == NULL) {
        return 0;
    }

    while (sucesso) {
        Aluno aluno;
        int estado = lerLinha(arquivo, linha, TAMANHO_LINHA);

        if (estado == 0) {
            break;
        }
        if (estado < 0 || !separarAluno(linha, &aluno)) {
            sucesso = 0;
            break;
        }

        if (buscarPorNome) {
            if (strcmp(aluno.nome, valor) == 0) {
                exibirAluno(&aluno);
                encontrados++;
            }
        } else if (strcmp(aluno.matricula, valor) == 0) {
            exibirAluno(&aluno);
            encontrados++;
        }
    }

    if (fclose(arquivo) != 0) {
        sucesso = 0;
    }

    if (!sucesso) {
        return -1;
    }
    return encontrados;
}

int buscarAlunoPorNome(const char *nome) {
    return buscarAluno(nome, 1);
}

int buscarAlunoPorMatricula(const char *matricula) {
    return buscarAluno(matricula, 0);
}

/* Solicita o valor da busca e apresenta mensagens para ausencia ou erro de leitura. */
static void solicitarBuscaAluno(const char *mensagem, int buscarPorNome) {
    char valor[TAMANHO_LINHA];
    int estado;
    int resultado;

    fputs(mensagem, stdout);
    fflush(stdout);

    estado = lerLinha(stdin, valor, TAMANHO_LINHA);
    if (estado != 1 || !campoValido(valor)) {
        puts("Entrada invalida.");
        return;
    }

    if (buscarPorNome) {
        resultado = buscarAlunoPorNome(valor);
    } else {
        resultado = buscarAlunoPorMatricula(valor);
    }

    if (resultado == 0) {
        puts("Nenhum aluno encontrado.");
    } else if (resultado < 0) {
        puts("Nao foi possivel buscar os alunos.");
    }
}

void buscarAlunoNome(void) {
    solicitarBuscaAluno("Nome: ", 1);
}

void buscarAlunoMatricula(void) {
    solicitarBuscaAluno("Matricula: ", 0);
}

/* Copia nome e turma do aluno identificado pela matricula para buffers externos. */
int obterAlunoPorMatricula(
    const char *matricula,
    char nome[],
    int capacidadeNome,
    char turma[],
    int capacidadeTurma
) {
    FILE *arquivo;
    char linha[TAMANHO_LINHA];
    int resultado = 0;

    if (!campoValido(matricula)
        || nome == NULL
        || turma == NULL
        || capacidadeNome <= 0
        || capacidadeTurma <= 0) {
        return -1;
    }

    arquivo = fopen(ARQUIVO_ALUNOS, "r");
    if (arquivo == NULL) {
        return 0;
    }

    while (resultado == 0) {
        Aluno aluno;
        int estado = lerLinha(arquivo, linha, TAMANHO_LINHA);

        if (estado == 0) {
            break;
        }
        if (estado < 0 || !separarAluno(linha, &aluno)) {
            resultado = -1;
            break;
        }

        if (strcmp(aluno.matricula, matricula) == 0) {
            if ((int)strlen(aluno.nome) >= capacidadeNome
                || (int)strlen(aluno.turma) >= capacidadeTurma) {
                resultado = -1;
                break;
            }

            strcpy(nome, aluno.nome);
            strcpy(turma, aluno.turma);
            resultado = 1;
        }
    }

    if (fclose(arquivo) != 0) {
        return -1;
    }
    return resultado;
}

/* Remove pela matricula, copiando os demais alunos para um arquivo temporario. */
int removerAluno(const char *matricula) {
    FILE *original;
    FILE *temporario;
    char linha[TAMANHO_LINHA];
    int removido = 0;
    int sucesso = 1;

    if (!campoValido(matricula)) {
        return -1;
    }

    original = fopen(ARQUIVO_ALUNOS, "r");
    if (original == NULL) {
        return 0;
    }

    temporario = fopen(ARQUIVO_ALUNOS_TEMP, "w");
    if (temporario == NULL) {
        fclose(original);
        return -1;
    }

    while (sucesso) {
        Aluno aluno;
        int estado = lerLinha(original, linha, TAMANHO_LINHA);

        if (estado == 0) {
            break;
        }
        if (estado < 0 || !separarAluno(linha, &aluno)) {
            sucesso = 0;
            break;
        }

        if (strcmp(aluno.matricula, matricula) == 0) {
            removido = 1;
        } else if (!escreverAluno(temporario, &aluno)) {
            sucesso = 0;
        }
    }

    if (fclose(original) != 0) {
        sucesso = 0;
    }
    if (fclose(temporario) != 0) {
        sucesso = 0;
    }

    if (!sucesso || !removido) {
        remove(ARQUIVO_ALUNOS_TEMP);
        if (!sucesso) {
            return -1;
        }
        return 0;
    }
    if (rename(ARQUIVO_ALUNOS_TEMP, ARQUIVO_ALUNOS) != 0) {
        remove(ARQUIVO_ALUNOS_TEMP);
        return -1;
    }

    return 1;
}
