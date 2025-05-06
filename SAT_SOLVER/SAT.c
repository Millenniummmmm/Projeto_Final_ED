#include "SAT.h"

// ---------- LEITURA DOS DADOS ----------
void ler_arquivo(const char *nome_arq, CNF *expressao){
    //printf("Tentando abrir: %s\n", nome_arq); //debug
    FILE *arquivo = fopen(nome_arq, "r"); //r porque é read
    
    if(!arquivo){
        perror("Erro ao abrir o arquivo no ler arquivo");
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
        cl.tamanho = 0;//começar na posiçao zero

        int num_lido; //armazenar numero lido
        char *percorrer_s = linha; //ponteiro pra percorrer a string
        while(sscanf(percorrer_s, "%d", &num_lido) == 1 && num_lido != 0){ //le um numero (negativo ou positivo) e se for diferente de 0, continua o loop
            cl.literais[cl.tamanho++] = num_lido; //adiciona o literal ao arrau de literais da struct e incrementa o tamanho
            
            while(*percorrer_s != ' ' && *percorrer_s != '\0')
                percorrer_s++; //avança o ponteiro pro proximo espaco ou fim de linha (pula o numero que foi lido) ja que o sscanf nao faz isso, temos que fazer manualmente
            //parou no espaço, então:
            while(*percorrer_s == ' ')
                percorrer_s++; //pula todos os espaços para ficar no inicio do proximo numero a ser lido
        }
        //adiciona a clausula com seus literais no vetor expressão
        expressao -> clausulas[indice++] = cl; //adiciona a clausula no vetor
    }

    expressao -> num_clausulas = indice; //corrige o número real de cláusulas (pode colocar errado na entrada)
    fclose(arquivo);
}

// ---------- VERIFICAR CLAUSULA ----------
//analisa se alguma atribuicao PARCIAL satisfaz a expressao
int verificar_cnf(CNF *cnf, int atribuicoes[]){ //ponteiro para conjuto de clausula e vetor com os valores atribuidos
    bool todas_satisfeitas = true;

    for(int i = 0; i < cnf -> num_clausulas; i++){ //for para navegar clausula por clausula
        Clausula *cl = &cnf -> clausulas[i]; //cria um ponteiro e pega o endereço da clausula 
        bool clausula_satisfeita = false;  //como ainda nao viu, coloca como falso
        bool indefinida = false; //mesma coisa de cima

        for(int j = 0; j < cl -> tamanho; j++){//loop pra percorrer os literais na clausula
            int literal = cl -> literais[j];
            int variavel = abs(literal); //numero da variavel, sempre positvo pq estamos vendo o indice
            //vetor[-4] => vetor[4]
            int valor = atribuicoes[variavel]; //valor atribuido: -1(falso), 0(nao atribuida) ou 1(vdd)
    
            if(valor == 0) //nao foi atribuuida (nao é falsa nem verdadeira)
                indefinida = true; //marca como indefinida
            else if((literal > 0 && valor == 1) || (literal < 0 && valor == -1)){//pelo menos um literal é verdadeiro
                //      a                1  = a        -a             -1 = a       
                clausula_satisfeita = true;
                //exemplo:
                //(a v -b)
                //a = 1 é verdadeiro
                break; //como a clausula é true, nao precisa olhar o resto dos literais ()
            }
        }
        
        if(!clausula_satisfeita && !indefinida) 
            return CONTRADICAO; //todos os literais deram falso
        if(!clausula_satisfeita && indefinida)
            todas_satisfeitas = false; //ainda nao deu certo, mas pode dar
            //exemplo:
            //(-a v c)
            //(0 v c) depende de c
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
   
// caso o retorno da função verificar_cnf seja INDEFINIDA, temos:    

    int prox_variavel = -10; //vai armazenar a proxima variavel nao atribuida
    for(int i = 1; i <= expressao -> num_variaveis; i++){ //verifica TODAS as variaveis possíveis 
        if(no -> atribuicoes[i] == 0){ //verifica se a variavel ainda nao foi atribuida
            prox_variavel = i; //o i ainda nao foi atribuido, entao armazena 
            break;  //sai dps que acha a 1° var nao atr
        }
    }

    if(prox_variavel == -10) //nao tem vaar dispoinivel e ainda indefinida
        return false;

    //tentativa pra atribuir 1 (v)
    struct Arvore *esq = malloc(sizeof(Arvore)); //cria um novo nó da a
    memcpy(esq -> atribuicoes, no -> atribuicoes, sizeof(int) * MAX_VAR); //copia as atribuicoes do nó ant para o esq

    esq -> atribuicoes[prox_variavel] = 1; //atribui 1 a proxima variavel
    esq -> variavel = prox_variavel; //guarada a variavel que foi atribuida
    esq -> valor = 1; //registra o valor nesse nó
    //exemplo:
    //(0 V -c)
    //(0 v 0)
    if(sat(esq, expressao, solucao)){ //chamada recursiva com nova atr
        //se retornar verdadeiro é pq tem uma solução possível com essa atr
        free(esq);
        return true; 
    }

    //tentativa pra atribuir -1 (f)
    struct Arvore *dir = malloc(sizeof(Arvore)); 
    memcpy(dir -> atribuicoes, no -> atribuicoes, sizeof(int) * MAX_VAR);
    dir -> atribuicoes[prox_variavel] = -1; //atribui -1 (f) na proxima variavel
    dir -> variavel = prox_variavel; //guarda a variavel
    dir -> valor = -1; //registra o valor
    //exemplo:
    //(0 v -c)
    //(0 v 1) da 1, então entra no if e retorna true
    if(sat(dir, expressao, solucao)){ //chamada recursiva pra direita
        free(dir);
        return true;
    }

    //se nenhuma tentativa funcionou, libera os dois filhos e retorna false
    free(esq);
    free(dir);
    return false;
}

void rodar(){
    CNF expressao = {0};     //zerando todos os itens
    struct Arvore raiz = {0};
    int solucao[MAX_VAR] = {0};
    char nome_arquivo[FILENAME_MAX];

    printf("\033[1;32m");
    printf("Digite o nome do arquivo a ser analisado: ");
    scanf(" %[^\n]", nome_arquivo);
    printf("\033[0m");
    char caminho_completo[FILENAME_MAX];
    snprintf(caminho_completo, FILENAME_MAX, "C:\\Users\\laris\\Desktop\\SAT_SOLVER\\Testes\\%s.cnf", nome_arquivo);

    FILE *arquivo = fopen(caminho_completo, "r");
    if(!arquivo){
        perror("Erro ao abrir o arquivo no sat solver");
        return;
    }
    ler_arquivo(caminho_completo, &expressao);

    for(int i = 0; i <= expressao.num_variaveis; i++) //deixa o array das atribuicoes zerado
        raiz.atribuicoes[i] = 0;

    if(sat(&raiz, &expressao, solucao)){ //chama a funcao recursiva
        printf("\033[1;32m");
        printf("\n");
        printf("\t\t\t\t\t========================\n");
        printf("\t\t\t\t\t||        SAT!        ||\n");
        printf("\t\t\t\t\t========================\n");
        printf("\n");
        printf("\033[0m");

        printf("\033[1;37m"); 
        printf("\t\t\t\t\t +-------------------+\n");
        printf("\t\t\t\t\t | Variável |  Valor |\n");
        printf("\t\t\t\t\t +-------------------+\n");

        for(int i = 1; i <= expressao.num_variaveis; i++){
            if(i < 10)
                printf("\t\t\t\t\t |     %d    |    %d   |\n", i, solucao[i] == 1 ? 1 : 0);
            else    
                printf("\t\t\t\t\t |     %d   |    %d   |\n", i, solucao[i] == 1 ? 1 : 0);
        }

        printf("\t\t\t\t\t +-------------------+\n");
        printf("\033[0m"); 
        printf("\n");
    }
    else{
        printf("\033[1;31m"); 
        printf("\n");
        printf("\t\t\t\t\t========================\n");
        printf("\t\t\t\t\t||       UNSAT!       ||\n");
        printf("\t\t\t\t\t========================\n");
        printf("\n");
        printf("\033[0m");
    }
}
