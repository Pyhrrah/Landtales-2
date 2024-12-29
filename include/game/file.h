#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

// Taille d'une case (tile) en pixels
#define TILE_SIZE 32

// Dimensions de la grille de la carte
#define ROWS 15  // Nombre de lignes
#define COLS 21  // Nombre de colonnes

/**
 * Charge une carte depuis un fichier et remplit les données correspondantes.
 * 
 * @param filename : Chemin vers le fichier contenant les données de la carte.
 * @param data : Tableau 2D pour stocker les données de la carte.
 * @return 1 si le chargement est réussi, 0 en cas d'échec.
 */
int loadMap(const char* filename, int data[11][11]);

/**
 * Dessine une carte à l'écran.
 * 
 * @param room : Tableau 2D représentant la carte actuelle.
 * @param renderer : Renderer SDL utilisé pour dessiner la carte.
 */
void drawMap(int room[ROWS][COLS], SDL_Renderer *renderer);

/**
 * Charge les données d'une salle depuis un fichier.
 * 
 * @param filename : Chemin vers le fichier contenant les données de la salle.
 * @param room : Tableau 2D pour stocker les données de la salle.
 */
void loadRoomData(const char* filename, int room[ROWS][COLS]);

/**
 * Sauvegarde l'état actuel de la partie.
 * 
 * @param saveNumber : Numéro de la sauvegarde à mettre à jour.
 * @param tentative : Nombre de tentatives effectuées par le joueur.
 * @param vie : Vie actuelle du joueur.
 * @param attaque : Niveau d'attaque du joueur.
 * @param defense : Niveau de défense du joueur.
 * @param etage : Numéro de l'étage actuel.
 * @param piece : Montant d'argent (pièces) collecté par le joueur.
 * @param room : Numéro de la salle actuelle.
 * @param max_vie : Niveau maximum de vie du joueur.
 */
void saveGame(int saveNumber, int tentative, int vie, int attaque, int defense, int etage, int piece, int room, int max_vie);

/**
 * Efface les fichiers liés à une carte pour un numéro de sauvegarde donné.
 * 
 * @param saveNumber : Numéro de la sauvegarde dont les fichiers doivent être supprimés.
 */
void clearMapDirectory(int saveNumber);

/**
 * Charge les données de la carte et de la salle à partir d'une sauvegarde.
 * 
 * @param saveNumber : Numéro de la sauvegarde à charger.
 * @param room : Salle actuelle à charger.
 * @param dataMap : Tableau 2D pour stocker les données de la carte.
 * @param mapRoom : Tableau 2D pour stocker les données de la salle.
 */
void loadMapAndRoom(int saveNumber, int room, int dataMap[11][11], int mapRoom[ROWS][COLS]);

/**
 * Charge les données du joueur à partir d'une sauvegarde.
 * 
 * @param saveNumber : Numéro de la sauvegarde à charger.
 * @param tentative : Pointeur pour stocker le nombre de tentatives du joueur.
 * @param vie : Pointeur pour stocker la vie actuelle du joueur.
 * @param attaque : Pointeur pour stocker le niveau d'attaque du joueur.
 * @param defense : Pointeur pour stocker le niveau de défense du joueur.
 * @param etage : Pointeur pour stocker le numéro de l'étage actuel.
 * @param piece : Pointeur pour stocker le montant d'argent (pièces) collecté.
 * @param room : Pointeur pour stocker le numéro de la salle actuelle.
 */
void loadPlayerData(int saveNumber, int *tentative, int *vie, int *attaque, int *defense, int *etage, int *piece, int *room);

#endif // GAME_H
