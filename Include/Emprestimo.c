#include <stdio.h>
#include <string.h>

#include "Aluno.h"
#include "Emprestimo.h"
#include "Livro.h"

#define ARQUIVO_EMPRESTIMO_TEMP "Files/emprestimo.tmp"
#define ARQUIVO_EMPRESTIMOS "Files/emprestimos.txt"
#define TAMANHO_CAMPO 1024
#define TAMANHO_DATA 64
#define TAMANHO_LINHA 2200

typedef struct {
    char matricula[TAMANHO_CAMPO];
    char aluno[TAMANHO_CAMPO];
    char turma[TAMANHO_CAMPO];
    char codigo[TAMANHO_CAMPO];
    char livro[TAMANHO_CAMPO];
    char autor[TAMANHO_CAMPO];
    int quantidade;
    char data[TAMANHO_DATA];
} RegistroEmprestimoBinario;

/* Verifica se o campo pode ser usado no registro textual do emprestimo. */
static int campoValido(const char *campo, int capacidade) {
    if (campo == NULL || campo[0] == '\0') {
        return 0;
    }
    if ((int)strlen(campo) >= capacidade) {
        return 0;
    }
    if (strpbrk(campo, "\r\n|") != NULL) {
        return 0;
    }
    return 1;
}

/* Grava o registro completo em binario para transferi-lo ao arquivo permanente. */
static int gravarTemporario(const RegistroEmprestimoBinario *registro) {
    FILE *arquivo = fopen(ARQUIVO_EMPRESTIMO_TEMP, "wb");
    int sucesso;

    if (arquivo == NULL) {
        return 0;
    }

    sucesso = fwrite(
        registro,
        sizeof(RegistroEmprestimoBinario),
        1,
        arquivo
    ) == 1;

    if (fclose(arquivo) != 0) {
        sucesso = 0;
    }
    return sucesso;
}

/* Le o binario temporario e acrescenta o formato legivel ao TXT permanente. */
static int transferirParaTexto(void) {
    FILE *temporario;
    FILE *permanente;
    RegistroEmprestimoBinario registro;
    int sucesso = 1;

    temporario = fopen(ARQUIVO_EMPRESTIMO_TEMP, "rb");
    if (temporario == NULL) {
        return 0;
    }

    if (fread(
            &registro,
            sizeof(RegistroEmprestimoBinario),
            1,
            temporario
        ) != 1) {
        sucesso = 0;
    }
    if (fclose(temporario) != 0) {
        sucesso = 0;
    }

    if (!sucesso) {
        return 0;
    }

    permanente = fopen(ARQUIVO_EMPRESTIMOS, "a");
    if (permanente == NULL) {
        return 0;
    }

    if (fprintf(
            permanente,
            "Aluno %s | Livro %s | %s\n",
            registro.aluno,
            registro.livro,
            registro.data
        ) < 0) {
        sucesso = 0;
    }
    if (fclose(permanente) != 0) {
        sucesso = 0;
    }

    return sucesso;
}

/* Localiza aluno e livro, cria o binario temporario e registra o emprestimo no TXT. */
int registrarEmprestimo(
    const char *matricula,
    const char *codigo,
    const char *data
) {
    RegistroEmprestimoBinario registro = {0};
    int resultado;

    if (!campoValido(matricula, TAMANHO_CAMPO)
        || !campoValido(codigo, TAMANHO_CAMPO)
        || !campoValido(data, TAMANHO_DATA)) {
        return -1;
    }

    strcpy(registro.matricula, matricula);
    strcpy(registro.codigo, codigo);
    strcpy(registro.data, data);

    resultado = obterAlunoPorMatricula(
        matricula,
        registro.aluno,
        TAMANHO_CAMPO,
        registro.turma,
        TAMANHO_CAMPO
    );
    if (resultado == 0) {
        return 0;
    }
    if (resultado < 0) {
        return -1;
    }

    resultado = obterLivroPorCodigo(
        codigo,
        registro.livro,
        TAMANHO_CAMPO,
        registro.autor,
        TAMANHO_CAMPO,
        &registro.quantidade
    );
    if (resultado == 0) {
        return -2;
    }
    if (resultado < 0) {
        return -1;
    }

    if (!gravarTemporario(&registro)) {
        remove(ARQUIVO_EMPRESTIMO_TEMP);
        return -1;
    }
    if (!transferirParaTexto()) {
        remove(ARQUIVO_EMPRESTIMO_TEMP);
        return -1;
    }

    remove(ARQUIVO_EMPRESTIMO_TEMP);
    return 1;
}

/* Exibe cada linha do arquivo permanente sem carregar todos os registros. */
int listarEmprestimos(void) {
    FILE *arquivo = fopen(ARQUIVO_EMPRESTIMOS, "r");
    char linha[TAMANHO_LINHA];
    int quantidade = 0;
    int erro;

    if (arquivo == NULL) {
        return 0;
    }

    while (fgets(linha, TAMANHO_LINHA, arquivo) != NULL) {
        fputs(linha, stdout);
        quantidade++;
    }

    erro = ferror(arquivo);
    if (fclose(arquivo) != 0) {
        erro = 1;
    }
    if (erro) {
        return -1;
    }
    return quantidade;
}
