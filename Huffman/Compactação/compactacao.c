#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <locale.h>

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
}Lista;

// ------------------------- Compactação ------------------------- //

// Primeiro Passo: Tabela de Frequências ------------------------- //

// Cria a tabela de frequências e inicializa com zero
void Inicializar_Tabela(unsigned int *tabela){
    for (int i = 0; i < TAMANHO_MAX; i++) {
        tabela[i] = 0;
    }
}

// Vai receber o arquivo e a tabela inicializada com zero
void Definir_Tabela_Freq(FILE *arquivo, unsigned int *tab_frequencia) {
    unsigned char index = 0; 
    while (fread(&index, 1, 1, arquivo) == 1) { // Lê o arquivo byte a byte
        tab_frequencia[index]++; // Incrementa a frequência do símbolo lido, preechendo a tabela
    }
}

// Função para testes
void imprimir_tabela(unsigned int *tabela) {
    printf("Tabela de Frequências:\n");
    for (int i = 0; i < TAMANHO_MAX; i++) {
        if (tabela[i] > 0) {
            printf("Simbolo: %c, Frequencia: %u\n", i, tabela[i]);
        }
    }
}

// Segundo Passo: Listas Encadeadas Ordenadas ------------------------- //

// Inicializa a lista
void Criar_Lista_Encadeada(Lista *l) {
    l->head = NULL; // Inicializa a cabeça da lista como NULL
    l->tamanho = 0; // Inicializa o tamanho da lista como 0  
}
// Vai inserir os nós da árvore na lista encadeada de forma ordenada. Recebe o nó a ser inserido e a lista encadeada
// No final de tudo, a head vai apontar para o no com menor frequencia.
void Inserir_em_Ordem(Lista *l, Base *base){
    if(l->head == NULL || *(unsigned int *)base->frequencia < *(unsigned int *)l->head->frequencia){
         // Se caso a frequencia for menor que a frequencia do inicio da lista ou se a lista estiver vazia
         base->proximo = l->head; // Adiciona o nó no início da lista, se caso head apontar pra nulo agora o novo nó vai apontar pra nulo. Se caso apontar pra outro no, faz a troca de ponteiros.
         l->head = base; // Agora head vai apontar pra o novo nó adicionado
         l->tamanho++; // Incrementa o tamanho da lista
         return;
    }  
 
    Base *atual = l->head; // Inicializa um ponteiro auxiliar para percorrer a lista
    while(atual->proximo != NULL && *(unsigned int *)atual->proximo->frequencia <= *(unsigned int *)base->frequencia){
        atual = atual->proximo; // Percorre a lista até encontrar a posição correta
    }
    base->proximo = atual->proximo; // Insere o nó na posição correta
    atual->proximo = base; // Atualiza o ponteiro do nó anterior
    l->tamanho++; // Incrementa o tamanho da lista
}
// Vai preencher a lista encadeada com os nós criados a partir da tabela de frequências. Recebe a lista encadeada vazia e a tabela de frequências
void Preencher_Lista_Encadeada(Lista *l, unsigned int *tabela_frequencia) {
    for (int i = 0; i < TAMANHO_MAX; i++) {
        if (tabela_frequencia[i] > 0) { // Verifica se a frequência é maior que zero. As frequencias maiores que zero que importam

            Base *novo_nodo = malloc(sizeof(Base)); // Aloca memória para o novo nó

            if (!novo_nodo) { // Verifica se houve algum erro na alocação
                perror("Erro ao alocar memória para o nó da lista encadeada.\n");
                exit(EXIT_FAILURE); // Encerra o programa se a alocação falhar
            }

            // Aloca memória para o símbolo
            unsigned char *simbolo = malloc(sizeof(unsigned char));
            *simbolo = (unsigned char)i;
            novo_nodo->dados = simbolo;

            // Aloca memória para a frequência 
            unsigned int *frequencia = malloc(sizeof(unsigned int));
            *frequencia = tabela_frequencia[i];
            novo_nodo->frequencia = frequencia;
            // Inicializa os ponteiros para nulo pois não sabemos onde eles vão apontar ainda
            novo_nodo->proximo = NULL; 
            novo_nodo->direita = NULL; 
            novo_nodo->esquerda = NULL; 
            Inserir_em_Ordem(l, novo_nodo); // Vai garantir que a lista esteja ordenada.

        }
    }
}
// Função para testar a lista encadeada
void imprimir_lista(Lista *l) {
    Base *atual = l->head; // Inicializa um ponteiro auxiliar para percorrer a lista
    printf("Lista Encadeada:\n");
    printf("Tamanho da lista: %d\n", l->tamanho); // Imprime o tamanho da lista
    while (atual != NULL) { // Enquanto o ponteiro atual não for NULL
        printf("Símbolo: %c, Frequência: %u\n", *(unsigned char *)atual->dados, *(unsigned int *)atual->frequencia);// Imprime o símbolo e a frequência
        atual = atual->proximo; // Avança para o próximo nó
    }
}

int main() {
    //setlocale(LC_ALL, "Portuguese"); // Define a localidade para português
    printf("Digite o nome do arquivo a ser compactado: ");
    // Aloca memória para o nome do arquivo
    char *nome_arquivo = calloc(FILENAME_MAX, sizeof(char));
    scanf(" %[^\n]", nome_arquivo);

    // Caminho base
    const char *caminho_base = "C:\\Huffman\\";
    
    // String final com caminho completo
    char caminho_completo[FILENAME_MAX];
    snprintf(caminho_completo, FILENAME_MAX, "%s%s", caminho_base, nome_arquivo); // Concatena o caminho base com o nome do arquivo

    // Tenta abrir o arquivo
    FILE *arquivo = fopen(caminho_completo, "rb");

    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // ----- Testes ----- //
    // Teste do passo 1: Tabela de Frequências
    unsigned int tabela_frequencia[TAMANHO_MAX]; // Tabela de frequências
    Inicializar_Tabela(tabela_frequencia); // Inicializa a tabela de frequências com zero
    Definir_Tabela_Freq(arquivo, tabela_frequencia); // Preenche a tabela de frequências
    fclose(arquivo); // Fecha o arquivo
    imprimir_tabela(tabela_frequencia); // Imprime a tabela de frequências
    // --------------------- //

    // Teste do passo 2: Lista Encadeada
    Lista lista; // Declara a lista encadeada
    Criar_Lista_Encadeada(&lista); // Inicializa a lista encadeada
    Preencher_Lista_Encadeada(&lista, tabela_frequencia); // Preenche a lista encadeada com os nós criados a partir da tabela de frequências
    imprimir_lista(&lista); // Imprime a lista encadeada
    // --------------------- //




    return 0;
}
