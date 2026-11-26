#ifndef FUNCAO_H
#define FUNCAO_H

#include <stdio.h>

typedef enum { TEXTO_FIXO, TEXTO_DINAMICO, INTEIRO } TipoCampo;
typedef struct {
    TipoCampo tipo;
    size_t deslocamento;
    size_t tamanho;
} Campo;

typedef struct {
    const char *arquivo;
    const char *marca; /* Quatro bytes identificam o formato da entidade. */
    size_t tamanho;
    const Campo *campos;
    size_t quantidadeCampos;
    size_t chave; /* Campo textual unico. */
    size_t ordem; /* Campo textual usado na ordenacao. */
    int aceitaLegado;
} TipoRegistro;

/* Recebe um registro emprestado, valido apenas durante a chamada.
   Retorne 1 quando corresponder ao filtro; 0 caso contrario.
   Pode imprimir ou copiar os dados, mas nao deve liberar seus ponteiros. */
typedef int (*FiltroRegistro)(const void *registro, void *contexto);

/* Criar/remover: 1 = sucesso, 0 = duplicado/ausente, -1 = erro.
   Criar copia os dados; o chamador continua dono do registro original. */
int CRIAR(const TipoRegistro *tipo, const void *registro);
/* nome != NULL: busca binaria exata pelo campo de ordenacao.
   nome == NULL: percorre todos os registros aplicando filtro.
   Retorna 1 se houve correspondencia, 0 se nao houve, -1 em erro. */
int BUSCAR(const TipoRegistro *tipo, const char *nome,
           FiltroRegistro filtro, void *contexto);
int REMOVER(const TipoRegistro *tipo, const char *chave);

#endif
