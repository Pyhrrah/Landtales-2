#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void detecterSalle(char map[121],char n, char * retour){
    char rangN = n*2-1;
    if (0 < n%11 && n%11 <= 5){
        retour[0] = map[rangN-1];
        retour[1] = map[rangN+1];
    } else {
        retour[0] = map[rangN-11];
        retour[1] = map[rangN+11];
    }
}

void creerMap(char * map){
    for(char k=0; k<11;k++){
        if(k%2==0){
            for(char i=0; i<11;i++) map[k*11+i] = i%2==0 ? (i/2)+1+(6*k/2) : -1;
        } else {
            for(char i=0; i<11;i++) map[k*11+i] = i%2==0 ? -1 : -2;
        }
    }
    char mursRestant = 35;
    srand( time( NULL ) );
    while( mursRestant > 0) {
        char mur = rand() % 60 +1;
        char salle1, salle2;
        char salles[2];
        detecterSalle(map, mur, salles);
        if (salles[0]!=salles[1]){
            mursRestant--;
            map[mur*2-1] = 0;
            for(char i=0;i<121;i++) {
                if (map[i]==salles[1]) map[i] = salles[0];
            }
        }
    }
}

char mapIndiceToNumeroSalle(char indice){
    indice++;
    char retour = indice%11 == 0 ? 11 : indice%11;
    return (retour/2+1)+6*(indice/22);
}

int main(){
    char map[121];
    creerMap(map);
    for(char k=0;k<11;k++){
        for(char i=0;i<11;i++){
            printf("%3d", map[k*11+i]);
        }
        printf("\n");
    }
    exit(EXIT_SUCCESS);
}