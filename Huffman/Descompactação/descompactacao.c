#include "Descompactar.h" // Inclui o cabeçalho com definições e estruturas necessárias

// Função que verifica o valor de um bit específico de um byte
int verifica_bit(unsigned char byte, int pos) {
    return (byte >> pos) & 1; // Retorna 1 se o bit na posição 'pos' for 1, senão retorna 0
}

// Função que interpreta o cabeçalho do arquivo compactado
// Extrai os 3 bits de lixo e os 13 bits de tamanho da árvore
void interpretar_cabecalho(FILE* arq, int* lixo, int* tamanho_arvore) {
    unsigned char primeiro, segundo;
    fread(&primeiro, 1, 1, arq); // Lê o primeiro byte do cabeçalho
    fread(&segundo, 1, 1, arq);  // Lê o segundo byte

    *lixo = primeiro >> 5; // Os 3 bits mais significativos indicam a quantidade de bits lixo no último byte
    *tamanho_arvore = ((primeiro & 0x1F) << 8) | segundo; // Os 5 bits restantes + segundo byte formam o tamanho da árvore
}

// Cria um novo nodo da árvore de Huffman com o símbolo passado
Nodo* novo_nodo(unsigned char simb) {
    Nodo* novo = (Nodo*) malloc(sizeof(Nodo)); // Aloca memória para o novo nodo
    if (!novo) {
        fprintf(stderr, "Erro de alocação!\n");
        return NULL;
    }
    unsigned char* ptr = (unsigned char*) malloc(sizeof(unsigned char)); // Aloca memória para o símbolo
    *ptr = simb;
    novo->simbolo = (void*) ptr;
    novo->esquerda = novo->direita = NULL; // Nodo folha
    return novo;
}

// Reconstrói recursivamente a árvore de Huffman a partir da sequência em pré-ordem
Nodo* reconstruir_arvore(unsigned char* dados, int* idx, int total) {
    if (*idx >= total) return NULL; // Caso base: fim do vetor

    unsigned char atual = dados[(*idx)++]; // Lê o caractere atual

    // Se não for '*', é um caractere folha
    if (atual != '*') {
        if (atual == '\\') atual = dados[(*idx)++]; // Trata escape (ex: '\\*' ou '\\\\')
        return novo_nodo(atual);
    }

    // Se for '*', cria um nodo interno e continua recursivamente
    Nodo* pai = novo_nodo('*');
    pai->esquerda = reconstruir_arvore(dados, idx, total);  // Constrói subárvore esquerda
    pai->direita = reconstruir_arvore(dados, idx, total);   // Constrói subárvore direita
    return pai;
}

// Lê os bytes da árvore serializada do arquivo
void ler_arvore(FILE* arq, unsigned char* buffer, int tamanho) {
    fread(buffer, 1, tamanho, arq);
}

// Decodifica os dados compactados usando a árvore reconstruída
void decodificar_arquivo(FILE* comprimido, FILE* destino, Nodo* raiz, int lixo) {
    Nodo* atual = raiz;
    unsigned char byte, anterior;
    fread(&anterior, 1, 1, comprimido); // Lê o primeiro byte (após cabeçalho e árvore)

    // Processa todos os bytes do arquivo
    while (fread(&byte, 1, 1, comprimido)) {
        for (int i = 7; i >= 0; i--) { // Percorre os bits do byte anterior
            atual = verifica_bit(anterior, i) ? atual->direita : atual->esquerda;
            if (!atual->esquerda && !atual->direita) { // Chegou em uma folha
                fwrite((unsigned char*)atual->simbolo, 1, 1, destino); // Escreve o símbolo
                atual = raiz; // Reinicia na raiz
            }
        }
        anterior = byte; // Atualiza o byte anterior
    }

    // Trata o último byte (desconsiderando os bits de lixo no final)
    for (int i = 7; i >= lixo; i--) {
        atual = verifica_bit(anterior, i) ? atual->direita : atual->esquerda;
        if (!atual->esquerda && !atual->direita) {
            fwrite((unsigned char*)atual->simbolo, 1, 1, destino);
            atual = raiz;
        }
    }
    printf("Descompactação concluída com sucesso!\n");
}

// Libera recursivamente a memória da árvore de Huffman
void liberar_arvore(Nodo* no) {
    if (!no) return;
    liberar_arvore(no->esquerda);
    liberar_arvore(no->direita);
    free(no->simbolo);
    free(no);
}

// Função principal que realiza o processo completo de descompactação
void Descomprimir_Dados() {

    // Solicita ao usuário o nome do arquivo
    printf("\033[1;32m");
    printf("Digite o nome do arquivo a ser descompactado: ");
    char nome_arquivo[FILENAME_MAX];
    scanf(" %[^\n]", nome_arquivo);
    printf("\033[0m");

    // Monta o caminho do arquivo de entrada (pasta fixa C:\Huffman)
    char caminho_completo[FILENAME_MAX];
    snprintf(caminho_completo, FILENAME_MAX, "C:\\Huffman\\%s", nome_arquivo);

    FILE* entrada = fopen(caminho_completo, "rb"); // Abre o arquivo compactado
    if (!entrada) {
        fprintf(stderr, "Erro ao abrir arquivo compactado!\n");
        exit(EXIT_FAILURE);
    }

    // Lê o cabeçalho do arquivo
    int lixo = 0, tam_arvore = 0;
    interpretar_cabecalho(entrada, &lixo, &tam_arvore);

    // Lê os dados da árvore serializada
    unsigned char* preordem = (unsigned char*) malloc(tam_arvore);
    ler_arvore(entrada, preordem, tam_arvore);

    // Reconstrói a árvore de Huffman a partir da sequência lida
    int pos = 0;
    Nodo* raiz = reconstruir_arvore(preordem, &pos, tam_arvore);
    free(preordem); // Libera o buffer da árvore

    // Remove a extensão do nome do arquivo (ex: ".huff")
    int nome_arquivo_len = strlen(nome_arquivo);
    for(int i = nome_arquivo_len - 1; i >= 0; i--) {
        if(nome_arquivo[i] == '.') {
            nome_arquivo[i] = '\0'; // Corta a extensão
            break;
        }
    }

    // Cria o caminho do arquivo de saída com o mesmo nome (sem extensão)
    char caminho_completo_final[FILENAME_MAX];
    snprintf(caminho_completo_final, FILENAME_MAX, "C:\\Huffman\\%s", nome_arquivo);

    FILE* saida = fopen(caminho_completo_final, "wb"); // Cria arquivo de saída
    if (!saida) {
        fprintf(stderr, "Erro ao criar arquivo de saída!\n");
        liberar_arvore(raiz);
        fclose(entrada);
        return;
    }

    // Decodifica os dados e escreve no novo arquivo
    decodificar_arquivo(entrada, saida, raiz, lixo);

    // Libera a memória usada e fecha arquivos
    liberar_arvore(raiz);
    fclose(entrada);
    fclose(saida);
}
