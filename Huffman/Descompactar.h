#ifndef Descompactar_h
#define Descompactar_h

/* Inclusões de bibliotecas necessárias */
#include <stdio.h>   // Para operações de arquivo (FILE, fread, etc.)
#include <stdlib.h>  // Para alocação de memória (malloc, free)
#include <string.h>  // Para manipulação de strings (strlen)
#include <limits.h>  // Para constantes de limites (não usadas diretamente aqui)
#include <math.h>    // Para funções matemáticas (não usadas diretamente aqui)
#include <stdbool.h> // Para tipo booleano (bool)
#include <locale.h>  // Para configuração de localidade
#include <windows.h> // Para funcionalidades específicas do Windows
#include <time.h>    // Para funções de tempo (não usadas diretamente aqui)

/* Estrutura do nó da árvore de Huffman
 * Representa cada nó da árvore usada na decodificação
 * Campos:
 *   simbolo - ponteiro para o caractere (folha) ou NULL (nós internos)
 *   esquerda - ponteiro para o filho esquerdo
 *   direita - ponteiro para o filho direito
 */
typedef struct nodo {
    void* simbolo;          // Ponteiro genérico para o símbolo (folhas)
    struct nodo* esquerda;  // Subárvore esquerda (bit 0)
    struct nodo* direita;   // Subárvore direita (bit 1)
} Nodo;

/* Seção de declarações de funções */

// Funções utilitárias
int verifica_bit(unsigned char byte, int pos);
/* Verifica se um bit específico está setado em um byte
 * Parâmetros:
 *   byte - o byte a ser verificado
 *   pos - posição do bit (0-7)
 * Retorno:
 *   1 se o bit estiver setado, 0 caso contrário
 */

void interpretar_cabecalho(FILE* arq, int* lixo, int* tamanho_arvore);
/* Interpreta o cabeçalho do arquivo compactado
 * Parâmetros:
 *   arq - ponteiro para o arquivo compactado
 *   lixo - ponteiro para armazenar quantidade de bits lixo
 *   tamanho_arvore - ponteiro para armazenar tamanho da árvore
 */

// Funções de manipulação da árvore de Huffman
Nodo* novo_nodo(unsigned char simb);
/* Cria um novo nó da árvore
 * Parâmetro:
 *   simb - caractere a ser armazenado no nó
 * Retorno:
 *   Ponteiro para o novo nó alocado
 */

Nodo* reconstruir_arvore(unsigned char* dados, int* idx, int total);
/* Reconstrói a árvore de Huffman a partir de sua representação serializada
 * Parâmetros:
 *   dados - vetor com a representação em pré-ordem
 *   idx - ponteiro para índice atual (é incrementado durante a reconstrução)
 *   total - tamanho total do vetor
 * Retorno:
 *   Ponteiro para a raiz da árvore reconstruída
 */

void liberar_arvore(Nodo* no);
/* Libera a memória alocada para a árvore
 * Parâmetro:
 *   no - raiz da árvore a ser liberada
 */

// Funções de leitura e reconstrução
void ler_arvore(FILE* arq, unsigned char* buffer, int tamanho);
/* Lê a representação serializada da árvore do arquivo
 * Parâmetros:
 *   arq - arquivo de entrada
 *   buffer - buffer para armazenar os dados
 *   tamanho - quantidade de bytes a ler
 */

// Funções de decodificação
void decodificar_arquivo(FILE* comprimido, FILE* destino, Nodo* raiz, int lixo);
/* Decodifica o arquivo compactado usando a árvore de Huffman
 * Parâmetros:
 *   comprimido - arquivo compactado (posicionado após a árvore)
 *   destino - arquivo de saída
 *   raiz - raiz da árvore de Huffman
 *   lixo - quantidade de bits lixo no último byte
 */

void Descomprimir_Dados();
/* Função principal que coordena todo o processo de descompactação
 * Interface com o usuário e fluxo principal de operações
 */

#endif // Fim da diretiva de pré-processador
