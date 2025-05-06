#ifndef Descompactar_h
#define Descompactar_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <locale.h>
#include <windows.h>
#include <time.h>


// Estrutura do nó da árvore de Huffman
typedef struct nodo {
    void* simbolo;
    struct nodo* esquerda;
    struct nodo* direita;
} Nodo;

// Funções utilitárias
int verifica_bit(unsigned char byte, int pos);
void interpretar_cabecalho(FILE* arq, int* lixo, int* tamanho_arvore);

// Funções de árvore de Huffman
Nodo* novo_nodo(unsigned char simb);
Nodo* reconstruir_arvore(unsigned char* dados, int* idx, int total);
void liberar_arvore(Nodo* no);

// Leitura e reconstrução
void ler_arvore(FILE* arq, unsigned char* buffer, int tamanho);

// Decodificação
void decodificar_arquivo(FILE* comprimido, FILE* destino, Nodo* raiz, int lixo);
void Descomprimir_Dados();

#endif 
