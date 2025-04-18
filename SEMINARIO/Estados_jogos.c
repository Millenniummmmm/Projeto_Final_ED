#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ESTADOS 100

typedef struct {
    char nome[50];
} Estado;

typedef struct {
    Estado estados[MAX_ESTADOS];
    int topo;
} PilhaEstados;

// Funções da pilha
void inicializar(PilhaEstados *pilha) {
    pilha->topo = -1;
}

int estaVazia(PilhaEstados *pilha) {
    return pilha->topo == -1;
}

int estaCheia(PilhaEstados *pilha) {
    return pilha->topo == MAX_ESTADOS - 1;
}

void empilhar(PilhaEstados *pilha, const char *nomeEstado) {
    if (estaCheia(pilha)) {
        printf("Erro: pilha cheia!\n");
        return;
    }
    pilha->topo++;
    strcpy(pilha->estados[pilha->topo].nome, nomeEstado);
    printf("Entrou no estado: %s\n", nomeEstado);
}

void desempilhar(PilhaEstados *pilha) {
    if (estaVazia(pilha)) {
        printf("Erro: pilha vazia!\n");
        return;
    }
    printf("Saindo do estado: %s\n", pilha->estados[pilha->topo].nome);
    pilha->topo--;
}

void estadoAtual(PilhaEstados *pilha) {
    if (estaVazia(pilha)) {
        printf("Sem estado atual\n");
        return;
    }
    printf("Estado atual: %s\n", pilha->estados[pilha->topo].nome);
}

// Simulação de jogo
int main() {
    PilhaEstados jogo;
    inicializar(&jogo);

    empilhar(&jogo, "JOGANDO");
    estadoAtual(&jogo);

    empilhar(&jogo, "MENU DE PAUSA");
    estadoAtual(&jogo);

    empilhar(&jogo, "CONFIGURACOES");
    estadoAtual(&jogo);

    // Agora o jogador aperta "voltar" duas vezes
    desempilhar(&jogo); // volta para MENU DE PAUSA
    estadoAtual(&jogo);

    desempilhar(&jogo); // volta para JOGANDO
    estadoAtual(&jogo);

    return 0;
}
