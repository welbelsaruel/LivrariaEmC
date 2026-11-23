#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes/User.h"

static void escreverCampoCsv(FILE *csv, const char *campo, size_t capacidade) {
    fputc('"', csv);
    for (size_t i = 0; i < capacidade && campo[i] != '\0'; i++) {
        if (campo[i] == '"') {
            fputc('"', csv);
        }
        fputc(campo[i], csv);
    }
    fputc('"', csv);
}

static void exportarUsuariosCsv(void) {
    FILE *binario = fopen("Files/user.bin", "rb");
    if (binario == NULL && errno != ENOENT) {
        perror("Files/user.bin");
        return;
    }

    FILE *csv = fopen("Files/user.csv", "w");
    if (csv == NULL) {
        perror("Files/user.csv");
        if (binario != NULL) {
            fclose(binario);
        }
        return;
    }

    fputs("matricula,nome,curso\n", csv);
    int leituraOk = 1;
    if (binario != NULL) {
        User usuario;
        size_t bytesLidos;
        while ((bytesLidos = fread(&usuario, 1, sizeof usuario, binario)) ==
               sizeof usuario) {
            escreverCampoCsv(csv, usuario.matricula, sizeof usuario.matricula);
            fputc(',', csv);
            escreverCampoCsv(csv, usuario.nome, sizeof usuario.nome);
            fputc(',', csv);
            escreverCampoCsv(csv, usuario.curso, sizeof usuario.curso);
            fputc('\n', csv);
        }
        leituraOk = bytesLidos == 0 && !ferror(binario);
        fclose(binario);
    }

    int escritaOk = !ferror(csv);
    if (fclose(csv) == EOF) {
        escritaOk = 0;
    }
    if (!leituraOk) {
        fputs("Erro ao ler os registros de Files/user.bin.\n", stderr);
    }
    if (!escritaOk) {
        fputs("Erro ao gravar Files/user.csv.\n", stderr);
    }
}

static int lerCampo(const char *mensagem, char *campo, size_t capacidade) {
    for (;;) {
        fputs(mensagem, stdout);
        fflush(stdout);

        size_t tamanho = 0;
        int excedeuLimite = 0;
        int caractere;

        while ((caractere = getchar()) != '\n' && caractere != EOF) {
            if (tamanho < capacidade - 1) {
                campo[tamanho++] = (char)caractere;
            } else {
                excedeuLimite = 1;
            }
        }

        if (tamanho == 0) {
            return 0;
        }
        if (excedeuLimite) {
            printf("Limite de %zu bytes. Digite novamente.\n", capacidade - 1);
            continue;
        }
        campo[tamanho] = '\0';
        char *fim;
        double numero = strtod(campo, &fim);
        if (numero < 0 && fim[strspn(fim, " \t\r\f\v")] == '\0') {
            return 0;
        }

        return 1;
    }
}

int main(void) {
    exportarUsuariosCsv();
    puts("Para sair, pressione Enter com o campo vazio ou digite um numero negativo.");
    for (;;) {
        User usuario = {0};

        if (!lerCampo("Matricula: ", usuario.matricula, sizeof usuario.matricula) ||
            !lerCampo("Nome: ", usuario.nome, sizeof usuario.nome) ||
            !lerCampo("Curso: ", usuario.curso, sizeof usuario.curso)) {
            return EXIT_SUCCESS;
        }

        if (criarUser(usuario)) {
            puts("Usuario criado com sucesso!");
            exportarUsuariosCsv();
        } else {
            fputs("Nao foi possivel criar o usuario.\n", stderr);
        }
    }
}
