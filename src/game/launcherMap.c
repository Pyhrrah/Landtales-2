#include <stdio.h>
#include <stdlib.h>
#include "./map.h"

int main(int argc, char const *argv[]){

    if (argc>2 && argv[2] == '1') system("rm -fr ./save[1-3]");

    char etage = 0;
    if (argc>1) sscanf(argv[1],"%hhd",&etage);
    
    if (etage < 1 || etage > 3) {
        for(char i=1; i<=3; i++){
            supprimerEtage();
            creerEtageEntier(i);
            printf("Génération de l'étage %hhd : OK\n",i);
            printf("La seed est %d\n",lireSeed());
        }
    }
    
    printf("La génération de la map entière est un succès !\n");
    
    exit(EXIT_SUCCESS);
}