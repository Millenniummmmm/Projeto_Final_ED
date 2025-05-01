// Esboço inicial

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Estrutura do nó da árvore de Huffman
typedef struct No {
    unsigned char caractere;
    struct No* esquerda;
    struct No* direita;
} No;

// Função para reconstruir a árvore de Huffman a partir da serialização pré-ordem
No* reconstruir_arvore(const unsigned char* dados, int* indice) {
    if (dados[*indice] == '*') {
        (*indice)++;
        No* no = (No*)malloc(sizeof(No));
        no->caractere = '*';
        no->esquerda = reconstruir_arvore(dados, indice);
        no->direita = reconstruir_arvore(dados, indice);
        return no;
    } else {
        No* no = (No*)malloc(sizeof(No));
        no->caractere = dados[(*indice)++];
        no->esquerda = NULL;
        no->direita = NULL;
        return no;
    }
}

// Função para liberar a árvore de Huffman
void liberar_arvore(No* raiz) {
    if (!raiz) return;
    liberar_arvore(raiz->esquerda);
    liberar_arvore(raiz->direita);
    free(raiz);
}

// Função principal de descompactação
void* descompactar(void* dados_compactados, int tamanho_entrada, int* tamanho_saida) {
    unsigned char* entrada = (unsigned char*)dados_compactados;
    
    // Leitura do tamanho da árvore serializada (2 bytes)
    int tam_arvore = (entrada[0] << 8) | entrada[1];

    // Reconstruir a árvore de Huffman
    int indice = 2; // Pula os 2 bytes do tamanho
    No* raiz = reconstruir_arvore(entrada, &indice);

    // Número de bits úteis no último byte da codificação
    unsigned char bits_finais = entrada[indice++];
    
    // Ponteiro para os dados compactados
    unsigned char* dados_codificados = entrada + indice;
    int tamanho_codificado = tamanho_entrada - indice;

    // Estimar o tamanho máximo da saída (pior caso: 1 byte por caractere)
    int capacidade = 1024;
    int tamanho_atual = 0;
    unsigned char* saida = (unsigned char*)malloc(capacidade);

    No* atual = raiz;
    int total_bits = (tamanho_codificado - 1) * 8 + bits_finais;

    for (int i = 0; i < total_bits; i++) {
        int byte_index = i / 8;
        int bit_index = 7 - (i % 8);
        int bit = (dados_codificados[byte_index] >> bit_index) & 1;

        atual = bit ? atual->direita : atual->esquerda;

        if (atual->esquerda == NULL && atual->direita == NULL) {
            if (tamanho_atual >= capacidade) {
                capacidade *= 2;
                saida = (unsigned char*)realloc(saida, capacidade);
            }
            saida[tamanho_atual++] = atual->caractere;
            atual = raiz;
        }
    }

    liberar_arvore(raiz);
    *tamanho_saida = tamanho_atual;
    return saida;
}
