#include "Descompactar.h"

int verifica_bit(unsigned char byte, int pos) {
    return (byte >> pos) & 1;
}

void interpretar_cabecalho(FILE* arq, int* lixo, int* tamanho_arvore) {
    unsigned char primeiro, segundo;
    fread(&primeiro, 1, 1, arq);
    fread(&segundo, 1, 1, arq);

    *lixo = primeiro >> 5; // 3 bits mais significativos
    *tamanho_arvore = ((primeiro & 0x1F) << 8) | segundo; // 5 bits + 8 bits
}

Nodo* novo_nodo(unsigned char simb) {
    Nodo* novo = (Nodo*) malloc(sizeof(Nodo));
    if (!novo) {
        fprintf(stderr, "Erro de alocação!\n");
        return NULL;
    }
    unsigned char* ptr = (unsigned char*) malloc(sizeof(unsigned char));
    *ptr = simb;
    novo->simbolo = (void*) ptr;
    novo->esquerda = novo->direita = NULL;
    return novo;
}

Nodo* reconstruir_arvore(unsigned char* dados, int* idx, int total) {
    if (*idx >= total) return NULL;

    unsigned char atual = dados[(*idx)++];

    if (atual != '*') {
        if (atual == '\\') atual = dados[(*idx)++];
        return novo_nodo(atual);
    }

    Nodo* pai = novo_nodo('*');
    pai->esquerda = reconstruir_arvore(dados, idx, total);
    pai->direita = reconstruir_arvore(dados, idx, total);
    return pai;
}

void ler_arvore(FILE* arq, unsigned char* buffer, int tamanho) {
    fread(buffer, 1, tamanho, arq);
}

void decodificar_arquivo(FILE* comprimido, FILE* destino, Nodo* raiz, int lixo) {
    Nodo* atual = raiz;
    unsigned char byte, anterior;
    fread(&anterior, 1, 1, comprimido); // primeiro byte já lido no header

    while (fread(&byte, 1, 1, comprimido)) {
        for (int i = 7; i >= 0; i--) {
            atual = verifica_bit(anterior, i) ? atual->direita : atual->esquerda;
            if (!atual->esquerda && !atual->direita) {
                fwrite((unsigned char*)atual->simbolo, 1, 1, destino);
                atual = raiz;
            }
        }
        anterior = byte;
    }

    for (int i = 7; i >= lixo; i--) {
        atual = verifica_bit(anterior, i) ? atual->direita : atual->esquerda;
        if (!atual->esquerda && !atual->direita) {
            fwrite((unsigned char*)atual->simbolo, 1, 1, destino);
            atual = raiz;
        }
    }
    printf("Descompactação concluída com sucesso!\n");
}

void liberar_arvore(Nodo* no) {
    if (!no) return;
    liberar_arvore(no->esquerda);
    liberar_arvore(no->direita);
    free(no->simbolo);
    free(no);
}

void Descomprimir_Dados() {
    FILE* entrada = fopen("C:\\Huffman\\compactado.huff", "rb");
    if (!entrada) {
        fprintf(stderr, "Erro ao abrir arquivo compactado!\n");
        return 0;
    }

    int lixo = 0, tam_arvore = 0;
    interpretar_cabecalho(entrada, &lixo, &tam_arvore);

    unsigned char* preordem = (unsigned char*) malloc(tam_arvore);
    ler_arvore(entrada, preordem, tam_arvore);

    int pos = 0;
    Nodo* raiz = reconstruir_arvore(preordem, &pos, tam_arvore);
    free(preordem);

    FILE* saida = fopen("C:\\Huffman\\saida.out", "wb");
    if (!saida) {
        fprintf(stderr, "Erro ao criar arquivo de saída!\n");
        liberar_arvore(raiz);
        fclose(entrada);
        return;
    }

    decodificar_arquivo(entrada, saida, raiz, lixo);

    liberar_arvore(raiz);
    fclose(entrada);
    fclose(saida);
}