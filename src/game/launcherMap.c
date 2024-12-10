#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]){
    system("gcc -lm map.c");
    for(char i=1; i<=36;i++){
        char command2[9];
        if (argc>1){
            sprintf(command2,"./a %d %s",i,argv[1]);
        } else {
            sprintf(command2,"./a %d",i);
        }
        printf("Voici la salle %d\n",i);
        system(command2);
    }
    printf("La génération de la map est un succès !\n");
    exit(EXIT_SUCCESS);
}