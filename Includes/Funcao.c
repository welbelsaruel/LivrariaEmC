#include <stdio.h>
#include <string.h>
#include "Funcao.h"

/* Mantem a decisao do projeto de nao incluir stdlib.h.
   As funcoes de alocacao ainda sao fornecidas pela biblioteca padrao. */
extern void *malloc(size_t);
extern void *realloc(void *, size_t);
extern void free(void *);

static const char *texto(const TipoRegistro *t, const void *r, size_t i) {
    const char *p = (const char *)r + t->campos[i].deslocamento;
    return t->campos[i].tipo == TEXTO_DINAMICO ? *(char *const *)p : p;
}

static void liberar(const TipoRegistro *t, void *r) {
    if (!r) return;
    for (size_t i = 0; i < t->quantidadeCampos; i++)
        if (t->campos[i].tipo == TEXTO_DINAMICO)
            free(*(char **)((char *)r + t->campos[i].deslocamento));
    free(r);
}

static int validarTipo(const TipoRegistro *t) {
    if (!t || !t->arquivo || !t->marca || strlen(t->marca) != 4 || !t->campos ||
        !t->tamanho || t->chave >= t->quantidadeCampos || t->ordem >= t->quantidadeCampos)
        return 0;
    for (size_t i = 0; i < t->quantidadeCampos; i++) {
        const Campo *c = &t->campos[i];
        size_t n = c->tipo == TEXTO_DINAMICO ? sizeof(char *) :
                   c->tipo == INTEIRO ? sizeof(int) : c->tamanho;
        if (c->tipo < TEXTO_FIXO || c->tipo > INTEIRO || !n ||
            c->deslocamento > t->tamanho || n > t->tamanho - c->deslocamento) return 0;
    }
    return t->campos[t->chave].tipo != INTEIRO && t->campos[t->ordem].tipo != INTEIRO;
}

static int validar(const TipoRegistro *t, const void *r) {
    if (!r) return 0;
    for (size_t i = 0; i < t->quantidadeCampos; i++) {
        const Campo *c = &t->campos[i];
        if (c->tipo == TEXTO_FIXO && !memchr(texto(t, r, i), 0, c->tamanho)) return 0;
        if (c->tipo == TEXTO_DINAMICO && !texto(t, r, i)) return 0;
    }
    return texto(t, r, t->chave)[0] != 0;
}

/* Le um registro sem depender da struct concreta. */
static int ler(FILE *f, const TipoRegistro *t, void **saida) {
    *saida = NULL;
    int byte = fgetc(f);
    if (byte == EOF) return ferror(f) ? -1 : 0;
    if (ungetc(byte, f) == EOF) return -1;
    void *r = malloc(t->tamanho);
    if (!r) return -1;
    memset(r, 0, t->tamanho);
    for (size_t i = 0; i < t->quantidadeCampos; i++) {
        const Campo *c = &t->campos[i];
        char *p = (char *)r + c->deslocamento;
        size_t n = c->tipo == INTEIRO ? sizeof(int) : c->tamanho;
        if (c->tipo == TEXTO_DINAMICO) {
            if (fread(&n, sizeof n, 1, f) != 1 || n == (size_t)-1) goto erro;
            long inicio = ftell(f);
            if (inicio < 0 || fseek(f, 0, SEEK_END)) goto erro;
            long fim = ftell(f);
            if (fim < inicio || n > (size_t)(fim - inicio) || fseek(f, inicio, SEEK_SET)) goto erro;
            *(char **)p = malloc(n + 1);
            if (!*(char **)p) goto erro;
            p = *(char **)p;
            p[n] = 0;
        }
        if (fread(p, 1, n, f) != n) goto erro;
        if (c->tipo == TEXTO_DINAMICO && strlen(p) != n) goto erro;
    }
    if (!validar(t, r)) goto erro;
    *saida = r;
    return 1;
erro:
    liberar(t, r);
    return -1;
}

static int indice(FILE *f, const TipoRegistro *t, size_t *n, long **pos) {
    char marca[4];
    *n = 0; *pos = NULL;
    rewind(f);
    size_t bytes = fread(marca, 1, 4, f);
    if (bytes != 4 || memcmp(marca, t->marca, 4)) {
        if (ferror(f)) return -1;
        if (bytes == 0 || (t->aceitaLegado && (bytes != 4 || memcmp(marca, "LVR2", 4)))) {
            rewind(f);
            return 0;
        }
        return -1;
    }
    if (fread(n, sizeof *n, 1, f) != 1) return -1;
    long inicio = ftell(f);
    if (inicio < 0 || fseek(f, 0, SEEK_END)) return -1;
    long fim = ftell(f);
    if (fim < inicio || *n > (size_t)(fim - inicio) / sizeof(long) || fseek(f, inicio, SEEK_SET)) return -1;
    if (*n) {
        *pos = malloc(*n * sizeof(long));
        if (!*pos) return -1;
        if (fread(*pos, sizeof(long), *n, f) != *n) goto erro;
        long dados = ftell(f);
        for (size_t i = 0; i < *n; i++)
            if ((*pos)[i] < dados || (*pos)[i] >= fim) goto erro;
    }
    return 1;
erro:
    free(*pos); *pos = NULL;
    return -1;
}

static int carregar(FILE *f, const TipoRegistro *t, void ***regs, size_t *n) {
    size_t total; long *pos;
    int formato = indice(f, t, &total, &pos), estado = 0;
    *regs = NULL; *n = 0;
    if (formato < 0) return 0;
    while (!formato || *n < total) {
        void *r = NULL;
        if (formato && fseek(f, pos[*n], SEEK_SET)) { estado = -1; break; }
        estado = ler(f, t, &r);
        if (estado != 1) break;
        if (*n >= (size_t)-1 / sizeof(void *) - 1) { liberar(t, r); estado = -1; break; }
        void **novo = realloc(*regs, (*n + 1) * sizeof(void *));
        if (!novo) { liberar(t, r); estado = -1; break; }
        *regs = novo; (*regs)[(*n)++] = r;
    }
    free(pos);
    return estado >= 0 && (!formato || *n == total);
}

static void ordenar(const TipoRegistro *t, void **r, size_t n) {
    for (size_t i = 1; i < n; i++) {
        void *atual = r[i]; size_t j = i;
        while (j && strcmp(texto(t, r[j-1], t->ordem), texto(t, atual, t->ordem)) > 0) {
            r[j] = r[j-1]; j--;
        }
        r[j] = atual;
    }
}

static int gravar(const TipoRegistro *t, void **r, size_t n) {
    size_t tamanho = strlen(t->arquivo);
    if (tamanho > (size_t)-1 - 5) return 0;
    char *tmp = malloc(tamanho + 5);
    if (!tmp) return 0;
    memcpy(tmp, t->arquivo, tamanho); memcpy(tmp + tamanho, ".tmp", 5);
    FILE *f = fopen(tmp, "wbx");
    if (!f) { free(tmp); return 0; }
    int ok = fwrite(t->marca, 1, 4, f) == 4 && fwrite(&n, sizeof n, 1, f) == 1;
    long tabela = ftell(f), zero = 0;
    for (size_t i = 0; ok && i < n; i++) ok = fwrite(&zero, sizeof zero, 1, f) == 1;
    for (size_t i = 0; ok && i < n; i++) {
        long pos = ftell(f);
        ok = pos >= 0;
        for (size_t j = 0; ok && j < t->quantidadeCampos; j++) {
            const Campo *c = &t->campos[j];
            const char *p = (const char *)r[i] + c->deslocamento;
            size_t bytes = c->tipo == INTEIRO ? sizeof(int) : c->tamanho;
            if (c->tipo == TEXTO_DINAMICO) {
                p = *(char *const *)p; bytes = strlen(p);
                ok = fwrite(&bytes, sizeof bytes, 1, f) == 1;
            }
            ok = ok && fwrite(p, 1, bytes, f) == bytes;
        }
        long proximo = ftell(f);
        ok = ok && proximo >= 0 && tabela >= 0 && !fseek(f, tabela, SEEK_SET) && fwrite(&pos, sizeof pos, 1, f) == 1;
        tabela = ftell(f);
        ok = ok && !fseek(f, proximo, SEEK_SET);
    }
    if (fclose(f)) ok = 0;
    if (ok) ok = rename(tmp, t->arquivo) == 0;
    if (!ok) remove(tmp);
    free(tmp);
    return ok;
}

int CRIAR(const TipoRegistro *t, const void *registro) {
    if (!validarTipo(t) || !validar(t, registro)) return -1;
    FILE *f = fopen(t->arquivo, "ab+");
    if (!f) return -1;
    void **r; size_t n;
    int ok = carregar(f, t, &r, &n), resultado = -1;
    fclose(f);
    if (ok) {
        /* A chave e unica dentro da base, independentemente do nome.
           Em duplicidade, nao regrava o arquivo nem altera o registro existente. */
        const char *chave = texto(t, registro, t->chave);
        for (size_t i = 0; i < n; i++) {
            if (strcmp(texto(t, r[i], t->chave), chave) == 0) {
                resultado = 0;
                ok = 0;
                break;
            }
        }
    }
    if (ok && n < (size_t)-1 / sizeof(void *) - 1) {
        void **novo = realloc(r, (n + 1) * sizeof(void *));
        if (novo) {
            r = novo; ordenar(t, r, n);
            size_t inicio = 0, fim = n;
            while (inicio < fim) {
                size_t meio = inicio + (fim-inicio)/2;
                if (strcmp(texto(t,r[meio],t->ordem), texto(t,registro,t->ordem)) <= 0) inicio = meio+1;
                else fim = meio;
            }
            memmove(r+inicio+1, r+inicio, (n-inicio)*sizeof(void *));
            r[inicio] = (void *)registro;
            resultado = gravar(t, r, n+1) ? 1 : -1;
            memmove(r+inicio, r+inicio+1, (n-inicio)*sizeof(void *));
        }
    }
    for (size_t i = 0; i < n; i++) liberar(t, r[i]);
    free(r); return resultado;
}

int BUSCAR(const TipoRegistro *t, const char *nome, FiltroRegistro filtro, void *contexto) {
    if (!validarTipo(t)) return -1;
    FILE *f = fopen(t->arquivo, "rb");
    if (!f) return -1;
    size_t n; long *pos;
    int formato = indice(f,t,&n,&pos), resultado = 0;
    if (formato < 0) { fclose(f); return -1; }
    size_t inicio = 0, fim = n;
    /* Localiza a primeira ocorrencia pelo indice ordenado. */
    while (formato && nome && inicio < fim) {
        size_t meio = inicio + (fim-inicio)/2;
        void *r = NULL;
        if (fseek(f,pos[meio],SEEK_SET) || ler(f,t,&r) != 1) { resultado = -1; goto fim; }
        int cmp = strcmp(texto(t,r,t->ordem), nome);
        liberar(t,r);
        if (cmp < 0) inicio = meio+1; else fim = meio;
    }
    for (size_t i = inicio; !formato || i < n; i++) {
        void *r = NULL;
        if (formato && fseek(f,pos[i],SEEK_SET)) { resultado = -1; break; }
        int estado = ler(f,t,&r);
        if (estado != 1) { if (estado < 0 || formato) resultado = -1; break; }
        int cmp = nome ? strcmp(texto(t,r,t->ordem), nome) : 0;
        if (!cmp && (!filtro || filtro(r,contexto))) resultado = 1;
        liberar(t,r);
        if (formato && nome && cmp > 0) break;
    }
fim:
    free(pos); fclose(f); return resultado;
}

int REMOVER(const TipoRegistro *t, const char *chave) {
    if (!validarTipo(t) || !chave) return -1;
    FILE *f = fopen(t->arquivo,"rb");
    if (!f) return -1;
    void **r; size_t n;
    int ok = carregar(f,t,&r,&n), resultado = ok ? 0 : -1;
    fclose(f);
    for (size_t i = 0; ok && i < n; i++) {
        if (!strcmp(texto(t,r[i],t->chave),chave)) {
            liberar(t,r[i]); memmove(r+i,r+i+1,(n-i-1)*sizeof(void *)); n--;
            ordenar(t,r,n); resultado = gravar(t,r,n) ? 1 : -1; break;
        }
    }
    for (size_t i = 0; i < n; i++) liberar(t,r[i]);
    free(r); return resultado;
}
