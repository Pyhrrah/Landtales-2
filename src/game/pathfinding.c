//FICHIER JAMAIS APPELÉ DANS LE CODE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define save ""

//**********************************//
// FICHIER DE TEST -> NON CONCLUANT //
//**********************************//

//réutilisation de fonction dans map.c sans les importer pour faciliter la versitilité des tests
//il faut qu'il y ait un fichier /map/Salle01/salle01.txt pour que les tests "fonctionnent", donc un fichier d'une vrai salle renommé en salle01.txt


char lireSalle(char numSalle, short sallePosX[315], short sallePosY[315], char salle[315]) {
    /*lit la salle donnée dans le fichier correspondant et la transforme en liste à une dimension pour l'exploiter*/
    char * nomFichierCheminTemp = "/map/Salle00/salle00.txt";
    char numSalleSTR[5];
    sprintf(numSalleSTR,"%02hhd",numSalle); 
    char nomFichier[strlen(save)+strlen(nomFichierCheminTemp)+1];
    strcpy(nomFichier, save);
    strcat(strcat(strcat(strcat(strcat(nomFichier,"/map/Salle"),numSalleSTR),"/salle"),numSalleSTR),".txt");
    FILE * fichier = NULL;
    if((fichier = fopen(nomFichier,"r"))==NULL) {
        printf("Erreur lors de la lecture de la salle\n");
        return 0;
    } else {
        for (short i = 0; i<315; i++) fscanf(fichier," %hd %hd %hhd",&sallePosX[i], &sallePosY[i], &salle[i]);
        fclose(fichier);
        fichier = NULL;
        return 1;
    }
}

//TENTATIVE D'ADDAPTATION DE L'ALGORITHME A* -> ECHEC (pour la reconstruction + seg fault pour un tableau)

typedef struct LL LL;

struct LL {
    short indice;    
    short value;
    char heuristique;
    char cout;
    LL * N;
    LL * E;
    LL * S;
    LL * W;
};

LL initVoidLL(){
    LL retour = {0,-1,-1,0,NULL,NULL,NULL,NULL};
    return retour;
}

LL initWithValueLL(short indice,short value, char cout){
    if (!(value >=1 && value <= 4)) cout = -1;
    LL retour = {indice,value,-1,cout,NULL,NULL,NULL,NULL};
    return retour;
}

void liaisonLL(LL noeud[315]){
    short throw[315];
    char salle[315];
    lireSalle(1,throw,throw,salle);
    for(int i = 0; i<315; i++){
        noeud[i] = initWithValueLL((short)i,(short)salle[i],0);
        if (!(i<21)){
            noeud[i].N = &noeud[i-21];
        }
        if (!(i>293)){
            noeud[i].S = &noeud[i+21];
        }
        
        if (!(i%21==20)){
            noeud[i].E = &noeud[i+1];
        }
        if (!(i%21==0)){
            noeud[i].W = &noeud[i-1];
        }
    }
}

//UNE ERREUR, NE DEVRAIT PAS EXISTER -> tableau de pointeur, sauf que les emplacements mémoires peuvent changer entre les différentes exécutions
//CRÉÉ À LA BASE POUR NE PAS À AVOIR À RECALCULER UN CHEMIN TOUT LE TEMPS -> DECISION DE SAVE LE CHEMIN CALCULÉ PLUTOT QUE LES NODES -> NON IMPLEMENTE PCQ ECHEC DES TRAITEMENTS PREDECESSEURS
void saveBaseLLFile(){
    LL noeuds[315];
    liaisonLL(noeuds);
    FILE * fichier = NULL;
    if((fichier = fopen("patternLL","wb"))==NULL){
        printf("Erreur lors de la sauvegarde du fichier pattern pour les noeuds de la salle\n");
    }
    fwrite(noeuds,sizeof(LL),315,fichier);
    fclose(fichier);
    fichier = NULL;
}

void getBaseLLFile(LL noeuds[315]){
    /*
    char retry = 0;
    FILE * fichier = NULL;
    test :
    if((fichier = fopen("patternLL","rb"))==NULL){
        if (retry == 0){
            retry++;
            saveBaseLLFile();
            printf("Test 1\n");
            goto test;
        } else {
            printf("Erreur lors de la lecture du fichier pattern pour les noeuds de la salle\n");
            goto suite;
        }
    }
    suite :
    printf("Avant read du fichier\n");
    fread(noeuds,sizeof(LL),315,fichier);
    fclose(fichier);
    fichier = NULL;
    printf("Après read du fichier\n");
    */
    system("rm patternLL");
    FILE * fichier = NULL;
    saveBaseLLFile();
    if((fichier = fopen("patternLL","rb"))==NULL){
        printf("Erreur lors de la lecture du fichier pattern pour les noeuds de la salle\n");
    }
    fread(noeuds,sizeof(LL),315,fichier);
    fclose(fichier);
    fichier = NULL;
}

//une fonction de test pour voir/afficher si le noeud possede des voisins
void test(int node,LL noeuds[315]){
    printf("Noeud %d : %d\n", node, noeuds[node].value);
    if (noeuds[node].N != NULL){
        printf("Noeud.N %d : %d\n", node, (noeuds[node].N)->value);
    } else {
        printf("Pas de Noeud.N\n");
    }
    
    if (noeuds[node].E != NULL){
        printf("Noeud.E %d : %d\n", node, (noeuds[node].E)->value);
    } else {
        printf("Pas de Noeud.E\n");
    }
    
    if (noeuds[node].S != NULL){
        printf("Noeud.S %d : %d\n", node, (noeuds[node].S)->value);
    } else {
        printf("Pas de Noeud.S\n");
    }
    
    if (noeuds[node].W != NULL){
        printf("Noeud.W %d : %d\n", node, (noeuds[node].W)->value);
    } else {
        printf("Pas de Noeud.W\n");
    }
}

short positionLxltoOneLinePos(char X, char x, char y) {
    /*X la longueur de la grille
    x l'abscisse du point dans la grille
    y l'ordonnée du point dans la grille
    Renvoie l'indice équivalent pour un tableau à une dimension représentant la grille
    Le premier point est x=1 et y=1 dans la grille*/
    x--;
    y--;
    short ligne = X * y;
    char colonne = x;
    return ligne+colonne;
}

char positionOneLinetoLxlPos(short n, char X, char retour[2]) {
    /*X la longueur de la grille
    n l'indice du point dans le tableau
    Renvoie les coordonnées x,y du point dans une grille de longueur X
    Le premier point est x=1 et y=1 dans la grille*/
    retour[0] = n%X+1;
    retour[1] = n/X+1;
    return 1;
}

char distanceMin(short i, short j){
    char x1y1[2], x2y2[2];
    positionOneLinetoLxlPos(i, 21, x1y1);
    positionOneLinetoLxlPos(j, 21, x2y2);
    return abs(x1y1[0] - x2y2[0]) + abs(x1y1[1] - x2y2[1]);
}

char compareDistMin(short i, short j, short obj){
    char distI = distanceMin(i, obj);
    char distJ = distanceMin(j, obj);
    if (distI < distJ) return 1;
    else if (distI == distJ) return 0;
    else return -1;
}

char compareParHeuristique(LL * noeud1, LL * noeud2){
    if (noeud1->heuristique < noeud2->heuristique) return 1;
    else if (noeud1->heuristique == noeud2->heuristique) return 0;
    else return -1;
}

void reallocEspaceChemin(LL ** espace, int quantite){
    if((espace = (LL **)realloc(espace,quantite*sizeof(LL *)))==NULL){
        printf("Erreur lors de la réllocation d'espace mémoire pour le LL\n");
    }
}

void libererTouteMaMemoire(LL ** espace){
    free(*espace);
}


void ajouterLLTab(LL * espace[], LL * element, int * taille, int * quantite){
    if ((*taille)>=(*quantite)){
        reallocEspaceChemin(espace,10);
        *quantite +=10;
    }
    int place = 0;
    for(; place<(*taille);place++){
        if(compareParHeuristique(element, espace[place])==1){
            break;
        }
    }
    for(int i = *taille; i > place; i++ ) espace[i] = espace[i-1];
    espace[place] = element;
    //espace[*taille] = *element;
    (*taille)++;
}

LL * retirerLLTab(LL * espace[], int indice, int * taille, int * quantite){
    LL * retour = espace[indice];
    for(int i = indice; i<((*taille)-1) ; i++){
        espace[i] = espace[i+1];
    }
    (*taille)--;
    return retour;
}

char inLLTab(LL * espace[], int indice, int taille){
    for (int i=0; i<taille; i++){
        if (indice == espace[i]->indice) return 1;
    }
    return 0;
}

//INSPIRE PAR LE PSEUDO CODE SUR WIKIPEDIA ET AILLEURS

char pathfinding(LL noeuds[315], short depart, short destination){
    if (depart == destination) return 0;
    for (int i =0; i < 315; i++){
        noeuds[i].heuristique = distanceMin(i,destination);
    }
    int tailleAP,capaciteAP, tailleP, capaciteP;
    tailleAP = 0;
    tailleP = 0;
    capaciteAP = 4;
    capaciteP = 1;
    LL * cheminParcouru[capaciteP];
    LL * cheminAParcourir[capaciteAP];
    ajouterLLTab(cheminAParcourir,&(noeuds[depart]),&tailleAP, &capaciteAP);
    while(tailleAP>0){
        LL * actuel = retirerLLTab(cheminAParcourir,0,&tailleAP, &capaciteAP);
        if (actuel->indice == destination){
            actuel = retirerLLTab(cheminParcouru,tailleP-1,&tailleP, &capaciteP);
            printf("Node %d\n", cheminParcouru[i]->indice);
            pathfinding(noeuds, depart, actuel->indice);
            return 1;
        } else {
            LL * temp[4] = { actuel->N,actuel->E,actuel->S,actuel->W};
            for(int k = 0; k<4; k++){
                if (!(inLLTab(cheminParcouru,temp[k]->indice,tailleP) || (inLLTab(cheminAParcourir,temp[k]->indice,tailleAP) && (temp[k]->cout < (actuel->cout+1)) && temp[k]->cout >= 0))){
                    temp[k]->cout = actuel->cout +1;
                    temp[k]->heuristique = temp[k]->cout + distanceMin(temp[k]->indice,destination);//distance([v.x, v.y], [objectif.x, objectif.y])
                    ajouterLLTab(cheminAParcourir,temp[k],&tailleAP, &capaciteAP);
                }
            }
            ajouterLLTab(cheminParcouru,actuel,&tailleP, &capaciteP);
        }
    }
    return 0;
}

/*
int main(){
    LL noeuds[315];
    //getBaseLLFile(noeuds); surement une erreur de la vie quand on y repense bien à cette fonction
    liaisonLL(noeuds);
    printf("Hello World\n");
    //for(int i = 0; i<315;i++) printf("Noeud %d value : %d\n", i, noeuds[i].value);
    test(310,noeuds);
    pathfinding(noeuds, 22, 292); //tentative de test en renomant la salle02 en salle01 dans le bon folder de test
    return 0;
}
*/