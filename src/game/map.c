#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#define save "./save1"
#define ETAGE_DEFAULT 1



void pass(){};

char ecrireErreur(char * msg) {
    FILE * fichier = NULL;
    char * errorFichierChemin = "/errors/errors.txt";
    char errorPath[strlen(save)+strlen(errorFichierChemin)+1];
    strcpy(errorPath, save);
    strcat(errorPath,"/errors/errors.txt");
    if((fichier = fopen(errorPath, "a"))==NULL) {
        printf("Erreur lors de l'ouverture du fichier d'erreur pour y ajouter une erreur\n");
        return 0;
    } else {
        printf("%s",msg);
        fprintf(fichier, "%s", msg);
        fclose(fichier);
        fichier = NULL;
        return 1;
    }
}

char ecrireFichierSTR(char * nomFichier, char * msg, char * msgErreur) {
    FILE * fichier = NULL;
    if((fichier = fopen(nomFichier, "w"))==NULL) {
        ecrireErreur(msgErreur);
        return 0;
    } else {
        printf("%s",msg);
        fprintf(fichier, "%s", msg);
        fclose(fichier);
        fichier = NULL;
        return 1;
    }
}

char ecrireFichierINT(char * nomFichier, int nombre, char * msgErreur) {
    FILE * fichier = NULL;
    if((fichier = fopen(nomFichier, "w"))==NULL) {
        ecrireErreur(msgErreur);
        return 0;
    } else {
        //printf("%d\n", nombre);   /*en général lors de l'écriture d'un fichier je préfère afficher dans la console ce qui va être écrit dans le fichier, mais pour la seed on peut s'en passer, c'est mieux*/
        fprintf(fichier, "%d", nombre);
        fclose(fichier);
        fichier = NULL;
        return 1;
    }
}

char folderExist(char * nomFolder){
    char * debut = "ls -d ";
    char * fin =  " > temp.txt";
    char command[strlen(debut) + strlen(nomFolder) + strlen(fin) + 1];
    strcat(strcat(strcpy(command, debut), nomFolder),fin);
    system(command);
    FILE * fichier = NULL;
    if((fichier = fopen("temp.txt", "r"))==NULL){
        printf("Erreur\n");
    }
    char test[strlen(nomFolder)+1];
    fscanf(fichier, "%s", test);
    fclose(fichier);
    fichier = NULL;
    system("rm -f temp.txt");
    return strcmp(test,nomFolder)==0;
}

char creerFolder(char * nomFolder) {
    if(!folderExist(nomFolder)) {
        char * debut = "mkdir -p ";
        char command[strlen(debut) + strlen(nomFolder)+1];
        strcat(strcpy(command,debut),nomFolder);
        system(command);
        printf("Le folder %s a bien été créé\n",nomFolder);
        return 1;
    }
    return 1;
}

char estPremier(int nombre){
    /*renvoie si le nombre est premier*/
    if(nombre==2) return 1;
    else if (nombre%2==0) return 0;
    for(int i= 3; i<=sqrt(nombre);i+=2){
        if (nombre%i==0) return 0;
    }
    return 1;
}

char afficherGrille(char * grille, char X, char Y){
    /*affiche les elements d'une grille dans la console selon la longueur X et la largeur Y de la grille*/
    for (char i = 0; i<Y; i++) {
        printf("[");
        for (char j = 0; j<X; j++) {
            char nombre[4];  /*+3 pour les caracteres]\n\0*/
            sprintf(nombre,"%hhd",grille[(short)i*X+j]);
            char affichage[7] = "";
            if (strlen(nombre) < 2) strcat(affichage," ");
            strcat(affichage,nombre);
            strcat(affichage,j != X-1 ? ", " : "]\n");
            printf("%s",affichage);
        }
    }
    printf("\n");
    return 1;
}

char nbPremiers(int n, int * premiers) {
    /*Recoit en parametre un tableau de taille n+1
    modifie le tableau les nombres premiers jusqu'à n non inclus*/
    if (n<=5) {
        ecrireErreur("Le nombre est trop petit pour choisir les nombres directeurs\n");
        return 0;
    }
    char c = 0;
    for(int i=n; i>=2; i--){
        if (estPremier(i)) {
            premiers[2 - c] = i;
            if (c++>=2) break;
        }
    }
    if (c<2) return 0;
    return 1;
}

void troupleNombresPremiers(int n, int nxy[3]) {
    /*ecrits dans nxy les trois derniers nombres premiers inférieurs ou égaux à n
    il faut que le n soit strictement plus grand que 5, sinon la fonction precedente ne générera pas au moins 2 nombrs premiers*/
    if(n<=5) {
        nbPremiers(5+1,nxy);
    } else {
        nbPremiers(n,nxy);
    }
}

char sauvegarderTrouple(int nxy[3]) {
    /*creer un fichier nombresDirecteurs.txt et y inscrit les trois nombres premiers en parametres, 1 par ligne*/
    char * nomFichierChemin = "/map/nombresDirecteurs.txt";
    char nomFichier[strlen(save)+strlen(nomFichierChemin)+1];
    strcpy(nomFichier, save);
    strcat(nomFichier,nomFichierChemin);
    FILE * fichier = NULL;
    if((fichier = fopen(nomFichier, "w"))==NULL) {
        ecrireErreur("Erreur lors de la sauvegarde des nombres directeurs\n");
        return 0;
    } else {
        for (char i=0; i<3; i++) fprintf(fichier, "%d\n", nxy[i]);
        fclose(fichier);
        fichier = NULL;
        return 1;
    }
}

char lireTrouple(int nxy[3]) {
    /*lit le fichier nombresDirecteurs.txt et renvoie les trois nombres premiers*/
    char * nomFichierChemin = "/map/nombresDirecteurs.txt";
    char nomFichier[strlen(save)+strlen(nomFichierChemin)+1];
    strcpy(nomFichier, save);
    strcat(nomFichier,nomFichierChemin);
    FILE * fichier = NULL;
    if((fichier = fopen(nomFichier, "r"))==NULL) {
        ecrireErreur("Erreur lors de la lecture des nombres directeurs\n");
        return 0;
    } else {
        for (char i=0; i<3; i++) fscanf(fichier, " %d\n", &nxy[i]);
        fclose(fichier);
        fichier = NULL;
        return 1;
    }
}

char detecterSalle(char map[121], char n, char retour[2]) {
    /*map : tableau de char à une dimension contenant toutes les cases de la map
    n : numéro du mur dans la map dont nous voulons connaitre les salles qu'il sépare
    retour : tableau de 2 char des salles adjacentes au murs*/
    char rangN = n * 2 - 1;
    if (0 < n%11 && n%11 <= 5) {
        retour[0] = map[rangN-1];
        retour[1] = map[rangN+1];
    } else {
        retour[0] = map[rangN-11];
        retour[1] = map[rangN+11];
    }
    return 1;
}

char creerLabyrinthe(char map[121]) {
    /*creer la map à 1 dimension selon les éléments de la seed sauvegardés en amont dans le fichier /map/seed.txt*/
    for (char i=0; i<11; i++) {
        if (i%2==0) {
            for (char j=0; j<11; j++) map[i*11+j] = j%2==0 ?(j/2+1)+(6*(i/2)) : -1;
        } else {
            for (char j=0; j<11; j++) map[i*11+j] = j%2==0 ? -1 : -2;
        }
    }
    char mursRestant = 35;
    int nxy[3];
    lireTrouple(nxy);
    char mur = (nxy[1]*nxy[2])%60+1;
    while (mursRestant > 0) {
        mur = (mur-1+nxy[0])%60+1; //+1 au modulo pour etre sur d'avoir un nombre entre 1 et 60 compris, et -1 au mur+n pour retirer ce qu'on avait ajouté au modulo, pcq sinon ca ferait en gros une boucle infinie
        char salles[2];
        detecterSalle(map,mur,salles);
        if (salles[0] != salles[1]) {
            mursRestant--;
            map[mur * 2 - 1] = 0;
            for (char i=0; i<121; i++) {
                if(map[i] == salles[1]) map[i] = salles[0];
            }
        }
    }
    return 1;
}

char mapIndiceToNumeroSalle(char indice) {
    /*indice = l'indice de la salle dans la liste des salles triées par ordre croissant
    renvoie la position de la salle, et non son numéro*/
    indice++;
    char retour = indice%11 == 0 ? 11 : indice%11;
    return (retour/2+1)+6*(indice/22);
}

char mapNumeroSalleToIndice(char num) {
    /*num est la position de la salle dans la map, allant de 1 à 36 compris, et non son numéro, il ne faut pas les confondre
    Renvoie l'indice de la salle dans le tableau de la map*/
    num--;
    char ligne = num/6;
    ligne = ligne*2;
    char colonne = num%6;
    colonne = colonne*2;
    return ligne*11+colonne;
}

int creerSeed(int seed) {
    /*creer la seed de la map et l'ecrit dans le fichier seed.txt*/
    if (seed == 0) {    /*par defaut si on veut du random il faut que seed = 0*/
        srand( time( NULL ) );
        seed = rand()%(2000000-31)+1; //31 minimum pour eviter les nombres premiers 2 et 3 pour la generation de la map et 13 et 19 pour la génération des salles
    } else {
        while (seed > 2000001 || seed < 32) {
            if (seed > 2000001) seed -= 2000000;
            else seed = 32;
        }
    }
    char * nomFichierChemin = "/map/seed.txt";
    char nomFichier[strlen(save)+strlen(nomFichierChemin)+1];
    strcpy(nomFichier, save);
    strcat(nomFichier,nomFichierChemin);
    char * msgErreur = "Erreur lors de l'écriture/sauvegarde de la seed\n";
    ecrireFichierINT(nomFichier,seed,msgErreur);
    return seed;
}

int lireSeed() {
    int seed;
    FILE * fichier = NULL;
    char * nomFichierChemin = "/map/seed.txt";
    char nomFichier[strlen(save)+strlen(nomFichierChemin)+1];
    strcpy(nomFichier, save);
    strcat(nomFichier,nomFichierChemin);
    if((fichier = fopen(nomFichier,"r"))==NULL) {
        ecrireErreur("Erreur lors de la lecture de la seed\n");
        return -1;
    } else {
        fscanf(fichier,"%d",&seed);
        fclose(fichier);
        fichier = NULL;
        return seed;
    }
}

char salleAutourSalleListe36(char n, char * retour) {
    /*renvoie les indices des salles [N,E,S,W] autour de la salle donnée, dans la liste de 36 elements (juste les salles sans les murs)
    renvoie donc l'indice de la salle dans la map
    renvoie -1 si pas de salle voisine sur le coté concerné
    le renvoie est, bien évidement, effectué par le tableau retour*/
    for (char i = 0; i<4;retour[i++] = -1);
    if (n >= 6) retour[0] = n-6;
    if (n%6 != 5) retour[1] = n+1;
    if (n <= 29) retour[2] = n+6;
    if (n%6 != 0) retour[3] = n-1;
    return 1;
}

char mapRandomizer(int seed, char map[121], char etage) {
    /*melange les numéros des salles dans la map et renvoie la map*/
    int nxy[3];
    troupleNombresPremiers(seed,nxy);   //la fonction utilise soit malloc, soit realloc, donc il faut free l'espace demandé
    sauvegarderTrouple(nxy);
    creerLabyrinthe(map);
    char liste[36];
    char pos;
    for (char i = 0; i<36; i++) {
        liste[i] = ((((long)(etage + i + nxy[0]) * nxy[1]) * nxy[2]) % 36) + 1;     //transformer en long l'operation, sinon je finis avec des nombres negatif
        if (liste[i] == 1) pos = i;
    }
    char pos2,temp;
    char adjacents[4];
    salleAutourSalleListe36(pos,adjacents);     //j'aurais pu recycler la variable pos et ne pas faire de pos2, mais si je l'avais fait ca aurait été confusing donc c'est mieux d'éviter
    for (char i = 0; i<4; i++) {
        if (adjacents[i] >= 0) {
            if (liste[adjacents[i]] == 2) {
                pos2 = adjacents[i];
                temp = liste[pos2];
                liste[pos2] = liste[(pos2 + 18) % 36];
                liste[(pos2 + 18) % 36] = temp ;
            }
        }
    }

    for (char i = 0; i<36; i++) {
        map[mapNumeroSalleToIndice(i+1)] = liste[i];
    }

    return 1;
}

char sauvegarderMap(int seed, char etage) {
    /* creer et sauvegarde la map à une dimension dans le fichier /saveX/map/map.txt*/
    char nomFolder[strlen(save)+strlen("/map")+1];
    strcpy(nomFolder, save);
    creerFolder(strcat(nomFolder,"/map"));
    if (seed == 0) seed = creerSeed(0);
    else seed = creerSeed(seed);

    char map[121];
    mapRandomizer(seed,map,etage);

    char * nomFichierChemin = "/map/map.txt";
    char nomFichier[strlen(save)+strlen(nomFichierChemin)+1];
    strcpy(nomFichier, save);
    strcat(nomFichier,nomFichierChemin);
    FILE * fichier = NULL;
    if((fichier = fopen(nomFichier,"w"))==NULL) {
        ecrireErreur("Erreur lors de la sauvegarde de la map\n");
        return 0;
    } else {
        for (char i = 0; i<121; i++) fprintf(fichier, " %d", map[i]);
        fclose(fichier);
        fichier = NULL;
        return 1;
    }
}

char lireMap(char map[121]) {
    /*lit la map dans le fichier map.txt et la transforme en liste à une dimension pour l'exploiter*/
    char * nomFichierChemin = "/map/map.txt";
    char nomFichier[strlen(save)+strlen(nomFichierChemin)+1];
    strcpy(nomFichier, save);
    strcat(nomFichier,nomFichierChemin);
    FILE * fichier = NULL;
    if((fichier = fopen(nomFichier,"r"))==NULL) {
        ecrireErreur("Erreur lors de la lecture de la map\n");
        return 0;
    } else {
        for (char i = 0; i<121; i++) fscanf(fichier, " %hhd", &map[i]);
        fclose(fichier);
        fichier = NULL;
        return 1;
    }
}

char obtenirEtatPorteByIndiceSalle(char n,char map[121], char retour[4]) {
    /*n est l'indice de la salle dans la map
    Renvoie un tableau [N,E,S,W] de booleen sur l'etat des portes de la salle*/
    for (char i = 0; i<4; i++) retour[i] = 1;
    char position = mapNumeroSalleToIndice(n);
    if (position <= 6) retour[0] = 0;
    else if (position >= 31) retour[2] = 0;
    if (position%6 == 0) retour[1] = 0;
    else if (position%6 == 1) retour[3] = 0;
    if (retour[0]) retour[0] = map[n-11] == 0 ? 1 : 0;
    if (retour[1]) retour[1] = map[n+1] == 0 ? 1 : 0;
    if (retour[2]) retour[2] = map[n+11] == 0 ? 1 : 0;
    if (retour[3]) retour[3] = map[n-1] == 0 ? 1 : 0;
    return 1;
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

short tileDecentraliseLxlto21x15(short n, char L, short depart) {
    /*renvoie l'indice n correspondant de la map Lxl dans la map 21x15
    Il est important de garder en tete que l'indice depart doit permettre la mise en place totale de la grille de longueur L dans la grille 21x15*/
    short ligneD = depart/21;
    char colonneD = depart%21;
    return (ligneD+(n/L))*21+colonneD+(n%L);
}

short tileCentraliseLxlto21x15(short n, char L) {
    /*renvoie l'indice n correspondant de la map Lxl dans la map 21x15
    en supposant que nous la dimension L est toujours inférieur à 21*/
    short baseY = (21 - L) / 2 ;
    short baseX = baseY * 21 ;
    return tileDecentraliseLxlto21x15(n,L,baseX+baseY);
}

char fairePetitLac(char numeroSalle, char **espaceLac, char dimensionLac[2]) {
    /*Créé un petit lac dans une grille
    Renvoie la grille dans espaceLac et sa longueur et sa largeur dans dimensionLac
    Il ne faudra pas oublier de free l'espaceLac une fois son utilisation fini en dehors de la fonction*/
    char idBlockEau = 11;
    int nxy[3];
    lireTrouple(nxy);
    int n = nxy[0], x = nxy[1], y = nxy[2];
    int seed = lireSeed();

    char longueur = (n + x + y) % 10 >= 5 ? (n + x + y) % 10 + 1 : (n + x + y) % 10 + 5 + 1;
    char largeur = (n + x + y - 2) % 7 >= 4 ? (n + x + y - 2) % 7 : (n + x + y - 2) % 7 + 4;
    dimensionLac[0] = longueur; //sauvegarde des dimensions du tableau pour les renvoyer
    dimensionLac[1] = largeur;

    *espaceLac = (char *)malloc(sizeof(char) * longueur * largeur);
    if (*espaceLac == NULL) {
        ecrireErreur("Erreur lors de l'allocation de mémoire pour la création d'un lac");
        return 0;
    }

    for (short i = 0; i < (short)longueur * largeur; i++) {
        (*espaceLac)[i] = 1;
    }

    char nbPointsDirecteurs = longueur / 3 + largeur / 3 + 2;
    short posPointsDirecteurs[nbPointsDirecteurs];


    short pos;
    for (char i = 0; i < nbPointsDirecteurs; i++) {
        pos = (seed + numeroSalle + (x * y * n * i)) % (longueur * largeur);
        (*espaceLac)[pos] = idBlockEau;
        posPointsDirecteurs[i] = pos;
    }

    for (char i = 0; i < nbPointsDirecteurs - 1; i++) {
        char point1[2], point2[2];
        char x1, y1, x2, y2, X, Y, xMin, yMin;
        positionOneLinetoLxlPos(posPointsDirecteurs[i], longueur, point1);
        x1 = point1[0];
        y1 = point1[1];
        for (char j = i + 1; j < nbPointsDirecteurs; j++) {
            positionOneLinetoLxlPos(posPointsDirecteurs[j], longueur, point2);
            x2 = point2[0];
            y2 = point2[1];
            if (y2 - y1 != 0 || x2 - x1 != 0) {
                xMin = x1 < x2 ? x1 : x2;
                yMin = y1 < y2 ? y1 : y2;
                if (y2 - y1 == 0) {
                    for (char k = 0; k < abs(x2 - x1); k++) {
                        X = xMin + k;
                        Y = y1;
                        if (X < 0 || Y < 0 || X >= longueur || Y >= largeur) continue;
                        (*espaceLac)[positionLxltoOneLinePos(longueur, X, Y)] = idBlockEau;
                    }
                } else if (x2 - x1 == 0) {
                    for (char k = 0; k < abs(y2 - y1); k++) {
                        X = x1;
                        Y = yMin + k;
                        if (X < 0 || Y < 0 || X >= longueur || Y >= largeur) continue;
                        (*espaceLac)[positionLxltoOneLinePos(longueur, X, Y)] = idBlockEau;
                    }
                } else {
                    float m = x2 - x1 != 0 ? (float)(y2 - y1) / (x2 - x1) : 0;
                    float p = y1 - m * x1;
                    if (abs(y2 - y1) > abs(x2 - x1)) {
                        for (char k = 0; k < abs(y2 - y1); k++) {
                            X = (char)round((double)(yMin + k - p) / m);
                            Y = yMin + k;
                            if (X < 0 || Y < 0 || X >= longueur || Y >= largeur) continue;
                            (*espaceLac)[positionLxltoOneLinePos(longueur, X, Y)] = idBlockEau;
                        }
                    } else {
                        for (char k = 0; k < abs(x2 - x1); k++) {
                            X = xMin + k;
                            Y = (char)round((double)m * (xMin + k) + p);
                            if (X < 0 || Y < 0 || X >= longueur || Y >= largeur) continue;
                            (*espaceLac)[positionLxltoOneLinePos(longueur, X, Y)] = idBlockEau;
                        }
                    }
                }
            }
        }
    }


    return 1;
}

char sommeTableau(char * tab, char taille) {      /*return char car je sais que l'appel renvoie une somme inférieur à 16, la fonction n'est pas garanti pour tous les usages autres que celui prévu à la base*/
    /*renvoie la somme des elements du tableau*/
    char somme = 0;
    if (taille == 0) return 0;
    else for (char i = 0; i<taille; i++) somme += tab[i];
    return somme;
}

char algoGlouton(char sommeMax, char retour[5], char etape) { /*ne fonctionne pas, à voir pour améliorer/fixe*/
    /*etape = 5 doit etre la valeur par defaut
     *le tableau retour doit etre initialisé à {0,0,0,0} avant son utilisation*/
    if (etape<=0) return 1; /*fin de la fonction recursive*/
    char disponible[4] = {5,4,3,2};
    char temp[5] = {0,0,0,0};
    for (char i = 0; i<5; i++) temp[i] = retour[i];
    for (char i = 0; i<4; i++) {
        algoGlouton(sommeMax-disponible[i], temp, etape-1);
        if ((disponible[i]+sommeTableau(retour,5)) <= sommeMax && sommeTableau(temp,5)==(sommeMax-disponible[i])) {
            retour[5-etape] = disponible[i];
            algoGlouton(sommeMax-disponible[i], retour, etape-1);
        }
    }
    return 1;
}

char genMobs(char numeroSalle, char retour[5]) {
    /*generation des mobs pour une salle
    fonction temporaire : en attendant d'etre remplacée par la solution agloGlouton*/
    int seed = lireSeed();
    char prob = (numeroSalle + seed) % 14;
    for (char i = 0; i<5; i++) retour[i] = 0;
    if (prob < 10) retour[0] = 2;       /*pour la premiere case du tableau*/
    else if (prob < 13) retour[0] = 3;
    else retour[0] = 5;
    if (prob < 5 || prob == 6) retour[1] = 2;       /*pour la deuxieme case du tableau*/
    else if (prob < 8 || prob == 10 || prob == 11) retour[1] = 3;
    else if (prob < 13) retour[1] = 4;
    else retour[1] = 5;
    if (prob < 4) retour[2] = 2;        /*pour la troisieme case du tableau*/
    else if (prob < 7) retour[2] = 3;
    else if(prob%2==0) retour[2] = 4;
    else if(prob < 13) retour[2] = 5;
    if (prob < 2) retour[3] = 2;
    else if (prob == 4 || prob == 5) retour[3] = 3;
    else if (prob == 2 || prob == 6) retour[3] = 4;
    else if (prob == 3) retour[3] = 5;
    if (prob == 0) retour[4] = 2;
    else if (prob == 1) retour[4] = 3;
    return 1;
}

char mobsShuffle(char mobs[5]) {
    /* Melange la place des mobs dans le tableau*/
    int places[3];
    lireTrouple(places);
    for (char i = 0; i<3; i++) places[i] = places[i]%5;
    char temp;
    for (char i = 0; i<3; i++) {
        temp = mobs[places[i]];
        mobs[places[i]] = mobs[i];
        mobs[i] = temp;
    }
    return 1;
}

double rad(int degre) {
    /*renvoie l'angle en radian*/
    return degre * M_PI / 180;
}

char racineNieme(char nMob, char numeroSalle, short retour[nMob]) {
    /*renvoie la position des mobs selon un cercle*/
    char coorX[nMob], coorY[nMob];
    for(char i = 0; i<nMob; i++) {
        retour[i] = 0;
        coorX[i] = 0;
        coorY[i] = 0;
    }

    double angle = fmod(rad((double)lireSeed()+numeroSalle),M_PI);     /*determination de l'angle de référence*/

    /*centre 0x0 -> translation au centre de la salle soit 11x8*/
    char transX = 11;
    char transY = 8 ;

    char homothetie = 5;     /*pour agrandir le rayon du cercle trigo*/

    for (char i = 0; i<nMob; i++) {
        /*coorX[i] = (char)cos((angle + (M_PI * i)) / (nMob / 2.0)) * homothetie + transX;*/
        coorX[i] = (char)round(cos((angle + (M_PI * i)) * 2 / nMob) * homothetie + transX);
        coorY[i] = (char)round(sin((angle + (M_PI * i)) * 2 / nMob) * homothetie + transY);
    }

    for (char i = 0; i<nMob; i++) retour[i] = positionLxltoOneLinePos(21,coorX[i],coorY[i]);

    return 1;
}

char posInPosTab(short pos, char taille, short posTab[taille]) {
    /*renvoie si la position est dans le tableau des positions*/
    char c = 0;
    for (char i = 0; i<taille; i++) {
        if (pos == posTab[i]) c++;
    }
    return c;
}

char placer1Mob(char salle[21*15], short placement, char taille, short mobsPos[taille], char indice) {
    char c = 0;
    while((posInPosTab(placement,taille,mobsPos) >= 1) ||  (!(salle[placement] <=4 && salle[placement] > 0))) placement++;
    mobsPos[indice] = placement;
    return 1; 
}

char allocEspaceMobTab(char ** mob, char taille) {
    /*alloue l'espace demandé pour le tableau de mob
     Il ne faudra pas oublier de free l'espace apres utilisation*/
    if((*mob = (char*)malloc(taille*sizeof(char)))==NULL) {
        ecrireErreur("Erreur lors de l'allocation d'espace pour le tableau de mob\n");
        return 0;
    } else {
        return 1;
    }
}

char allocEspaceMobPosTab(short ** mobPos, char taille) {
    /*alloue l'espace demandé pour le tableau des positions des mobs
     Il ne faudra pas oublier de free l'espace apres utilisation*/
    if((*mobPos = (short*)malloc(taille*sizeof(short)))==NULL) {
        ecrireErreur("Erreur lors de l'allocation d'espace pour le tableau des positions des mobs\n");
        return 0;
    } else {
        return 1;
    }
}

char creerSalle(char identifiantSalle, char etage) {
    /*identifiantSalle est l'identifiant de la salle, pas sa position ni son indice dans map
    creer la salle demandée*/
    char map[121];
    lireMap(map);
    char indiceSalle;
    for (char i = 0; i<121; i++) if (map[i] == identifiantSalle) indiceSalle = i;

    char salle[21*15];      /*21*15 = 315*/
    for (short i = 0; i<315;i++) salle[i] = 1;

    for (short i = 0; i<21; i++) salle[i] = 5;       /*mur de face ligne supérieur*/
    for (short i = 315-21; i<315; i++) salle[i] = 5;     /*mur de face ligne inférieur*/
    for (short i = 21; i<(315-21); i+=21) salle[i] = 6;     /*mur bg*/
    for (short i = (21+20); i<(315-21); i+=21) salle[i] = 7;        /*mur bd*/

    /*placement des portes dans la salle*/
    char portes[4];
    obtenirEtatPorteByIndiceSalle(indiceSalle,map,portes);      /*portes = {N,E,S,W}*/
    if (portes[0]) salle[10] = 8;
    if (portes[1]) salle[21*8-1] = 10;
    if (portes[2]) salle[21*14+10] = 8;
    if (portes[3]) salle[21*7] = 9;

    /*on va se donner environ 30% des tiles au sol qui seront d'un motif différent*/
    int seed = lireSeed();
    int seedTemp = lireSeed();
    int nxy[3]; lireTrouple(nxy);
    int n = nxy[0], x = nxy[1], y = nxy[2];
    char tileModif = 75 ; /*19*13 = 247 donc 30% = 74.1 donc 75 tiles à modifier*/
    tileModif = tileModif - (n*identifiantSalle%50); /*247 donc 10% = 24.7 donc 25 tiles minimum : 75-50=25*/
    short tile = (seed+identifiantSalle*n)%247;
    char tailleSeed = 0;
    while (seedTemp > 0) {
        tailleSeed++;
        seedTemp/=10;
    }
    while (tileModif > 0) {
        tile = (tile+tailleSeed*80)%247;
        salle[tileCentraliseLxlto21x15(tile,19)] = tile%3+1+1;
        tileModif--;
    }
    /*pas sûr de la qualité du random pour les dalles au sol mais pas grave, on va dire que c'est OK*/

    char nbMob = 0;
    char * mobs;
    short * mobsPositions;
    short positionCarreCentre[4] = {21*5+6,21*5+14,21*9+14,21*9+6};

    /*Génération de la base d'une salle OK, maintenant il faut faire les spécificités de la salle*/

    if (identifiantSalle == 1) pass();         /*salle de base/spawn*/

    else if (identifiantSalle == 2) {       /*salle du boss*/
        short positionBasesPilliers[4] = {21*3+4, 21*3+15, 21*10+15, 21*10+4};
        for (char i = 0; i<4; i++) {    /*constructions des pilliers*/
            salle[positionBasesPilliers[i]] = 15;
            salle[positionBasesPilliers[i]+1] = 16;
            salle[positionBasesPilliers[i]+22] = 17;
            salle[positionBasesPilliers[i]+21] = 18;
        }
        nbMob = 5;
        allocEspaceMobTab(&mobs,nbMob);
        mobs[0] = 1;
        for (char i = 1; i<nbMob; i++) mobs[i] = (seed+etage+i+((seed+etage)%2))%2 + 2; /*equivalent à la ternaire (seed+etage)%2 ? (seed+etage+i)%2 + 2 : (seed+etage+i+1)%2 + 2; pour que les mobs soient organisé comme ca [1,2,3,2,3] ou ca [1,3,2,3,2]*/
        allocEspaceMobPosTab(&mobsPositions,nbMob);
        mobsPositions[0] = 21*7+10;     /* position centrale pour le boss*/
        for (char i = 0; i<4; i++) mobsPositions[i+1] = positionCarreCentre[i];
    }

    else if (identifiantSalle >= 3 && identifiantSalle <= 6) {      /* salle avec mini-boss */
        nbMob = 4;
        allocEspaceMobTab(&mobs,nbMob);
        mobs[0] = 6; mobs[1] = mobs[3] = identifiantSalle%2+2; mobs[2] =(identifiantSalle+1)%2+2;
        allocEspaceMobPosTab(&mobsPositions,nbMob);
        mobsPositions[0] = 21*7+10;     /* position centrale pour le mini-boss*/
        for (char i = 0; i<3; i++) mobsPositions[i+1] = positionCarreCentre[(identifiantSalle+i)%4];
    }

    else if (identifiantSalle >= 17 && identifiantSalle <= 21) salle[tileCentraliseLxlto21x15(seed%(17*11),17)] = 19;   /*salles de loot*/

    else if (identifiantSalle == 22) {  /*salle de loot piégée*/
        salle[tileCentraliseLxlto21x15(seed%(17*11),17)] = 20;
        if (etage < 3) {
            nbMob = 1;
            allocEspaceMobTab(&mobs,nbMob);
            mobs[0] = etage <= 1 ? 7 : 8;
            allocEspaceMobPosTab(&mobsPositions,nbMob);
            mobsPositions[0] = tileCentraliseLxlto21x15(seed%(17*11),17);
        } else {
            nbMob = 6;
            allocEspaceMobTab(&mobs,nbMob);
            for (char i = 0; i<6; i++) mobs[i] = 9;
            allocEspaceMobPosTab(&mobsPositions,nbMob);
            racineNieme(nbMob,identifiantSalle,mobsPositions);
            for (char i = 0; i<nbMob; i++) placer1Mob(salle,mobsPositions[i],nbMob,mobsPositions,i);
        }
    }

    else {      /*le reste des salles, celles des mobs et des intermédiaires*/
        char noEnnemies = 0;
        /*probEau la proba sur 6 qu'il y ait de l'eau*/
        char probEau;
        if (identifiantSalle >= 7 && identifiantSalle <= 16) {      /*salles "intermédiaires"*/
            probEau = 4;
            noEnnemies = 1;
        } else probEau = 1;


        if ((seed+identifiantSalle+etage)%6 < probEau) {
            char *espaceLac;
            char dimensionsLac[2];
            fairePetitLac(identifiantSalle, &espaceLac,dimensionsLac);
            char L = dimensionsLac[0], l = dimensionsLac[1];
            char departX = seed % (17 - L) + 2 ;    /*+2 pour ecarter des bords de la map*/
            char departY = seed % (11 - l) + 2;
            for (short i = 0; i<((short)L*l); i++) {
                salle[tileDecentraliseLxlto21x15(i,L,positionLxltoOneLinePos(21, departX+1, departY+1))] = espaceLac[i];
            }
            //free(espaceLac);    /*la fonction fairePetitLac utilise malloc sur espaceLac, comme nous avons fini d'utiliser espaceLac, il faut libéré la mémoire*/
            espaceLac = NULL;

        }

        for (char i = 0; i<((seed%12)+3); i++) {    /*pour placer des obstacles sur la map, 3 minimum et 14 maximum*/
            char placed = 0;
            short placement = (x * (i + 1) % (17 * 11));
            while (!placed) {
                if (salle[tileCentraliseLxlto21x15(placement, 17)] >= 11) placement = (placement+y) % (17*11);
                else {
                    salle[tileCentraliseLxlto21x15(placement, 17)] = (n+identifiantSalle+i)%3 +12;
                    placed = 1;
                }
            }
        }


        if (!noEnnemies) {      /*22*2+15*3+7*4+6*5 = 147 et nous avons 14 salles avec des monstres*/
            char mobsTemp[5] = {0,0,0,0,0};
            genMobs(identifiantSalle,mobsTemp);
            mobsShuffle(mobsTemp);
            for (char i = 0; i<5; i++) if (mobsTemp[i] > 0) nbMob++;    /*comptage du nombre de mob*/
            allocEspaceMobTab(&mobs,nbMob);
            char c = 0;     /*sert de variable pour compter*/
            for(char i = 0; i<5; i++) if (mobsTemp[i] > 0) mobs[c++] = mobsTemp[i];     /*determination des mobs terminé*/
            allocEspaceMobPosTab(&mobsPositions,nbMob);
            short mobsPositionsTemp[nbMob];
            racineNieme(nbMob,identifiantSalle,mobsPositionsTemp);
            for (char i = 0; i<nbMob; i++) {
                placer1Mob(salle,mobsPositionsTemp[i],nbMob,mobsPositions,i);
            } /*placement des mobs terminé*/
        }
    }


    /*Sauvegarde de la salle dans le fichier*/
    char tailleTile = 16;
    char tempSTR[3];
    sprintf(tempSTR,"%02d",identifiantSalle);
    char salleFolderSTR[strlen("/map/Salle")+strlen(tempSTR)+1];
    strcpy(salleFolderSTR,"/map/Salle");
    strcat((salleFolderSTR),tempSTR);
    char folder[strlen(save)+strlen("/map/Salle")+strlen(tempSTR)+1];
    strcpy(folder,save);
    strcat(folder,salleFolderSTR);
    creerFolder(folder);

    char nomFichierSalle[strlen(folder)+strlen(tempSTR)+strlen("/salle.txt")+1];
    strcpy(nomFichierSalle,folder);
    strcat(strcat(strcat(nomFichierSalle,"/salle"),tempSTR),".txt");
    FILE * fichierSalle = NULL;
    if ((fichierSalle = fopen(nomFichierSalle,"w"))==NULL) {
        ecrireErreur("Erreur lors de l'ecriture du ficher de la salle\n");
        return 0;
    } else {
        short X,Y;
        for (short i = 0; i<315; i++) {
            X = (i%21)*tailleTile;
            Y = (i/21)*tailleTile;
            fprintf(fichierSalle,"%d %d %d\n",X,Y,salle[i]);
        }
        fclose(fichierSalle);
        fichierSalle = NULL;
    }


    if (nbMob > 0) {
        char nomFichierMob[strlen(folder)+strlen(tempSTR)+strlen("/mobs.txt")+1];
        strcpy(nomFichierMob,folder);
        strcat(strcat(strcat(nomFichierMob,"/mobs"),tempSTR),".txt");
        FILE * fichierMob = NULL;
        if ((fichierMob = fopen(nomFichierMob, "w"))==NULL) {
            ecrireErreur("Erreur lors de l'edition du fichier des mobs\n");
        } else {
            short X,Y;
            for (char i = 0; i<nbMob; i++) {
                X = (mobsPositions[i] % 21) * tailleTile;
                Y = (mobsPositions[i] / 21) * tailleTile;
                fprintf(fichierMob,"%d %d %d\n",X,Y,mobs[i]);
            }
            fclose(fichierMob);
            fichierMob = NULL;
            free(mobs);
            mobs = NULL;
            free(mobsPositions);
            mobsPositions = NULL;
        }
    }

    return 1;
}

char creerSave(){
    /*creer le folder de la save donnée*/
    char chemin[strlen(save)+strlen("/errors")+1];
    creerFolder(strcat(strcpy(chemin,save),"/errors"));
}

char creerEtageEntier(char numEtage){
    /*creer un etage entier*/

    creerSave();

    int seed;
    if ((seed = lireSeed())==-1){
        seed = creerSeed(0);
    }
    sauvegarderMap(seed,numEtage);

    for(char salleNum = 1; salleNum <= 36 ; salleNum++){
        creerSalle(salleNum,numEtage);
    }
}

char supprimerEtage(){      //supprime toutes les données de l'étage sauf la seed
    char * commandTemp = "rm -fr /map/S*";
    char command[strlen(save) + strlen(commandTemp) + 1];
    strcat(strcat(strcpy(command,"rm -fr "),save),"/map/S*");
    system(command);
}