#include "SAT.h"

int main(){
    system("chcp 65001 > nul");
    setlocale(LC_ALL, "pt_BR.UTF-8");
    
    int escolha;

    printf("\033[1;34m");
    printf("\n\n\n");
    printf("\t   ███████╗ █████╗ ████████╗    ███████╗ ██████╗ ██╗     ██╗   ██╗███████╗██████╗ \n");
    printf("\t   ██╔════╝██╔══██╗╚══██╔══╝    ██╔════╝██╔═══██╗██║     ██║   ██║██╔════╝██╔══██╗\n");
    printf("\t   ███████╗███████║   ██║       ███████╗██║   ██║██║     ██║   ██║█████╗  ██████╔╝\n");
    printf("\t   ╚════██║██╔══██║   ██║       ╚════██║██║   ██║██║     ██║   ██║██╔══╝  ██╔══██╗\n");
    printf("\t   ███████║██║  ██║   ██║       ███████║╚██████╔╝███████╗╚██████╔╝███████╗██║  ██║\n");
    printf("\t   ╚══════╝╚═╝  ╚═╝   ╚═╝       ╚══════╝ ╚═════╝ ╚══════╝ ╚═════╝ ╚══════╝╚═╝  ╚═╝\n");
    printf("\033[0m");

    while(1){
        printf("\033[1;32m");
        printf("\t\t\t=============================\n");
        printf("\t\t\t\tMenu Principal\n");
        printf("\t\t\t=============================\n\n");
        printf("\t\t\tSelecione o que deseja fazer:\n");
        printf("\t\t\t1. Sat Solver\n");
        printf("\t\t\t2. Sair\n");
        printf("\033[0m");

        scanf("%d", &escolha);
        switch(escolha){
            case 1:
                rodar();
                Sleep(5000);
                break;
            case 2:
                printf("\033[1;32m");
                printf("Obrigado por usar o programa!! :)");
                printf("\033[31;31m");
                printf("Saindo...\n");
                Sleep(3000);
                exit(0);
        }
    }
    return 0;
}