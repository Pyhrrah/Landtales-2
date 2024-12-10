#include <stdio.h>
#include <stdlib.h>

int main(){
    system("gcc -lm map.c");
    for(char i=1; i<=36;i++){
        char command2[7];
        sprintf(command2,"./a %d",i);
        printf("Voici la salle %d\n",i);
        system(command2);
    }
    exit(EXIT_SUCCESS);
}