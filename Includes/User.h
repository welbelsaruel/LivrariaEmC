#ifndef USER_H
#define USER_H

typedef struct {
    char nome[100];
    int matricula;  
    char curso[50];
} User;

void BuscarUsuario(const char *nome, User *usuario);

void Inserir_Pessoa();

void Remover_Pessoa();

#endif