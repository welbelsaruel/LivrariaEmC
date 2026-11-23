#include <stdio.h>
#include <string.h>

#include "User.h"

#define USER_BIN "Files/user.bin"

int criarUser(User user) {
    FILE *arquivo;
    User existente;

    if (memchr(user.matricula, '\0', sizeof user.matricula) == NULL ||
        user.matricula[0] == '\0') {
        return 0;
    }

    arquivo = fopen(USER_BIN, "rb");
    if (arquivo != NULL) {
        while (fread(&existente, sizeof existente, 1, arquivo) == 1) {
            existente.matricula[sizeof existente.matricula - 1] = '\0';
            if (strcmp(user.matricula, existente.matricula) == 0) {
                fclose(arquivo);
                return 0;
            }
        }
        fclose(arquivo);
    }

    arquivo = fopen(USER_BIN, "ab");
    if (arquivo == NULL) {
        return 0;
    }

    int gravou = fwrite(&user, sizeof user, 1, arquivo) == 1;
    return fclose(arquivo) == 0 && gravou;
}

int buscarUMatricula(const char *matricula, User *pessoa) {
    if (matricula == NULL || pessoa == NULL) {
        return 0;
    }

    FILE *arquivo = fopen(USER_BIN, "rb");
    if (arquivo == NULL) {
        return 0;
    }

    User registro;
    while (fread(&registro, sizeof registro, 1, arquivo) == 1) {
        if (memchr(registro.matricula, '\0', sizeof registro.matricula) != NULL &&
            strcmp(matricula, registro.matricula) == 0) {
            *pessoa = registro;
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

int buscarUNome(const char *nome, User *pessoa) {
    if (nome == NULL || pessoa == NULL) {
        return 0;
    }

    FILE *arquivo = fopen(USER_BIN, "rb");
    if (arquivo == NULL) {
        return 0;
    }

    User registro;
    while (fread(&registro, sizeof registro, 1, arquivo) == 1) {
        if (memchr(registro.nome, '\0', sizeof registro.nome) != NULL &&
            strcmp(nome, registro.nome) == 0) {
            *pessoa = registro;
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

int buscarUCurso(const char *curso, User *pessoa) {
    if (curso == NULL || pessoa == NULL) {
        return 0;
    }

    FILE *arquivo = fopen(USER_BIN, "rb");
    if (arquivo == NULL) {
        return 0;
    }

    User registro;
    while (fread(&registro, sizeof registro, 1, arquivo) == 1) {
        if (memchr(registro.curso, '\0', sizeof registro.curso) != NULL &&
            strcmp(curso, registro.curso) == 0) {
            *pessoa = registro;
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

int buscarUser(const char *valor, User *pessoa, FuncaoBuscaUser funcaoBusca) {
    if (valor == NULL || pessoa == NULL || funcaoBusca == NULL) {
        return 0;
    }

    return funcaoBusca(valor, pessoa);
}

int removerUser(const char *matricula) {
    if (matricula == NULL) {
        return 0;
    }

    FILE *arquivo = fopen(USER_BIN, "rb");
    if (arquivo == NULL) {
        return 0;
    }

    const char *caminhoTemporario = USER_BIN ".tmp";
    FILE *temporario = fopen(caminhoTemporario, "wbx");
    if (temporario == NULL) {
        fclose(arquivo);
        return 0;
    }

    int removeu = 0;
    User registro;
    size_t bytesLidos;
    while ((bytesLidos = fread(&registro, 1, sizeof registro, arquivo)) ==
           sizeof registro) {
        if (!removeu &&
            memchr(registro.matricula, '\0', sizeof registro.matricula) != NULL &&
            strcmp(matricula, registro.matricula) == 0) {
            removeu = 1;
        } else if (fwrite(&registro, sizeof registro, 1, temporario) != 1) {
            break;
        }
    }

    int copiaCompleta = bytesLidos == 0 && !ferror(arquivo);
    if (fclose(arquivo) != 0) {
        copiaCompleta = 0;
    }
    if (fclose(temporario) != 0) {
        copiaCompleta = 0;
    }

    if (removeu && copiaCompleta && rename(caminhoTemporario, USER_BIN) == 0) {
        return 1;
    }

    remove(caminhoTemporario);
    return 0;
}

void liberarUser(User *user) {
    if (user != NULL) {
        *user = (User){0};
    }
}
