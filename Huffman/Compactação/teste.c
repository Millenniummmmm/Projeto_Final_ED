#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <locale.h>
#include <windows.h> 
#include <time.h> 

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

//----------------------------Utilidades----------------------------//

void Huffman() {
    printf("\n\n\n");
    printf("\033[1;34m");
    printf("\t██╗  ██╗██╗   ██╗███████╗███████╗███╗   ███╗ █████╗ ███╗   ██╗\n");
    printf("\t██║  ██║██║   ██║██╔════╝██╔════╝████╗ ████║██╔══██╗████╗  ██║\n");
    printf("\t███████║██║   ██║█████╗  █████╗  ██╔████╔██║███████║██╔██╗ ██║\n");
    printf("\t██╔══██║██║   ██║██╔══╝  ██╔══╝  ██║╚██╔╝██║██╔══██║██║╚██╗██║\n");
    printf("\t██║  ██║╚██████╔╝██║     ██║     ██║ ╚═╝ ██║██║  ██║██║ ╚████║\n");
    printf("\t╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═╝     ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝\n");
    printf("\n\n\n");
}

void Informacoes(){
    printf("\033[1;34m");
    printf("\t\t\t===================================\n");
    printf("\t\t\t\t    Informações\n");
    printf("\t\t\t===================================\n\n");
    printf("\t\t\tEsse programa foi desenvolvido por:\n\n");
    printf("\t\t\t\tLarissa Ferreira\n");
    printf("\t\t\t\tOtávio Joshua\n");
    printf("\t\t\t\tZilderlan Naty\n\n");
    printf("\t\t\tProfessor: Márcio Ribeiro\n\n");
    printf("\t\t\tUniversidade Federal de Alagoas\n\n");
    printf("\033[0m");
}

void Menu_Principal() {
    printf("\033[1;32m");
    printf("\t\t\t=============================\n");
    printf("\t\t\t\tMenu Principal\n");
    printf("\t\t\t=============================\n\n");
    printf("\t\t\tSelecione o que deseja fazer:\n");
    printf("\t\t\t1. Compactar arquivo\n");
    printf("\t\t\t2. Descompactar arquivo\n");
    printf("\t\t\t3. Sair\n");
    printf("\033[0m");
}

void Destruir_Arvore(Base* raiz) {
    if (raiz == NULL) return;
    Destruir_Arvore(raiz->esquerda); 
    Destruir_Arvore(raiz->direita); 
    free(raiz->dados); 
    free(raiz->frequencia);
    free(raiz); 
}

void Liberar_Dicionario(char **dicionario) {
    for (int i = 0; i < TAMANHO_MAX; i++) {
        free(dicionario[i]);
    }
    free(dicionario);
}

void Liberar_Lista(Lista *l) {
    Base *atual = l->head;
    while (atual != NULL) {
        Base *prox = atual->proximo;
        free(atual->dados);
        free(atual->frequencia);
        free(atual);
        atual = prox;
    }
    l->head = NULL;
    l->tamanho = 0;
}


// ------------------------- Compactação ------------------------- //

// Etapa 1 : Tabela de Frequências ------------------------- //

/*
    Nessa etapa, será criada uma tabela que armazena a frequência de cada símbolo presente no arquivo.
    Isso é fundamental para a codificação de Huffman, pois os símbolos mais frequentes devem ser representados com menos bits,
    enquanto os menos frequentes usam mais bits.

    Detalhe fundamental: fread(ponteiro, tamanho, número_de_blocos, arquivo)
    Enquanto for possível ler 1 byte do arquivo com sucesso, esse byte é armazenado em 'index' e sua frequência é incrementada.
    fread retorna o número de blocos lidos com sucesso. Aqui estamos lendo 1 bloco de 1 byte por vez, ou seja, byte a byte.

    Quando um byte é lido com sucesso, ele é armazenado na variável 'index', 
    já que estamos lendo o arquivo byte a byte (blocos de 1 byte de tamanho 1).
    O valor armazenado em 'index' (de 0 a 255) representa um símbolo específico.
    Esse valor é então usado como índice para acessar a tabela de frequências,
    e a contagem desse símbolo é incrementada.
    Se o símbolo aparecer novamente, o mesmo índice será acessado e sua frequência será aumentada mais uma vez.
    Assim, a tabela de frequências vai acumulando a quantidade de vezes que cada símbolo aparece no arquivo.
*/

// Inicializa a tabela de frequências com zero. A tabela vai ter 256 posições, uma para cada símbolo ASCII possível.

void Inicializar_Tabela(unsigned int *tabela){
    for (int i = 0; i < TAMANHO_MAX; i++) {
        tabela[i] = 0;
    }
}

// Vai receber o arquivo e a tabela inicializada com zero. Essa função vai ler o arquivo byte a byte e vai incrementar a frequência do símbolo lido na tabela de frequências.

void Definir_Tabela_Freq(FILE *arquivo, unsigned int *tab_frequencia) {
    unsigned char index = 0;
    while (fread(&index, 1, 1, arquivo) == 1) { // Lê o arquivo byte a byte. fread retorna 1 se um byte for lido com sucesso, ou seja, vai percorrer o arquivo inteiro até que seja 0.
        tab_frequencia[(unsigned char)index]++; // Incrementa a frequência do símbolo lido, preechendo a tabela
    }
}

// Etapa 2 : Listas Encadeadas Ordenadas ------------------------- //

/*
    O algoritmo de Huffman, para funcionar, precisa que suas frequencias estejam ordenadas de acordo com seu número de ocorrências. A lista encadeada ordenada em ordem crescente é 
    uma alternativa, visto que os simbolos menos frequentes estarão no começo da lista e os mais frequentes no final. Head apontará pro começo da lista, que nesse caso, seria o nó
    menos frequente. A lista encadeada vai ser usada para armazenar os nós da árvore de Huffman. Nessa etapa teremos funções fundamentais para manipular o funcionamento do árvore.

    HEAD -> 300 -> 346 -> 471 -> 612 -> NULL
*/

// Inicializa a lista

void Criar_Lista_Encadeada(Lista *l) {
    l->head = NULL; // Inicializa a cabeça da lista como NULL
    l->tamanho = 0; // Inicializa o tamanho da lista como 0  
}

// Vai inserir os nós da árvore na lista encadeada de forma ordenada(Ordem Crescente). Recebe o nó a ser inserido e a lista encadeada.
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
    while(atual->proximo != NULL && *(long long int *)atual->proximo->frequencia < *(long long int *)base->frequencia){
        atual = atual->proximo; // Percorre a lista até encontrar a posição correta
    }
    base->proximo = atual->proximo; // Insere o nó na posição correta (armazena)
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
            *simbolo = (unsigned char)i;  // Ex: *simbolo = (unsigned char)65;  ou *simbolo = 'A'; Acessa o símbolo ASCII correspondente
            novo_nodo->dados = simbolo; // Armazena o símbolo no nó (Armazena o ponteiro para o símbolo no campo 'dados' do nó)
            // Aloca memória para a frequência 
            long long int *frequencia = malloc(sizeof(long long int));
            *frequencia = (long long int)tabela_frequencia[i]; // Converte o valor (unsigned int) na posição i da tabela de frequências para long long int 
            novo_nodo->frequencia = frequencia; // Armazena o ponteiro da frequência convertida no campo 'frequencia' do nó
            // Inicializa os ponteiros para nulo pois não sabemos onde eles vão apontar ainda
            novo_nodo->proximo = NULL; 
            novo_nodo->direita = NULL; 
            novo_nodo->esquerda = NULL; 
            Inserir_em_Ordem(l, novo_nodo); // Vai garantir que a lista esteja ordenada.
        }
    }
}

// Vai receber a lista e vai retornar a cabeça da lista (nó com menor frequencia)

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

// Etapa 3 : Montar a árvore de Huffman ------------------------- //

/* 
    A árvore de Huffman é uma árvore binária onde cada nó interno tem dois filhos e cada folha representa um símbolo. É construída a partir
    da tabela de frequências, onde os símbolos mais frequentes estão mais próximos da raiz e os menos frequentes estão mais distantes.
*/

// Recebe a lista e constrói a árvore de Huffman, retornando a raiz da árvore   

Base* Construir_Arvore_de_Huffman(Lista *no){
    Base *node1;
    Base *node2; 
    while(no->tamanho > 1) {
         node1 = Pegar_Node_Inicial(no); 
         node2 = Pegar_Node_Inicial(no); 

        // Cria um novo nó que vai ser a raiz temporaria da árvore
        Base *raiz_temporaria = malloc(sizeof(Base)); // Aloca memória para o novo nó

        if (!raiz_temporaria) { // Verifica se houve algum erro na alocação
            perror("Erro ao alocar memória para o nó da árvore de Huffman.\n");
            exit(EXIT_FAILURE); // Encerra o programa se a alocação falhar
        }
        // Aloca memória para a frequência e soma as frequências dos nós filhos
        long long int *frequencia = malloc(sizeof(long long int));
        *frequencia = *(long long int *)node1->frequencia + *(long long int *)node2->frequencia;

        raiz_temporaria->frequencia = frequencia; // A frequência do novo nó é a soma das frequências dos nós filhos
        unsigned char *simbolo = malloc(sizeof(unsigned char));
        *simbolo = '*';
        raiz_temporaria->dados = simbolo;

        // Os filhos do novo nó são os nós que foram removidos da lista
        raiz_temporaria->esquerda = node1; 
        raiz_temporaria->direita = node2; 
        raiz_temporaria->proximo = NULL; // O próximo nó é NULL, pois ainda vai ser inserido na lista

        Inserir_em_Ordem(no, raiz_temporaria); // Vai inserir o Nó pai na posição correta
        
    }
    Base *raiz_final = no->head; // A raiz da árvore é o único nó restante na lista encadeada
    no->head = NULL; // A lista encadeada agora está vazia
    no->tamanho = 0; // O tamanho da lista encadeada agora é zero
   return raiz_final;
}

// Etapa 4 : Gerar Dicionário de Códigos ------------------------- //

/*
    A árvore de Huffman é percorrida em pré-ordem para gerar os códigos binários para cada símbolo. Os símbolos mais frequentes terão códigos mais curtos,
    os simbolos menos frequentes códigos mais longos. A função Completar_Dicionario percorre a árvore e preenche o dicionário com os códigos binários correspondentes.
    O dicionário é nada mais nada menos do que uma matriz, onde o total de colunas da matriz é a altura da arvore + 1(\0) e as linhas são os 256 símbolos possíveis.
*/

// A função recebe a raiz da árvore e retorna a altura da mesma 

int Calcular_Altura_Arvore(Base *raiz){
    if (raiz == NULL) return 0; // Se a raiz for nula, retorna 0
    // Processo recursivo para verificar a altura da árvore. Primeiro verifica a da esquerda, depois a da direita.
    int altura_esquerda = Calcular_Altura_Arvore(raiz->esquerda) + 1; // Soma 1 porque foi percorrida uma aresta pra fazer isso
    int altura_direita = Calcular_Altura_Arvore(raiz->direita) + 1; 
    // Verifica qual é a maior altura e retorna ela
    return (altura_esquerda > altura_direita) ? altura_esquerda : altura_direita;
}

// Recebe a altura da arvore somada mais 1 que resulta no numero total de colunas da matriz do dicionario. Vai retornar o dicionario inicializado antes de preenche-lo.

/*
    DETALHE: Uma string em C é um char *, ou seja, um ponteiro para caractere.
    Como teremos 256 strings, precisamos de 256 char *.
    Como temos 256 char *, precisamos acessar cada um deles, e fazemos isso com um ponteiro duplo — o char **.
    Ele começa apontando para o primeiro char *, e assim conseguimos acessar todos usando índice, como se fosse uma matriz.
*/

char** Definir_Dicionario(int colunas){
    char **dicionario = calloc(TAMANHO_MAX, sizeof(char*)); // Aloca memória para 256 ponteiros de char (um para cada símbolo ASCII possível)
    for (int i = 0; i < TAMANHO_MAX; i++) {
        dicionario[i] = calloc(colunas, sizeof(char)); // Aloca memória para a string do código binário do símbolo i (com espaço para colunas caracteres + '\0')
    }
    return dicionario; // Retorna o dicionário inicializado
}

// Essa função vai preencher o dicionario com os simbolos e os respectivos codigos binarios. Ela vai receber a raiz da arvore, o dicionario, o codigo a ser formado(caminho) e o numero de colunas

void Completar_Dicionario(Base* raiz, char** dicionario, char *codigo, int colunas){
    if (raiz == NULL) return; 

    // Se for folha, preenche o dicionário com o símbolo e o código binário
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        // Acessa o símbolo armazenado no nó (convertendo de void * para unsigned char), e usa esse valor como índice no dicionário para copiar o código binário correspondente.
        strcpy(dicionario[*(unsigned char *)raiz->dados], codigo); 
        //strcpy(destino, origem)
    } else {
        // Percorre a árvore para gerar os códigos binários
        char codigo_esquerda[colunas]; // Array auxiliar para armazenar o código da esquerda
        char codigo_direita[colunas]; // Array auxiliar para armazenar o código da direita
        // Os arrays começam inicialmente com lixo mas o snprintf sobrescreve o lixo com o codigo atual e adiciona um fim de string, protegendo contra acessos indevidos
        // Se andarmos pra esquerda, adicionamos '0' ao código, se andarmos pra direita, adicionamos '1'
        //snprintf(destino, tamanho, formato, argumentos);
        snprintf(codigo_esquerda, sizeof(codigo_esquerda), "%s0", codigo); // Adiciona '0' ao código da esquerda
        snprintf(codigo_direita, sizeof(codigo_direita), "%s1", codigo); // Adiciona '1' ao código da direita
        Completar_Dicionario(raiz->esquerda, dicionario, codigo_esquerda, colunas); // Chama recursivamente para a esquerda
        Completar_Dicionario(raiz->direita, dicionario, codigo_direita, colunas); // Chama recursivamente para a direita
    }
}

// Etapa 5 : Funções para o cabeçalho ----------------------------- //

/*
    Aqui teremos as funções auxiliares para a geração do cabeçalho .huff;
*/

// Recebe o tamanho do codigo binario produzido pela codificacao e retorna o tamanho do lixo em decimal que vai ser adicionado no cabeçalho

int Calcular_Lixo(long long tamanho_bits) {
    return (8 - (tamanho_bits % 8)) % 8;
}

// Verifica se é um caracter de escape. Retorna true se for, false se não for.

bool Caracter_Especial(unsigned char c) {
    if(c == '*' || c == '\\') // Verifica se o caractere é especial
        return true; // Se for, retorna verdadeiro
    else 
        return false; // Se não for, retorna falso    
}

// Vai receber a raiz da arvore, o vetor saída e a posição inicial dele. Essa função vai escrever a árvore em pré-ordem no vetor de saída.

void Escrever_Pre_Ordem(Base* raiz, char* buffer, int* posicao) {
    if (raiz == NULL) return;

    // Folha
    if (!raiz->esquerda && !raiz->direita) {
        unsigned char dado = *(unsigned char*)raiz->dados;

        // Escreve caractere com escape, se necessário
        if (Caracter_Especial(dado)) {
            buffer[(*posicao)++] = '\\';
        }

        buffer[(*posicao)++] = dado;
        return;
    }

    // Nó interno
    buffer[(*posicao)++] = '*';
    // Pré-ordem: escreve o nó atual, depois a subárvore esquerda e depois a subárvore direita
    Escrever_Pre_Ordem(raiz->esquerda, buffer, posicao);
    Escrever_Pre_Ordem(raiz->direita, buffer, posicao);
}

// Receve a raiz e retorna o tamanho total da árvore recursivamente. Caracter de escape conta como dois por causa do \\  e do \*

int Calcular_Tamanho_Arvore(Base* raiz) {
    if (raiz == NULL) return 0;

    // Se for folha
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        unsigned char c = *(unsigned char*)raiz->dados;
        if (c == '*' || c == '\\') {
            return 2; // Usa escape (\*) ou (\\), necessita de 2 espaços pra ser representado
        } else {
            return 1; // Só o caractere normal
        }
    }
    // Para os nós não-folha, conta 1 para o nó e soma os tamanhos das subárvores
    return 1 + Calcular_Tamanho_Arvore(raiz->esquerda) + Calcular_Tamanho_Arvore(raiz->direita);
}

// Recebe a raiz da árvore, aloca espaço pra string e dps chama a função para escrever a árvore em pré-ordem. Retorna a string com a árvore serializada.

char* Arvore_Pre_Ordem(Base* raiz) {
    int tamanho = Calcular_Tamanho_Arvore(raiz);
    char *saida = calloc(tamanho + 1, sizeof(char)); // +1 só por segurança pro '\0'
    if (!saida) {
        perror("Erro ao alocar memória para árvore serializada");
        exit(EXIT_FAILURE);
    }

    int pos = 0;
    Escrever_Pre_Ordem(raiz, saida, &pos);
    saida[pos] = '\0'; // Adiciona o terminador de string
    
    return saida;
}

// Etapa 6 : Codificação e Compactação ------------------------- //

/*
     Após a construção da árvore de Huffman e geração do dicionário de códigos binários para cada símbolo,
    esta etapa realiza a escrita do arquivo compactado propriamente dito.

    O processo acontece em três grandes blocos:

    1. Escrita inicial do cabeçalho:
       - Reserva espaço para os 2 primeiros bytes (lixo + tamanho da árvore) com '\0' temporariamente
       - Em seguida, escreve a árvore serializada em pré-ordem (com escape quando necessário)

    2. Escrita dos bits codificados:
       - Percorre cada símbolo do arquivo original
       - Usa o dicionário para obter o código binário correspondente
       - Monta bytes completos bit a bit (acumulando de 8 em 8)
       - Quando um byte está cheio, escreve no arquivo
       - No final, se sobrarem bits incompletos, escreve o último byte e calcula o "lixo" (bits não usados)

    3. Escrita final do cabeçalho:
       - Agora com o valor real do lixo e o tamanho da árvore
       - Gera o valor de 16 bits (3 bits de lixo + 13 bits do tamanho da árvore)
       - Volta ao início do arquivo com `fseek` e sobrescreve os dois primeiros bytes

    Resultado final: o arquivo "compactado.huff" contém o cabeçalho seguido da sequência de bits comprimidos,
    e pode ser descompactado corretamente usando a mesma árvore de Huffman.
*/

void Compactar_Arquivo(unsigned char *dados, long long tamanhoArquivo, char **dicionario, Base *HuffTree) {
    FILE *arquivo = fopen("C:\\Huffman\\compactado.huff", "wb");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    // Cabeçalho
    int tam_arvore = Calcular_Tamanho_Arvore(HuffTree);
    char *arvore_serializada = Arvore_Pre_Ordem(HuffTree);

    // Reservamos espaço para o cabeçalho
    fwrite("\0", 1, 1, arquivo);
    fwrite("\0", 1, 1, arquivo);
    fwrite(arvore_serializada, 1, tam_arvore, arquivo);

    // Escrita dos bits
    unsigned char byte = 0;
    int pos = 7;
    long long total_bits = 0;

    for (long long i = 0; i < tamanhoArquivo; i++) {
        char *codigo = dicionario[dados[i]];
        for (int j = 0; codigo[j] != '\0'; j++) {
            if (codigo[j] == '1') {
                byte |= (1 << pos);
            }
            pos--;
            total_bits++;

            if (pos < 0) {
                fwrite(&byte, 1, 1, arquivo);
                byte = 0;
                pos = 7;
            }
        }
    }

    int lixo = 0;
    if (pos != 7) {
        fwrite(&byte, 1, 1, arquivo);
        lixo = pos + 1;
    }

    // Reescreve o cabeçalho agora com lixo e tamanho da árvore
    unsigned short cabecalho = (lixo << 13) | tam_arvore;
    unsigned char byte1 = cabecalho >> 8;
    unsigned char byte2 = cabecalho % 256; 

    fseek(arquivo, 0, SEEK_SET);
    fwrite(&byte1, 1, 1, arquivo);
    fwrite(&byte2, 1, 1, arquivo);
    fclose(arquivo);
    free(arvore_serializada);

    printf("Compactação concluída com sucesso!\n");
    printf("Tamanho da árvore: %d\n", tam_arvore);
    printf("Bits lixo no final: %d\n", lixo);
    printf("Bits totais gravados: %I64d\n", total_bits);
}

/*
[============================(Função principal para a Compressão de Dados)============================]
*/

void Comprimir_Dados() {

    // Abertura do arquivo

    char nome_arquivo[FILENAME_MAX];
    printf("\033[1;32m");
    printf("Digite o nome do arquivo a ser compactado: ");
    scanf(" %[^\n]", nome_arquivo);
    printf("\033[0m");
    char caminho_completo[FILENAME_MAX];
    snprintf(caminho_completo, FILENAME_MAX, "C:\\Huffman\\%s", nome_arquivo);

    FILE *arquivo = fopen(caminho_completo, "rb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    // Passo 1: Frequências

    unsigned int tabela_frequencia[TAMANHO_MAX];
    Inicializar_Tabela(tabela_frequencia);
    Definir_Tabela_Freq(arquivo, tabela_frequencia);
    rewind(arquivo);

    // Passo 2: Lista Encadeada

    Lista lista;
    Criar_Lista_Encadeada(&lista);
    Preencher_Lista_Encadeada(&lista, tabela_frequencia);

    // Passo 3: Árvore de Huffman

    Base *HuffTree = Construir_Arvore_de_Huffman(&lista);

    // Passo 4: Dicionário

    int altura = Calcular_Altura_Arvore(HuffTree);
    char **dicionario = Definir_Dicionario(altura + 1);
    Completar_Dicionario(HuffTree, dicionario, "", altura + 1);

    // Passo 5: Leitura dos dados

    fseek(arquivo, 0, SEEK_END);
    long long tamanhoArquivo = ftell(arquivo);
    rewind(arquivo);
    unsigned char *dados = malloc(tamanhoArquivo);
    fread(dados, 1, tamanhoArquivo, arquivo);
    fclose(arquivo);

    // Passo 6: Codificação e Compactação
    
    Compactar_Arquivo(dados, tamanhoArquivo, dicionario, HuffTree);

    // Libera a memória alocada

    free(dados);
    Destruir_Arvore(HuffTree); // Libera a árvore de Huffman
    Liberar_Dicionario(dicionario); // Libera o dicionário
    Liberar_Lista(&lista); // Libera a lista encadeada
}

int main() {

    system("chcp 65001 > nul"); 

    setlocale(LC_ALL, "pt_BR.UTF-8");
    SetConsoleOutputCP(65001); // Configura o console para UTF-8

    Informacoes(); // Chama a função de informações
    Sleep(3000);
    system("cls"); // Limpa a tela do consol

    Huffman();

    while(1) {

    Menu_Principal(); 

    int escolha;

    scanf("%d", &escolha);

    switch (escolha) {
        case 1:
            Comprimir_Dados(); // Chama a função de compressão
            break;
        case 2:
            //Descomprimir_Dados(); // Chama a função de descompressão (não implementada)
            printf("Descompactar arquivo ainda não implementado.\n");
            break;
        case 3:
            printf("\033[31;31m");
            printf("Saindo...\n");
            exit(0);
        default:
            printf("Opção inválida.\n");
            break;
    }
    
}
    return 0;
}
