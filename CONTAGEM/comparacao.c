//lista encadeada versus arvore binaria de busca

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX 500 //numero de elementos a serem inseridos em cada ed
#define EXECUCOES 200 //numero de execuçoes do teste

// ---------- LISTA ENCADEADA ----------

typedef struct Lista {
    int valor;
    struct Lista* prox;
} Lista;

// função para inserir no início da lista 
Lista* inserirLista(Lista* lista, int valor) {
    Lista* novo = (Lista*)malloc(sizeof(Lista));
    if (novo == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    novo->valor = valor;
    novo->prox = lista;
    return novo;
}

bool busca_lista(Lista* lista, int numero, int* comp_L){
    *comp_L = 0; //inicializa o contador de comparaçoes da lista
    while(lista != NULL){
        (*comp_L)++;
        if(lista -> valor == numero)
            return true; //encontrou
        lista = lista -> prox;
    }
    return false; //nao encontrou
}

/*void exibirLista(Lista* lista){
    printf("\tLista: ");
    while(lista != NULL){
        printf("%d ", lista -> valor);
        lista = lista -> prox;
    }
    printf("\n");
}*/

// ---------- ÁRVORE DE BUSCA BINÁRIA ----------

typedef struct Arvore{
    int valor;
    struct Arvore* esquerda;
    struct Arvore* direita;
}Arvore;

Arvore* inserirArvore(Arvore* raiz, int valor){
    if(raiz == NULL){
        Arvore* novo = (Arvore*)malloc(sizeof(Arvore));
        novo -> valor = valor;
        novo -> esquerda = novo -> direita = NULL;
        return novo;
    }

    if(valor < raiz->valor)
        raiz -> esquerda = inserirArvore(raiz->esquerda, valor);
    else
        raiz -> direita = inserirArvore(raiz->direita, valor);

    return raiz;
}

bool busca_arvore(Arvore* raiz, int numero, int* comp_A){
    *comp_A = 0; //inicializa o contador de comparacao de arvore
    while(raiz != NULL){
        (*comp_A)++;
        if(raiz -> valor == numero)
            return true; //encontrou
     
        if(numero < raiz -> valor)
            raiz = raiz -> esquerda;
        else
            raiz = raiz -> direita;
    }
    return false; //nao encontrou
}

/*void exibirEmOrdem(Arvore* raiz){
    if(raiz != NULL){
        exibirEmOrdem(raiz -> esquerda);
        printf("%d ", raiz -> valor);
        exibirEmOrdem(raiz -> direita);
    }
}*/

// ---------- FUNÇÃO PRINCIPAL ----------

int main(){
    Lista* lista = NULL;
    Arvore* arvore = NULL;

    srand(time(NULL)); //inicializa o gerador de numeros aleatorios

    FILE* arquivo = fopen("C:\\Users\\laris\\Desktop\\CONTAGEM\\dados_comparacao.csv", "w");

    if(arquivo == NULL){
        printf("Erro ao abrir arquivo pra escrita!\n");
        return 1;
    }
    fprintf(arquivo, "Procurado,Comparacoes_ABB,Comparacoes_Lista\n");

    //loop de execucoes de teste
    //printf("\tInserindo %d elementos aleatorios %d vezes nas duas estruturas:\n", MAX, EXECUCOES);
    for(int i = 0; i < EXECUCOES; i++){
        for(int j = 0; j < MAX; j++){
            int num = rand() % 500; //gera numero aleatorio entre 0 e 499

            lista = inserirLista(lista, num);
            arvore = inserirArvore(arvore, num);
        }

        int num_proc = rand() % 500; //gera o numero aleatorio a ser procurado
        int comp_A = 0, comp_L = 0; 

        //int encontrou_A = 
        busca_arvore(arvore, num_proc, &comp_A);
        //int encontrou_L = 
        busca_lista(lista, num_proc, &comp_L);
/*
        printf("\tNumero sorteado: %d\n", num_proc);
        printf("\tNumero de comparacoes para achar na arvore de busca binaria: %d\n", comp_A);
        printf("\tEncontrado? %s\n", encontrou_A ? "Sim" : "Nao");
        printf("\tNumero de comparacoes para achar na lista encadeada: %d\n", comp_L);
        printf("\tEncontrado? %s\n", encontrou_L ? "Sim" : "Nao");
*/


        fprintf(arquivo, "%d,%d,%d\n", num_proc, comp_A, comp_L);

        /*printf("\t\t\n--- Exibindo Estruturas ---\n");
        exibirLista(lista);

        printf("\tArvore: ");
        exibirEmOrdem(arvore);
        printf("\n");*/
    }

    fclose(arquivo);
    return 0;
}
