#ifndef ALUNO_H
#define ALUNO_H

typedef struct {
    char *matricula;
    char *nome;
    char *turma;
} Aluno;

int inserirAluno(const Aluno *aluno);
int buscarAlunoPorNome(const char *nome);
int buscarAlunoPorMatricula(const char *matricula);
void buscarAlunoNome(void);
void buscarAlunoMatricula(void);
int obterAlunoPorMatricula(
    const char *matricula,
    char nome[],
    int capacidadeNome,
    char turma[],
    int capacidadeTurma
);
int removerAluno(const char *matricula);

#endif
