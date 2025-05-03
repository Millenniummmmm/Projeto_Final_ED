#ifndef Compactar_h
#define Compactar_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <locale.h>
#include <windows.h>
#include <time.h>

#define TAMANHO_MAX 256

// Estrutura de Dados principal 
typedef struct Base {
    void* dados;
    void* frequencia;
    struct Base *proximo;
    struct Base *direita;
    struct Base *esquerda;
} Base;

// Estrutura de Dados auxiliar
typedef struct Lista{
    Base *head;
    int tamanho;
} Lista;

// Funções utilitárias
void Huffman();
void Informacoes();
void Menu_Principal();
void Destruir_Arvore(Base* raiz);
void Liberar_Dicionario(char **dicionario);
void Liberar_Lista(Lista *l);

// Funções de compactação
void Inicializar_Tabela(unsigned int *tabela);
void Definir_Tabela_Freq(FILE *arquivo, unsigned int *tab_frequencia);
void Criar_Lista_Encadeada(Lista *l);
void Inserir_em_Ordem(Lista *l, Base *base);
void Preencher_Lista_Encadeada(Lista *l, unsigned int *tabela_frequencia);
Base* Pegar_Node_Inicial(Lista *l);
Base* Construir_Arvore_de_Huffman(Lista *no);
int Calcular_Altura_Arvore(Base *raiz);
char** Definir_Dicionario(int colunas);
void Completar_Dicionario(Base* raiz, char** dicionario, char *codigo, int colunas);
int Calcular_Lixo(long long tamanho_bits);
bool Caracter_Especial(unsigned char c);
void Escrever_Pre_Ordem(Base* raiz, char* buffer, int* posicao);
int Calcular_Tamanho_Arvore(Base* raiz);
char* Arvore_Pre_Ordem(Base* raiz);
void Compactar_Arquivo(unsigned char *dados, long long tamanhoArquivo, char **dicionario, Base *HuffTree);

// Função principal de compressão
void Comprimir_Dados();

#endif // COMPACTACAO_H
