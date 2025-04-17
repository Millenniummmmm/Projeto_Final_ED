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
    printf("Tabela de Frequencias:\n");
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
    if(l->head == NULL || *(long long int *)base->frequencia < *(long long int *)l->head->frequencia){
         // Se caso a frequencia for menor que a frequencia do inicio da lista ou se a lista estiver vazia
         base->proximo = l->head; // Adiciona o nó no início da lista, se caso head apontar pra nulo agora o novo nó vai apontar pra nulo. Se caso apontar pra outro no, faz a troca de ponteiros.
         l->head = base; // Agora head vai apontar pra o novo nó adicionado
         l->tamanho++; // Incrementa o tamanho da lista
         return;
    }  
 
    Base *atual = l->head; // Inicializa um ponteiro auxiliar para percorrer a lista
    while(atual->proximo != NULL && *(long long int *)atual->proximo->frequencia <= *(long long int *)base->frequencia){
        atual = atual->proximo; // Percorre a lista até encontrar a posição correta
    }
    base->proximo = atual->proximo; // Insere o nó na posição correta
    atual->proximo = base; // Atualiza o ponteiro do nó anterior
    l->tamanho++; // Incrementa o tamanho da lista
}
// Vai preencher a lista encadeada com os nós criados a partir da tabela de frequências. Recebe a lista encadeada vazia e a tabela de frequências
// Alerta nessa função
void Preencher_Lista_Encadeada(Lista *l, unsigned int *tabela_frequencia) {
    for (int i = 0; i < TAMANHO_MAX; i++) {
        if (tabela_frequencia[i] > 0) { // Verifica se a frequência é maior que zero. As frequencias maiores que zero que importam

            Base *novo_nodo = malloc(sizeof(Base)); // Aloca memória para o novo nó

            if (!novo_nodo) { // Verifica se houve algum erro na alocação
                perror("Erro ao alocar memória para o nó da lista encadeada.\n");
                exit(EXIT_FAILURE); // Encerra o programa se a alocação falhar
            }
            // VERIFICAR SE VAI PRECISAR MANTER COM CASTING DESSA FORMA MAIS PRA FRENTE
            // Aloca memória para o símbolo
            unsigned char *simbolo = malloc(sizeof(unsigned char));
            *simbolo = (unsigned char)i;
            novo_nodo->dados = simbolo;

            // Aloca memória para a frequência 
            long long int *frequencia = malloc(sizeof(long long int));
            *frequencia = (long long int)tabela_frequencia[i];
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
        printf("Simbolo: %c, Frequencia: %lld\n", *(unsigned char *)atual->dados, *(long long int *)atual->frequencia);// Imprime o símbolo e a frequência
        atual = atual->proximo; // Avança para o próximo nó
    }
}


// Terceiro passo: Montar a árvore de Huffman ------------------------- //

/* 
A árvore de Huffman é uma árvore binária onde cada nó interno tem dois filhos e cada folha representa um símbolo.
A árvore é construída a partir da tabela de frequências, onde os símbolos mais frequentes estão mais próximos da raiz e os menos frequentes estão mais distantes.
*/

// Vai receber a lista e vai retornar o inicio da lista, o no com a menor frequencia.
Base* Pegar_Node_Inicial(Lista *l) {
    Base *temp = NULL; // Inicializa um ponteiro temporário para armazenar o nó a ser removido

    if (l->head != NULL) { // Verifica se a lista não está vazia
        temp = l->head; // Vai receber o nó que queremos pegar para remoção
        l->head = l->head->proximo; // O novo inicio vai ser o proximo node já que vamos remover esse nó
        l->tamanho--; // Decrementa o tamanho da lista, visto que removemos um nó
        temp->proximo = NULL; // O nó removido não deve apontar para nada, já que ele foi removido da lista
    }
    return temp; // Retorna o nó da cabeça da lista
}
// Recebe a lista e constrói a árvore de Huffman, retornando a raiz da árvore   
Base* Construir_Arvore_de_Huffman(Lista *no){
    Base *node1;
    Base *node2; 
    while(no->tamanho > 1) {
         node1 = Pegar_Node_Inicial(no); 
         node2 = Pegar_Node_Inicial(no); 

        // Cria um novo nó que vai ser a raiz da árvore
        Base *raiz_temporaria = malloc(sizeof(Base)); // Aloca memória para o novo nó

        if (!raiz_temporaria) { // Verifica se houve algum erro na alocação
            perror("Erro ao alocar memória para o nó da árvore de Huffman.\n");
            exit(EXIT_FAILURE); // Encerra o programa se a alocação falhar
        }
        // Aloca memória para a frequência e soma as frequências dos nós filhos
        long long int *frequencia = malloc(sizeof(long long int));
        *frequencia = *(long long int *)node1->frequencia + *(long long int *)node2->frequencia;
        //printf("Frequência: %lld\n", *frequencia); // Imprime a frequência do nó pai
        raiz_temporaria->frequencia = frequencia; // A frequência do novo nó é a soma das frequências dos nós filhos
        unsigned char *simbolo = malloc(sizeof(unsigned char));
        *simbolo = '*';
        raiz_temporaria->dados = simbolo;
        // Os filhos do novo nó são os nós que foram removidos da lista
        raiz_temporaria->esquerda = node1; 
        raiz_temporaria->direita = node2; 
        raiz_temporaria->proximo = NULL; // O próximo nó é NULL, já que ele vai ser a nova cabeça da lista

        Inserir_em_Ordem(no, raiz_temporaria); // Vai inserir o Nó pai na posição correta
    }
    Base *raiz_final = no->head; // A raiz da árvore é o único nó restante na lista encadeada
    no->head = NULL; // A lista encadeada agora está vazia
    no->tamanho = 0; // O tamanho da lista encadeada agora é zero
   return raiz_final;
}
// Função para testar a arvore
void imprimir_folhas_pre_ordem(Base *raiz, int nivel) {
    if (raiz == NULL) return;

    // Se for folha, imprime
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        printf("Simbolo: %c | Frequencia: %lld | Altura: %d\n", *(unsigned char *)raiz->dados, *(long long int *)raiz->frequencia, nivel);
    }
    else{
        // Percorre esquerda e direita (em ordem de pré-ordem)
        imprimir_folhas_pre_ordem(raiz->esquerda, nivel + 1);
        imprimir_folhas_pre_ordem(raiz->direita, nivel + 1);
    }  
}
// Passo 4: Gerar Dicionário de Códigos ------------------------- //
// A árvore de Huffman é percorrida em pré-ordem para gerar os códigos binários para cada símbolo.

// A função recebe a raiz da árvore e retorna a altura da mesma (Entender melhor essa função)
int Calcular_Altura_Arvore(Base *raiz){
    if (raiz == NULL) return 0; // Se a raiz for nula, retorna 0
    // Processo recursivo para verificar a altura da árvore
    int altura_esquerda = Calcular_Altura_Arvore(raiz->esquerda) + 1; // Soma 1 porque foi percorrida uma aresta pra fazer isso
    int altura_direita = Calcular_Altura_Arvore(raiz->direita) + 1; 
    // Verifica qual é a maior altura e retorna ela
    return (altura_esquerda > altura_direita) ? altura_esquerda : altura_direita;
}

// rever essa função depois (VER O PRIMEIRO CALLOC)
// recebe a altura da arvore somada mais 1 que resulta no numero total de colunas da matriz do dicionario. Vai retornar o dicionario inicializado antes de preenche-lo.
char** Definir_Dicionario(int colunas){
    char **dicionario = calloc(TAMANHO_MAX, sizeof(char*)); // Aloca memória para 256 ponteiros de char (um para cada símbolo ASCII possível)
    for (int i = 0; i < TAMANHO_MAX; i++) {
        dicionario[i] = calloc(colunas, sizeof(char)); // Aloca memória para a string do código binário do símbolo i (com espaço para colunas caracteres + '\0'
    }
    return dicionario; // Retorna o dicionário inicializado
}
// Essa função vai preencher o dicionario com os simbolos e os respectivos codigos binarios. Ela vai receber a raiz da arvore, o dicionario, o codigo a ser formado(caminho) e o numero de colunas
// A função retorna o dicionário preenchido. (Entender bem essa função depois)
void** Completar_Dicionario(Base* raiz, char** dicionario, char *codigo, int colunas){
    if (raiz == NULL) return; 

    // Se for folha, preenche o dicionário com o símbolo e o código binário
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        strcpy(dicionario[*(unsigned char *)raiz->dados], codigo); // Copia o código binário para uma linha do dicionário
    } else {
        // Percorre a árvore para gerar os códigos binários
        char codigo_esquerda[colunas]; // Array auxiliar para armazenar o código da esquerda
        char codigo_direita[colunas]; // Array auxiliar para armazenar o código da direita
        // Se andarmos pra esquerda, adicionamos '0' ao código, se andarmos pra direita, adicionamos '1'
        snprintf(codigo_esquerda, sizeof(codigo_esquerda), "%s0", codigo); // Adiciona '0' ao código da esquerda
        snprintf(codigo_direita, sizeof(codigo_direita), "%s1", codigo); // Adiciona '1' ao código da direita
        Completar_Dicionario(raiz->esquerda, dicionario, codigo_esquerda, colunas); // Chama recursivamente para a esquerda
        Completar_Dicionario(raiz->direita, dicionario, codigo_direita, colunas); // Chama recursivamente para a direita
    }
}

void imprime_dicionario(char** dicionario){
    printf("Dicionario:\n");
    for (int i = 0; i < TAMANHO_MAX; i++) {
        if (dicionario[i][0] != '\0') { // Verifica se o símbolo tem um código associado
            printf("Simbolo: %c, Codigo: %s\n", i, dicionario[i]); // Imprime o símbolo e o código binário
        }
    }
}

int main() {
    setlocale(LC_ALL, "Portuguese"); // Define a localidade para português
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
    imprimir_tabela(tabela_frequencia); // Imprime a tabela de frequências
    // --------------------- //

    // Teste do passo 2: Lista Encadeada
    Lista lista; // Declara a lista encadeada
    Criar_Lista_Encadeada(&lista); // Inicializa a lista encadeada
    Preencher_Lista_Encadeada(&lista, tabela_frequencia); // Preenche a lista encadeada com os nós criados a partir da tabela de frequências
    imprimir_lista(&lista); // Imprime a lista encadeada
    fclose(arquivo); // Fecha o arquivo
    // --------------------- //

    // Teste do passo 3: Montar a árvore de Huffman
    printf("Arvore de Huffman\n");
    Base *HuffTree = Construir_Arvore_de_Huffman(&lista); // Constrói a árvore de Huffman
    imprimir_folhas_pre_ordem(HuffTree, 0); // Imprime a árvore de Huffman em pré-ordem
    // --------------------- //

    // Teste do passo 4: Gerar Dicionario
    int altura = Calcular_Altura_Arvore(HuffTree); // Calcula a altura da árvore de Huffman
    char **dicionario = Definir_Dicionario(altura + 1); // Cria o dicionário de códigos binários
    Completar_Dicionario(HuffTree, dicionario, "", altura + 1); // Preenche o dicionário com os códigos binários
    imprime_dicionario(dicionario); // Imprime o dicionário de códigos binários


    return 0;
}
