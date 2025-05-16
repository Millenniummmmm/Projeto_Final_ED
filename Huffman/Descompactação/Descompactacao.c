#include "Descompactar.h" // Inclui as funções necessárias

/* Função: interpretar_cabecalho
 * Extrai informações do cabeçalho do arquivo compactado
 * Parâmetros:
 *   arq - ponteiro para o arquivo compactado
 *   lixo - ponteiro para armazenar quantidade de bits lixo
 *   tamanho_arvore - ponteiro para armazenar tamanho da árvore
 */
void interpretar_cabecalho(FILE* arq, int* lixo, int* tamanho_arvore) {
    unsigned char primeiro, segundo;
    fread(&primeiro, 1, 1, arq); // Lê primeiro byte (contém 3 bits lixo + 5 bits altos do tamanho)
    fread(&segundo, 1, 1, arq);  // Lê segundo byte (contém 8 bits baixos do tamanho)

    *lixo = primeiro >> 5; // Extrai os 3 bits mais significativos (bits 7-5)
    *tamanho_arvore = ((primeiro & 31) << 8) | segundo; // Combina os 5 bits restantes com o segundo byte, 31 porque 2^5 = 32 posições
}

/* Função: ler_arvore
 * Lê os bytes da árvore serializada do arquivo
 * Parâmetros:
 *   arq - arquivo de entrada
 *   buffer - buffer para armazenar os dados lidos
 *   tamanho - quantidade de bytes a ler
 */
void ler_arvore(FILE* arq, unsigned char* buffer, int tamanho) {
    fread(buffer, 1, tamanho, arq); // Lê 'tamanho' bytes do arquivo para o buffer
}

/* Função: novo_no
 * Cria um novo nó da árvore de Huffman
 * Parâmetro:
 *   simb - caractere a ser armazenado no nó
 * Retorno:
 *   Ponteiro para o novo nó alocado
 */
Nodo* novo_no(unsigned char simb) {
    Nodo* novo = (Nodo*) malloc(sizeof(Nodo)); // Aloca estrutura do nó
    if (!novo) {
        fprintf(stderr, "Erro de alocação!\n");
        return NULL;
    }
    unsigned char* ptr = (unsigned char*) malloc(sizeof(unsigned char)); // Aloca espaço para o símbolo
    *ptr = simb;
    novo->simbolo = ptr;
    novo->esquerda = novo->direita = NULL; // Inicializa filhos como NULL (nó folha)
    return novo;
}

/* Função: reconstruir_arvore
 * Reconstrói a árvore de Huffman a partir de sua representação em pré-ordem
 * Parâmetros:
 *   dados - vetor com a representação da árvore
 *   idx - ponteiro para índice atual no vetor (é incrementado durante a reconstrução)
 *   total - tamanho total do vetor
 * Retorno:
 *   Ponteiro para a raiz da árvore reconstruída
 */
Nodo* reconstruir_arvore(unsigned char* dados, int* idx, int total) {
    if (*idx >= total) return NULL; // Verifica fim do vetor

    unsigned char atual = dados[(*idx)++]; // Lê próximo caractere e avança índice

    if (atual != '*') { // Se não for nó interno
        if (atual == '\\') // Se for caractere de escape
            atual = dados[(*idx)++]; // Lê próximo caractere (trata '*' ou '\' escapados)
        return novo_no(atual); // Cria nó folha
    }

    // Se for '*', é nó interno - reconstroje subárvores recursivamente
    Nodo* pai = novo_no('*');
    pai->esquerda = reconstruir_arvore(dados, idx, total);
    pai->direita = reconstruir_arvore(dados, idx, total);
    return pai;
}

/* Função: verifica_bit
 * Verifica o valor de um bit específico em um byte
 * Parâmetros:
 *   byte - o byte a ser analisado
 *   pos - posição do bit a verificar (0-7, onde 0 é o bit menos significativo)
 * Retorno:
 *   1 se o bit estiver setado, 0 caso contrário
 */
int verifica_bit(unsigned char byte, int pos) {
    return (byte >> pos) & 1; // Desloca o byte 'pos' posições para direita e faz AND com 1
}

/* Função: decodificar_arquivo
 * Decodifica o conteúdo compactado usando a árvore de Huffman
 * Parâmetros:
 *   comprimido - arquivo compactado (posicionado após a árvore)
 *   destino - arquivo de saída
 *   raiz - raiz da árvore de Huffman
 *   lixo - quantidade de bits lixo no último byte
 */
void decodificar_arquivo(FILE* comprimido, FILE* destino, Nodo* raiz, int lixo) {
    Nodo* atual = raiz;
    unsigned char proximo, anterior;
    fread(&anterior, 1, 1, comprimido); // Lê primeiro byte de dados

    while (fread(&proximo, 1, 1, comprimido)) { // Lê todos os bytes restantes
        for (int i = 7; i >= 0; i--) { // Processa cada bit do byte anterior
            atual = verifica_bit(anterior, i) ? atual->direita : atual->esquerda; //verifica bit 1 ou 0
            if (!atual->esquerda && !atual->direita) { // Chegou em folha
                fwrite((unsigned char*)atual->simbolo, 1, 1, destino); 
                atual = raiz; // Volta para raiz
            }
        }
        anterior = proximo; // Atualiza byte anterior
    }
    // Processa último byte (ignorando bits de lixo)
    for (int i = 7; i >= lixo; i--) {
        atual = verifica_bit(anterior, i) ? atual->direita : atual->esquerda;
        if (!atual->esquerda && !atual->direita) {
            fwrite((unsigned char*)atual->simbolo, 1, 1, destino);
            atual = raiz;
        }
    }
    printf("Descompactação concluída com sucesso!\n");
}

/* Função: liberar_arvore
 * Libera memória alocada para a árvore de Huffman (pós-ordem)
 * Parâmetro:
 *   no - nó atual sendo liberado (começa pela raiz)
 */
void liberar_arvore(Nodo* no) {
    if (!no) return;
    liberar_arvore(no->esquerda); // Libera subárvore esquerda
    liberar_arvore(no->direita);  // Libera subárvore direita
    free(no->simbolo); // Libera símbolo
    free(no);          // Libera nó
}

/* Função: Descomprimir_Dados
 * Função principal que coordena todo o processo de descompactação
 */
void Descomprimir_Dados() {
    // Solicita nome do arquivo compactado
    printf("\033[1;32m"); // Código ANSI para texto verde brilhante
    printf("Digite o nome do arquivo a ser descompactado: ");
    char nome_arquivo[FILENAME_MAX];
    scanf(" %[^\n]", nome_arquivo); // Lê até encontrar nova linha
    printf("\033[0m"); // Reseta cor do texto

    // Monta caminho completo (assume diretório C:\Huffman)
    char caminho_completo[FILENAME_MAX];
    snprintf(caminho_completo, FILENAME_MAX, "C:\\Huffman\\%s", nome_arquivo);

    FILE* entrada = fopen(caminho_completo, "rb");
    if (!entrada) {
        fprintf(stderr, "Erro ao abrir arquivo compactado!\n");
        exit(EXIT_FAILURE);
    }

    // Lê cabeçalho para obter lixo e tamanho da árvore
    int lixo = 0, tam_arvore = 0;
    interpretar_cabecalho(entrada, &lixo, &tam_arvore);

    // Lê dados da árvore
    unsigned char* preordem = (unsigned char*) malloc(tam_arvore);
    ler_arvore(entrada, preordem, tam_arvore);

    // Reconstrói árvore
    int pos = 0;
    Nodo* raiz = reconstruir_arvore(preordem, &pos, tam_arvore);
    free(preordem); // Libera buffer temporário

    // Remove extensão do nome do arquivo (ex: ".huff")
    int nome_arquivo_len = strlen(nome_arquivo);
    for(int i = nome_arquivo_len - 1; i >= 0; i--) {
        if(nome_arquivo[i] == '.') {
            nome_arquivo[i] = '\0';
            break;
        }
    }

    // Cria arquivo de saída (mesmo nome sem extensão)
    char caminho_completo_final[FILENAME_MAX];
    snprintf(caminho_completo_final, FILENAME_MAX, "C:\\Huffman\\%s", nome_arquivo);

    FILE* saida = fopen(caminho_completo_final, "wb");
    if (!saida) {
        fprintf(stderr, "Erro ao criar arquivo de saída!\n");
        liberar_arvore(raiz);
        fclose(entrada);
        return;
    }

    // Decodifica conteúdo e escreve no arquivo de saída
    decodificar_arquivo(entrada, saida, raiz, lixo);

    // Limpeza final
    liberar_arvore(raiz);
    fclose(entrada);
    fclose(saida);
}
