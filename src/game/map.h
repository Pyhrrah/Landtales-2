#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#define save "./save1"
#define ETAGE_DEFAULT 1


//void pass(){};        //commenté car pose des erreurs à la compilation

char ecrireErreur(char * msg);

char ecrireFichierSTR(char * nomFichier, char * msg, char * msgErreur);

char ecrireFichierINT(char * nomFichier, int nombre, char * msgErreur);

char folderExist(char * nomFolder);

char creerFolder(char * nomFolder);

char estPremier(int nombre);    //renvoie si le nombre est premier

char afficherGrille(char * grille, char X, char Y);     //affiche les elements d'une grille dans la console selon la longueur X et la largeur Y de la grille

char nbPremiers(int n, int * premiers);     //Recoit en parametre un tableau de taille n+1, modifie le tableau les nombres premiers jusqu'à n non inclus

void troupleNombresPremiers(int n, int nxy[3]);     //ecrits dans nxy les trois derniers nombres premiers inférieurs ou égaux à n, il faut que le n soit strictement plus grand que 5, sinon la fonction precedente ne générera pas au moins 2 nombrs premiers

char sauvegarderTrouple(int nxy[3]);       //creer un fichier nombresDirecteurs.txt et y inscrit les trois nombres premiers en parametres, 1 par ligne

char lireTrouple(int nxy[3]);       //lit le fichier nombresDirecteurs.txt et renvoie les trois nombres premiers

char detecterSalle(char map[121], char n, char retour[2]);      //map : tableau de char à une dimension contenant toutes les cases de la map; n : numéro du mur dans la map dont nous voulons connaitre les salles qu'il sépare; retour : tableau de 2 char des salles adjacentes au murs

char creerLabyrinthe(char map[121]);        //creer la map à 1 dimension selon les éléments de la seed sauvegardés en amont dans le fichier /map/seed.txt

char mapIndiceToNumeroSalle(char indice);       //indice = l'indice de la salle dans la liste des salles triées par ordre croissant, renvoie la position de la salle, et non son numéro

char mapNumeroSalleToIndice(char num);      //num est la position de la salle dans la map, allant de 1 à 36 compris, et non son numéro, il ne faut pas les confondre. Renvoie l'indice de la salle dans le tableau de la map

int creerSeed(int seed);        //creer la seed de la map et l'ecrit dans le fichier seed.txt; par defaut si on veut du random il faut que seed = 0 {

int lireSeed();

char salleAutourSalleListe36(char n, char * retour);        //renvoie les indices des salles [N,E,S,W] autour de la salle donnée, dans la liste de 36 elements (juste les salles sans les murs); renvoie donc l'indice de la salle dans la map, renvoie -1 si pas de salle voisine sur le coté concerné; le renvoie est, bien évidement, effectué par le tableau retour

char mapRandomizer(int seed, char map[121]);        //melange les numéros des salles dans la map et renvoie la map; la fonction utilise soit malloc, soit realloc, donc il faut free l'espace demandé

char sauvegarderMap(int seed);      //creer et sauvegarde la map à une dimension dans le fichier /saveX/map/map.txt

char lireMap(char map[121]);        //lit la map dans le fichier map.txt et la transforme en liste à une dimension pour l'exploiter

char obtenirEtatPorteByIndiceSalle(char n,char map[121], char retour[4]);       //n est l'indice de la salle dans la map; Renvoie un tableau [N,E,S,W] de booleen sur l'etat des portes de la salle

short positionLxltoOneLinePos(char X, char x, char y);      //X la longueur de la grille; x l'abscisse du point dans la grille; y l'ordonnée du point dans la grille; Renvoie l'indice équivalent pour un tableau à une dimension représentant la grille; Le premier point est x=1 et y=1 dans la grille

char positionOneLinetoLxlPos(short n, char X, char retour[2]);      //X la longueur de la grille; n l'indice du point dans le tableau; Renvoie les coordonnées x,y du point dans une grille de longueur X; Le premier point est x=1 et y=1 dans la grille

short tileDecentraliseLxlto21x15(short n, char L, short depart);        //renvoie l'indice n correspondant de la map Lxl dans la map 21x15; Il est important de garder en tete que l'indice depart doit permettre la mise en place totale de la grille de longueur L dans la grille 21x15

short tileCentraliseLxlto21x15(short n, char L);        //renvoie l'indice n correspondant de la map Lxl dans la map 21x15; en supposant que nous la dimension L est toujours inférieur à 21

char fairePetitLac(char numeroSalle, char **espaceLac, char dimensionLac[2]);       //Créé un petit lac dans une grille; Renvoie la grille dans espaceLac et sa longueur et sa largeur dans dimensionLac; Il ne faudra pas oublier de free l'espaceLac une fois son utilisation fini en dehors de la fonction

char sommeTableau(char * tab, char taille);     //return char car je sais que l'appel renvoie une somme inférieur à 16, la fonction n'est pas garanti pour tous les usages autres que celui prévu à la base; renvoie la somme des elements du tableau

char algoGlouton(char sommeMax, char retour[5], char etape);        //ne fonctionne pas, à voir pour améliorer/fixe;

char genMobs(char numeroSalle, char retour[5]);     //generation des mobs pour une salle; fonction temporaire : en attendant d'etre remplacée par la solution agloGlouton

char mobsShuffle(char mobs[5]);     //Melange la place des mobs dans le tableau

double rad(int degre);      //renvoie l'angle en radian

char racineNieme(char nMob, char numeroSalle, short retour[nMob]);      //renvoie la position des mobs selon un cercle

char posInPosTab(short pos, char taille, short posTab[taille]);     //renvoie si la position est dans le tableau des positions

char placer1Mob(char salle[21*15], short placement, char taille, short mobsPos[taille], char indice);

char allocEspaceMobTab(char ** mob, char taille);       //alloue l'espace demandé pour le tableau de mob; Il ne faudra pas oublier de free l'espace apres utilisation

char allocEspaceMobPosTab(short ** mobPos, char taille);        //alloue l'espace demandé pour le tableau des positions des mobs; Il ne faudra pas oublier de free l'espace apres utilisation   

char creerSalle(char identifiantSalle, char etage);     //identifiantSalle est l'identifiant de la salle, pas sa position ni son indice dans map; creer la salle demandée

char creerSave();       //creer le folder de la save donnée

char creerEtageEntier(char numEtage);       //creer un etage entier

char supprimerEtage();      //supprime toutes les données de l'étage sauf la seed

char supprimerSave();       //supprime la save en definie