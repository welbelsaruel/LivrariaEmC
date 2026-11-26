#include <stdio.h>
#include <string.h>

#include "Livro.h"

#define ARQUIVO_LIVROS "Files/livros.csv"
#define ARQUIVO_LIVROS_TEMP "Files/livros.tmp"
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

/* Converte a quantidade textual em int e rejeita caracteres ou valores invalidos. */
static int converterQuantidade(const char *texto, int *quantidade) {
    unsigned int valor = 0;
    unsigned int maximo = (~0u) >> 1;
    int indice = 0;

    if (texto == NULL || texto[0] == '\0') {
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

/* Divide uma linha CSV e preenche os ponteiros e a quantidade da estrutura Livro. */
static int separarLivro(char linha[], Livro *livro) {
    char *primeiroSeparador;
    char *segundoSeparador;
    char *terceiroSeparador;

    primeiroSeparador = strchr(linha, ';');
    if (primeiroSeparador == NULL) {
        return 0;
    }
    segundoSeparador = strchr(primeiroSeparador + 1, ';');
    if (segundoSeparador == NULL) {
        return 0;
    }
    terceiroSeparador = strchr(segundoSeparador + 1, ';');
    if (terceiroSeparador == NULL) {
        return 0;
    }
    if (strchr(terceiroSeparador + 1, ';') != NULL) {
        return 0;
    }

    *primeiroSeparador = '\0';
    *segundoSeparador = '\0';
    *terceiroSeparador = '\0';

    livro->codigo = linha;
    livro->titulo = primeiroSeparador + 1;
    livro->autor = segundoSeparador + 1;

    if (!campoValido(livro->codigo)
        || !campoValido(livro->titulo)
        || !campoValido(livro->autor)
        || !converterQuantidade(
            terceiroSeparador + 1,
            &livro->quantidade
        )) {
        return 0;
    }

    return 1;
}

/* Grava todos os campos de um livro em uma linha do CSV. */
static int escreverLivro(FILE *arquivo, const Livro *livro) {
    if (fprintf(
            arquivo,
            "%s;%s;%s;%d\n",
            livro->codigo,
            livro->titulo,
            livro->autor,
            livro->quantidade
        ) < 0) {
        return 0;
    }

    return 1;
}

/* Mostra todos os atributos do livro no formato definido para o terminal. */
static void exibirLivro(const Livro *livro) {
    printf(
        "Livro: %s | %s | %s | quantidade: %d\n",
        livro->titulo,
        livro->autor,
        livro->codigo,
        livro->quantidade
    );
}

/* Verifica codigos existentes e acrescenta o livro ao CSV quando o codigo e unico. */
int inserirLivro(const Livro *livro) {
    FILE *arquivo;
    char linha[TAMANHO_LINHA];
    int sucesso = 1;

    if (livro == NULL
        || !campoValido(livro->codigo)
        || !campoValido(livro->titulo)
        || !campoValido(livro->autor)
        || livro->quantidade < 0) {
        return -1;
    }

    arquivo = fopen(ARQUIVO_LIVROS, "r");
    while (arquivo != NULL && sucesso) {
        Livro atual;
        int estado = lerLinha(arquivo, linha, TAMANHO_LINHA);

        if (estado == 0) {
            break;
        }
        if (estado < 0 || !separarLivro(linha, &atual)) {
            sucesso = 0;
            break;
        }
        if (strcmp(atual.codigo, livro->codigo) == 0) {
            fclose(arquivo);
            return 0;
        }
    }

    if (arquivo != NULL && fclose(arquivo) != 0) {
        sucesso = 0;
    }
    if (!sucesso) {
        return -1;
    }

    arquivo = fopen(ARQUIVO_LIVROS, "a");
    if (arquivo == NULL) {
        return -1;
    }

    sucesso = escreverLivro(arquivo, livro);
    if (fclose(arquivo) != 0) {
        sucesso = 0;
    }

    if (!sucesso) {
        return -1;
    }
    return 1;
}

/* Percorre o CSV e exibe os livros que correspondem ao titulo ou ao autor. */
static int buscarLivro(const char *valor, int buscarPorTitulo) {
    FILE *arquivo;
    char linha[TAMANHO_LINHA];
    int encontrados = 0;
    int sucesso = 1;

    if (!campoValido(valor)) {
        return -1;
    }

    arquivo = fopen(ARQUIVO_LIVROS, "r");
    if (arquivo == NULL) {
        return 0;
    }

    while (sucesso) {
        Livro livro;
        int estado = lerLinha(arquivo, linha, TAMANHO_LINHA);

        if (estado == 0) {
            break;
        }
        if (estado < 0 || !separarLivro(linha, &livro)) {
            sucesso = 0;
            break;
        }

        if (buscarPorTitulo) {
            if (strcmp(livro.titulo, valor) == 0) {
                exibirLivro(&livro);
                encontrados++;
            }
        } else if (strcmp(livro.autor, valor) == 0) {
            exibirLivro(&livro);
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

int buscarLivroPorTitulo(const char *titulo) {
    return buscarLivro(titulo, 1);
}

int buscarLivroPorAutor(const char *autor) {
    return buscarLivro(autor, 0);
}

/* Copia os dados do livro identificado pelo codigo para buffers externos. */
int obterLivroPorCodigo(
    const char *codigo,
    char titulo[],
    int capacidadeTitulo,
    char autor[],
    int capacidadeAutor,
    int *quantidade
) {
    FILE *arquivo;
    char linha[TAMANHO_LINHA];
    int resultado = 0;

    if (!campoValido(codigo)
        || titulo == NULL
        || autor == NULL
        || quantidade == NULL
        || capacidadeTitulo <= 0
        || capacidadeAutor <= 0) {
        return -1;
    }

    arquivo = fopen(ARQUIVO_LIVROS, "r");
    if (arquivo == NULL) {
        return 0;
    }

    while (resultado == 0) {
        Livro livro;
        int estado = lerLinha(arquivo, linha, TAMANHO_LINHA);

        if (estado == 0) {
            break;
        }
        if (estado < 0 || !separarLivro(linha, &livro)) {
            resultado = -1;
            break;
        }

        if (strcmp(livro.codigo, codigo) == 0) {
            if ((int)strlen(livro.titulo) >= capacidadeTitulo
                || (int)strlen(livro.autor) >= capacidadeAutor) {
                resultado = -1;
                break;
            }

            strcpy(titulo, livro.titulo);
            strcpy(autor, livro.autor);
            *quantidade = livro.quantidade;
            resultado = 1;
        }
    }

    if (fclose(arquivo) != 0) {
        return -1;
    }
    return resultado;
}

/* Remove pelo codigo, copiando os demais livros para um arquivo temporario. */
int removerLivro(const char *codigo) {
    FILE *original;
    FILE *temporario;
    char linha[TAMANHO_LINHA];
    int removido = 0;
    int sucesso = 1;

    if (!campoValido(codigo)) {
        return -1;
    }

    original = fopen(ARQUIVO_LIVROS, "r");
    if (original == NULL) {
        return 0;
    }

    temporario = fopen(ARQUIVO_LIVROS_TEMP, "w");
    if (temporario == NULL) {
        fclose(original);
        return -1;
    }

    while (sucesso) {
        Livro livro;
        int estado = lerLinha(original, linha, TAMANHO_LINHA);

        if (estado == 0) {
            break;
        }
        if (estado < 0 || !separarLivro(linha, &livro)) {
            sucesso = 0;
            break;
        }

        if (strcmp(livro.codigo, codigo) == 0) {
            removido = 1;
        } else if (!escreverLivro(temporario, &livro)) {
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
        remove(ARQUIVO_LIVROS_TEMP);
        if (!sucesso) {
            return -1;
        }
        return 0;
    }
    if (rename(ARQUIVO_LIVROS_TEMP, ARQUIVO_LIVROS) != 0) {
        remove(ARQUIVO_LIVROS_TEMP);
        return -1;
    }

    return 1;
}
