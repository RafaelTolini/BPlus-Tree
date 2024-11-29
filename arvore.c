#include <stdio.h>
#include <stdlib.h>

#define MAX_CHAVES 2
#define MIN_CHAVES 1
#define CHAVE_VAZIA -1

#define TRUE 1
#define FALSE 0

typedef struct Pagina {
    int numChaves;
    int numFilhos;
    int chaves[MAX_CHAVES+1];
    struct Pagina* filhos[MAX_CHAVES+2];
    int folha;
} Pagina;

Pagina* cria_pagina(int folha);
void divide__pagina_filho(Pagina* pai, int idx);
void inserir_nova_chave(Pagina* no, int chave);
Pagina* inserir(Pagina* raiz, int chave);
void imprimir_arvore(Pagina* no, int nivel);

int main() {
    Pagina* raiz = NULL;
    int chaves[] = {5,10,7,11,12,13,14, 15, 16};
    int qtd;

    qtd = sizeof(chaves) / sizeof(chaves[0]);

    for(int i = 0; i < qtd; i++){
        raiz = inserir(raiz, chaves[i]);
        // printf("APOS INSERCAO DA CHAVE %d:\n", chaves[i]);
        // imprimir_arvore(raiz, 0);
    }

    printf("APOS TODAS AS INSERCOES:\n\n");
    imprimir_arvore(raiz, 0);

    return 0;
}

// Cria nova página vazia
Pagina* cria_pagina(int folha) {
    Pagina* pagina = (Pagina*)malloc(sizeof(Pagina));
    
    pagina->numChaves = 0;
    pagina->folha = folha;
    for (int i = 0; i < MAX_CHAVES+1; i++) 
        pagina->chaves[i] = CHAVE_VAZIA;
    for (int i = 0; i < MAX_CHAVES+2; i++) 
        pagina->filhos[i] = NULL;

    return pagina;
}

//Divide a raiz criando duas novas páginas
void divide_raiz(Pagina* raiz){

    Pagina* nova_pagina_esq = cria_pagina(raiz->folha);
    Pagina* nova_pagina_dir = cria_pagina(raiz->folha);
    nova_pagina_esq->numChaves = MIN_CHAVES;
    nova_pagina_dir->numChaves = (raiz->folha)?MAX_CHAVES:MIN_CHAVES;

    if(raiz->folha)
        for (int i = 0; i < raiz->numChaves; i++)
            nova_pagina_dir->chaves[i] = raiz->chaves[i + 1];
    else
        nova_pagina_dir->chaves[0]=raiz->chaves[2];

    nova_pagina_esq->chaves[0] = raiz->chaves[0];

    raiz->chaves[0] = raiz->chaves[1];
    raiz->chaves[1] = raiz->chaves[2] = CHAVE_VAZIA;
    raiz->numChaves = 1;
    
    if(!raiz->folha){
        nova_pagina_esq->filhos[0]=raiz->filhos[0];
        nova_pagina_esq->filhos[1]=raiz->filhos[1];

        nova_pagina_dir->filhos[0]=raiz->filhos[2];
        nova_pagina_dir->filhos[1]=raiz->filhos[3];

        nova_pagina_esq->numFilhos = nova_pagina_dir->numFilhos = 2;
    }

    raiz->filhos[0] = nova_pagina_esq;
    raiz->filhos[1] = nova_pagina_dir;
    raiz->numFilhos = 2;

    raiz->folha = FALSE;
}

//Divide o filho e ajusta as chaves e ponteiros adequadamente
void divide__pagina_filho(Pagina* pai, int idx) {

    Pagina* filho = pai->filhos[idx];
    Pagina* nova_pagina = cria_pagina(filho->folha);
    nova_pagina->numChaves = (filho->folha)?MAX_CHAVES:MIN_CHAVES;

    if(filho->folha)
        for (int i = 0; i < filho->numChaves; i++)
            nova_pagina->chaves[i] = filho->chaves[i + 1];
    else
        nova_pagina->chaves[0]=filho->chaves[2];
    
    int i = pai->numChaves;
    while(i > 0 && pai->chaves[i-1] > filho->chaves[1]){
        pai->chaves[i] = pai->chaves[i-1];
        pai->filhos[i+1] = pai->filhos[i];
        i--;
    }
    pai->chaves[i] = filho->chaves[1];
    pai->filhos[idx+1] = nova_pagina;
    pai->numChaves++;
    pai->numFilhos++;

    filho->numChaves = MIN_CHAVES;
    filho->chaves[1] = filho->chaves[2] = CHAVE_VAZIA;

    if(!filho->folha){
        nova_pagina->filhos[0]=filho->filhos[2];
        nova_pagina->filhos[1]=filho->filhos[3];

        filho->filhos[2] = filho->filhos[3] = NULL;

        nova_pagina->numFilhos = filho->numFilhos = 2;
    }

    return;
}

//Insere chave normalmente em um nó folha e avalia recursivamente no caminho da chave
//inserida se o filho precisa ser dividido
void inserir_nova_chave(Pagina* pagina, int chave) {
    int i = pagina->numChaves;

    if(pagina->folha){
        while (i > 0 && pagina->chaves[i-1] > chave) {
            pagina->chaves[i] = pagina->chaves[i-1];
            i--;
        }
        pagina->chaves[i] = chave;
        pagina->numChaves++;

        return;
    }

    while(i > 0 && pagina->chaves[i-1] > chave)i--;

    inserir_nova_chave(pagina->filhos[i], chave);

    if(pagina->filhos[i]->numChaves > MAX_CHAVES)divide__pagina_filho(pagina, i);

    return;
}

//Cria a raiz ou insere um nó e verifica caso especial de divisão da raiz
Pagina* inserir(Pagina* raiz, int chave) {
    if(!raiz){
        raiz = cria_pagina(TRUE);
        raiz->chaves[0] = chave;
        raiz->numChaves++;
        return raiz;
    }

    inserir_nova_chave(raiz, chave);

    if(raiz->numChaves > MAX_CHAVES)
        divide_raiz(raiz);

    return raiz;
}

//Imprime cada pagina seguida de seus filhos (percurso pela)
//árvore em pré-ordem
void imprimir_arvore(Pagina* pagina, int nivel) {
    if(!pagina)return;

    printf("NIVEL %d - PAGINA %p:\n\n", nivel, pagina);

    printf("CHAVES:\n");
    for(int i = 0; i < MAX_CHAVES; i++){
        if(i < pagina->numChaves)
            printf("%d  ", pagina->chaves[i]);
        else
            printf("%d  ", CHAVE_VAZIA);
    }
    printf("\n\n");

    printf("FILHOS:\n");
    for (int i = 0; i < MAX_CHAVES + 1; i++){
        if (pagina->filhos[i])
            printf("%p  ", pagina->filhos[i]);

        else 
            printf("%d  ", CHAVE_VAZIA);   
    }
    printf("\n\n");

    printf("################################################\n");

    for(int i = 0; i < pagina->numFilhos; i++)
        if(pagina->filhos[i]) 
            imprimir_arvore(pagina->filhos[i], nivel + 1);
}