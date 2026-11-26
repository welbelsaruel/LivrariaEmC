#ifndef EMPRESTIMO_H
#define EMPRESTIMO_H

/* Retorna 1 em sucesso, 0 sem aluno, -2 sem livro e -1 em erro. */
int registrarEmprestimo(
    const char *matricula,
    const char *codigo,
    const char *data
);

/* Exibe e retorna a quantidade de emprestimos, ou -1 em erro. */
int listarEmprestimos(void);

#endif
