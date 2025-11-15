#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==========================================================
   Funções auxiliares
   ========================================================== */

/* Implementação própria de strdup — compatível com GitHub */
char *copiarString(const char *s) {
    char *c = malloc(strlen(s) + 1);
    if (!c) {
        printf("Erro de memoria!\n");
        exit(1);
    }
    strcpy(c, s);
    return c;
}

/* ==========================================================
   Estruturas de dados
   ========================================================== */

typedef struct Sala {
    char *nome;
    struct Sala *esq;
    struct Sala *dir;
} Sala;

typedef struct NodoPista {
    char *pista;
    struct NodoPista *esq;
    struct NodoPista *dir;
} NodoPista;

typedef struct HashNode {
    char *pista;
    char *suspeito;
    struct HashNode *prox;
} HashNode;

#define HASH_TAM 23
HashNode *tabelaHash[HASH_TAM];

NodoPista *arvorePistas = NULL;

/* ==========================================================
   Hash
   ========================================================== */

unsigned int hash(const char *s) {
    unsigned int h = 0;
    while (*s)
        h = h * 31 + *s++;
    return h % HASH_TAM;
}

void inserirHash(const char *pista, const char *suspeito) {
    unsigned int id = hash(pista);
    HashNode *n = malloc(sizeof(HashNode));

    n->pista = copiarString(pista);
    n->suspeito = copiarString(suspeito);
    n->prox = tabelaHash[id];
    tabelaHash[id] = n;
}

char *buscarSuspeito(const char *pista) {
    unsigned int id = hash(pista);
    HashNode *atual = tabelaHash[id];
    while (atual) {
        if (strcmp(atual->pista, pista) == 0)
            return atual->suspeito;
        atual = atual->prox;
    }
    return NULL;
}

/* ==========================================================
   BST de pistas
   ========================================================== */

int existePista(NodoPista *r, const char *p) {
    if (!r) return 0;
    int cmp = strcmp(p, r->pista);
    if (cmp == 0) return 1;
    if (cmp < 0) return existePista(r->esq, p);
    return existePista(r->dir, p);
}

NodoPista *inserirPista(NodoPista *r, const char *p) {
    if (!r) {
        NodoPista *n = malloc(sizeof(NodoPista));
        n->pista = copiarString(p);
        n->esq = n->dir = NULL;
        return n;
    }
    int cmp = strcmp(p, r->pista);
    if (cmp < 0)
        r->esq = inserirPista(r->esq, p);
    else if (cmp > 0)
        r->dir = inserirPista(r->dir, p);
    return r;
}

/* ==========================================================
   Salas
   ========================================================== */

Sala *criarSala(const char *nome) {
    Sala *s = malloc(sizeof(Sala));
    s->nome = copiarString(nome);
    s->esq = s->dir = NULL;
    return s;
}

const char *pistaDaSala(const char *nome) {
    if (strcmp(nome, "Sala Principal") == 0) return "impressao no vaso";
    if (strcmp(nome, "Biblioteca") == 0) return "pagina rasgada";
    if (strcmp(nome, "Escritorio") == 0) return "bilhete suspeito";
    if (strcmp(nome, "Estufa") == 0) return "luva rasgada";
    if (strcmp(nome, "Cozinha") == 0) return "pegadas na farinha";
    if (strcmp(nome, "Jardim") == 0) return "corda jogada";
    return NULL;
}

const char *suspeitoPorPista(const char *p) {
    if (strcmp(p, "pagina rasgada") == 0) return "Bibliotecario";
    if (strcmp(p, "bilhete suspeito") == 0) return "Maid";
    if (strcmp(p, "luva rasgada") == 0) return "Jardineiro";
    if (strcmp(p, "pegadas na farinha") == 0) return "Cozinheiro";
    if (strcmp(p, "corda jogada") == 0) return "Jardineiro";
    if (strcmp(p, "impressao no vaso") == 0) return "Bibliotecario";
    return "Desconhecido";
}

/* ==========================================================
   Exploração
   ========================================================== */

void explorar(Sala *s) {
    char opcao[10];

    while (s) {
        printf("\nVocê está em: %s\n", s->nome);

        const char *pista = pistaDaSala(s->nome);

        if (pista) {
            printf("Pista encontrada: %s\n", pista);

            if (!existePista(arvorePistas, pista)) {
                arvorePistas = inserirPista(arvorePistas, pista);
                inserirHash(pista, suspeitoPorPista(pista));
                printf("Pista guardada!\n");
            } else {
                printf("Você já tinha essa pista.\n");
            }
        }

        printf("\nPara onde deseja ir? (e = esquerda, d = direita, s = sair): ");
        fgets(opcao, 10, stdin);

        if (opcao[0] == 'e') {
            if (s->esq) s = s->esq;
            else printf("Não existe sala à esquerda.\n");
        }
        else if (opcao[0] == 'd') {
            if (s->dir) s = s->dir;
            else printf("Não existe sala à direita.\n");
        }
        else if (opcao[0] == 's') {
            printf("Saindo da exploração...\n");
            return;
        }
        else {
            printf("Opção inválida.\n");
        }
    }
}

/* ==========================================================
   Verificação final
   ========================================================== */

void contarSuspeito(NodoPista *r, const char *sus, int *cont) {
    if (!r) return;
    contarSuspeito(r->esq, sus, cont);
    char *s = buscarSuspeito(r->pista);
    if (s && strcmp(s, sus) == 0)
        (*cont)++;
    contarSuspeito(r->dir, sus, cont);
}

void julgamentoFinal() {
    char nome[100];
    printf("\nDigite o nome do suspeito que deseja acusar: ");
    fgets(nome, 100, stdin);
    nome[strcspn(nome, "\n")] = 0;

    int cont = 0;
    contarSuspeito(arvorePistas, nome, &cont);

    printf("\nPistas que apontam para %s: %d\n", nome, cont);

    if (cont >= 2)
        printf("Veredito: A acusação é válida! O culpado foi encontrado!\n");
    else
        printf("Veredito: Acusação insuficiente.\n");
}

/* ==========================================================
   Construção da mansão
   ========================================================== */

Sala *construirMansao() {
    Sala *s1 = criarSala("Sala Principal");
    Sala *s2 = criarSala("Biblioteca");
    Sala *s3 = criarSala("Estufa");
    Sala *s4 = criarSala("Escritorio");
    Sala *s5 = criarSala("Cozinha");
    Sala *s6 = criarSala("Jardim");

    s1->esq = s2;
    s1->dir = s3;

    s2->esq = s4;
    s2->dir = s5;

    s3->dir = s6;

    return s1;
}

/* ==========================================================
   MAIN
   ========================================================== */

int main() {
    Sala *mansao = construirMansao();

    printf("\n==== DETECTIVE QUEST ====\n");
    printf("Explore a mansão e colete pistas.\n");

    explorar(mansao);
    julgamentoFinal();

    return 0;
}