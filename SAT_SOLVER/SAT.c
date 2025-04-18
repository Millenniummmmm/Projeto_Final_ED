#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <locale.h>

#define MAX_VAR 2000
#define MAX_CLAUSULAS 2000
#define MAX_LITERAIS 200

#define SATISFEITA 0
#define CONTRADICAO 1
#define INDEFINIDA 2

typedef struct{
    int literais[MAX_LITERAIS]; //literais da clausula
    int tamanho; //quantos literais essa clausula tem
}Clausula;

typedef struct{
    Clausula clausulas[MAX_CLAUSULAS]; //lista de clausulas
    
    int num_clausulas;  //quantas clausulas existem
    int num_variaveis;  //quantas variaveis (literais) existem
}CNF;

typedef struct Arvore{
    int variavel; //variavel escolhida no nó
    int valor;  //1 ou -1
    int atribuicoes[MAX_VAR]; //cópia das atribuicoes feitas
    
    struct Arvore *esq; //vdd (1)
    struct Arvore *dir; //f (-1)
}Arvore;

// ---------- LEITURA DOS DADOS ----------
void ler_arquivo(const char *nome_arq, CNF *expressao){
    //printf("Tentando abrir: %s\n", nome_arq); //debug
    FILE *arquivo = fopen(nome_arq, "r"); //r porque é read
    
    if(!arquivo){
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    char linha[300]; //array pra ver em qual linha ta
    int indice = 0; //indice pra preencher as clausulas lidas

    while(fgets(linha, sizeof(linha), arquivo)){ //le o arquivo linha por linha e guarda na string linha
        if(linha[0] == 'c') //pula os comentarios
            continue; 

        if(linha[0] == 'p'){ //só precisamos verificar o indice 0
            sscanf(linha, "p cnf %d %d", &expressao -> num_variaveis, &expressao -> num_clausulas); //le apenas os numeros
            continue;
        }

        Clausula cl;
        cl.tamanho = 0;

        int num_lido; //armazenar numero lido
        char *percorrer_s = linha; //ponteiro pra percorrer a string
        while(sscanf(percorrer_s, "%d", &num_lido) == 1 && num_lido != 0){ //le um numero (negativo ou positivo) e se for diferente de 0, continua o loop
            cl.literais[cl.tamanho++] = num_lido; //adiciona o literal ao arrau e incrementa o tamanho
            
            while(*percorrer_s != ' ' && *percorrer_s != '\0')
                percorrer_s++; //avança o ponteiro pro proximo espaco ou fim de linha (pula o numero que foi lido)
            
            while(*percorrer_s == ' ')
                percorrer_s++; //pula todos os espaços para ficar no inicio do proximo numero a ser lido
        }
        expressao -> clausulas[indice++] = cl; //adiciona a clausula no vetor
    }

    expressao -> num_clausulas = indice; //corrige o número real de cláusulas (pode colocar errado na entrada)
    fclose(arquivo);
}

// ---------- VERIFICAR CLAUSULA ----------
int verificar_cnf(CNF *cnf, int atribuicoes[]){ //analisa se alguma atribuicao parcial satisfaz a expressao
    bool todas_satisfeitas = true;

    for(int i = 0; i < cnf -> num_clausulas; i++){
        Clausula *cl = &cnf -> clausulas[i];
        bool clausula_satisfeita = false;
        bool indefinida = false;

        for(int j = 0; j < cl -> tamanho; j++){//loop pra percorrer os literais na clausula
            int literal = cl -> literais[j];
            int variavel = abs(literal); //numero da variavel, sempre positvo
            int valor = atribuicoes[variavel]; //valor atribuido: -1(falso), 0(nao atribuida) ou 1(vdd)
    
            if(valor == 0) //nao foi atribuuida
                indefinida = true; //marca como indefinida
            else if((literal > 0 && valor == 1) || (literal < 0 && valor == -1)){//pelo menos um literal é verdadeiro
                clausula_satisfeita = true;
                break; //como a clausula é true, nao precisa olhar o resto dos literais
            }
        }
        
        if(!clausula_satisfeita && !indefinida) 
            return CONTRADICAO; //todos os literais deram falso
        if(!clausula_satisfeita && indefinida)
            todas_satisfeitas = false; //ainda nao deu certo, mas pode dar
    }
    return todas_satisfeitas ? SATISFEITA : INDEFINIDA;
} 

// ---------- ANALISA SE É SAT ----------
bool sat(Arvore *no, CNF *expressao, int solucao[]){
    int estado = verificar_cnf(expressao, no -> atribuicoes);

    if(estado == SATISFEITA){
        memcpy(solucao, no -> atribuicoes, sizeof(int) *MAX_VAR); //copia as atribuicoes para solucao
        return true; //se for satisfeita, ja pode retornar
    }
    if(estado == CONTRADICAO)
        return false; //se for contradicao, ja pode retornar
    
    int prox_variavel = -10; //armazena a proxima variavel nao atribuida
    for(int i = 1; i <= expressao -> num_variaveis; i++){
        if(no -> atribuicoes[i] == 0){ //verifica se a variavel ainda nao foi atribuida
            prox_variavel = i; //o i ainda nao foi atribuido, entao armazena 
            break;  //sai dps que acha a 1° var nao atr
        }
    }

    if(prox_variavel == -10) //nao tem vaar dispoinivel e ainda indefinida
        return false;

    //tentativa pra atribuir 1
    struct Arvore *esq = malloc(sizeof(Arvore)); //cria um novo nó da a
    memcpy(esq -> atribuicoes, no -> atribuicoes, sizeof(int) * MAX_VAR); //copia as atribuicoes do nó ant para o esq
    esq -> atribuicoes[prox_variavel] = 1; //atribui 1 a proxima variavel
    esq -> variavel = prox_variavel; //guarada a variavel que foi atribuida
    esq -> valor = 1; //registra o valor nesse nó

    if(sat(esq, expressao, solucao)){ //chamada recursiva com nova atr
        free(esq);
        return true; 
    }

    //tentativa pra atribuir -1
    struct Arvore *dir = malloc(sizeof(Arvore)); 
    memcpy(dir -> atribuicoes, no -> atribuicoes, sizeof(int) * MAX_VAR);
    dir -> atribuicoes[prox_variavel] = -1; //atribui -1 (f) na proxima variavel
    dir -> variavel = prox_variavel; //guarda a variavel
    dir -> valor = -1; //registra o valor

    if(sat(dir, expressao, solucao)){ //chamada recursiva pra direita
        free(dir);
        return true;
    }

    //se nenhuma tentativa funcionou, libera os dois filhos e retorna false
    free(esq);
    free(dir);
    return false;
} 

// ---------- FUNCAO PRINCIPAL ----------
int main(){
    //zerando todos os itens
    setlocale(LC_ALL, "Portuguese");

    CNF expressao = {0}; 
    struct Arvore raiz = {0};
    int solucao[MAX_VAR] = {0};

    ler_arquivo("C:\\Users\\laris\\Desktop\\SAT_SOLVER\\entrada.txt", &expressao);

    for(int i = 0; i <= expressao.num_variaveis; i++) //deixa o array das atribuicoes zerado
        raiz.atribuicoes[i] = 0;

    if(sat(&raiz, &expressao, solucao)){ //chama a funcao recursiva
        printf("\033[1;32m");
        printf("\n");
        printf("\t========================\n");
        printf("\t||        SAT!        ||\n");
        printf("\t========================\n");
        printf("\n");
        printf("\033[0m");

        printf("\033[1;34m"); 
        printf("\t +-------------------+\n");
        printf("\t | Variavel |  Valor |\n");
        printf("\t +-------------------+\n");

        for(int i = 1; i <= expressao.num_variaveis; i++)
            printf("\t |     %d    |    %d   |\n", i, solucao[i] == 1 ? 1 : 0);

        printf("\t +-------------------+\n");
        printf("\033[0m"); 
        printf("\n");
    }
    else{
        printf("\033[1;31m"); 
        printf("\n");
        printf("\t========================\n");
        printf("\t||       UNSAT!       ||\n");
        printf("\t========================\n");
        printf("\n");
        printf("\033[0m");
    }
    return 0;
}
