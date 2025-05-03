#include "Descompactar.h"
#include "Compactar.h"


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


int main() {

    system("chcp 65001 > nul"); 

    setlocale(LC_ALL, "pt_BR.UTF-8");
    SetConsoleOutputCP(65001); 

    Informacoes(); 
    Sleep(3000);
    system("cls"); 

    Huffman();

    while(1) {

    Menu_Principal(); 

    int escolha;

    scanf("%d", &escolha);

    switch (escolha) {
        case 1:
            Comprimir_Dados(); 
            break;
        case 2:
            Descomprimir_Dados();
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