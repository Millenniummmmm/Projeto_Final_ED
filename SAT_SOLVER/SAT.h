#ifndef SAT_h
#define SAT_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <windows.h>

#define MAX_VAR 2000
#define MAX_CLAUSULAS 2500
#define MAX_LITERAIS 200

#define SATISFEITA 0
#define CONTRADICAO 1
#define INDEFINIDA 2

// essas duas primeiras structs servem apenas para guardar dados. A lógica de decisão e busca ta implementada na struct arvore
typedef struct{
    int literais[MAX_LITERAIS]; //literais da clausula
    int tamanho; //quantos literais essa clausula tem
}Clausula;

typedef struct{
    Clausula clausulas[MAX_CLAUSULAS]; //lista de clausulas
    
    int num_clausulas;  //quantas clausulas existem
    int num_variaveis;  //quantas variaveis (literais) existem
}CNF;

typedef struct Arvore{
    int variavel; //variavel escolhida no nó. serve para saber a decisão que ta sendo tomada
    int valor;  //1 ou -1 (verdadeiro ou falso)
    int atribuicoes[MAX_VAR]; //cópia das atribuicoes feitas (0, -1, 1)
    
    struct Arvore *esq; //vdd (1)
    struct Arvore *dir; //f (-1)
}Arvore;

void ler_arquivo(const char *nome_arq, CNF *expressao);
int verificar_cnf(CNF *cnf, int atribuicoes[]);
bool sat(Arvore *no, CNF *expressao, int solucao[]);
void rodar();

#endif