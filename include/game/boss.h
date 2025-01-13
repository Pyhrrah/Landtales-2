#ifndef BOSS_HANDLER_H
#define BOSS_HANDLER_H

#include <SDL2/SDL.h>
#include "./../../include/core/ennemies.h"
#include "./../../include/core/player.h"

#define ROWS 15         // Nombre de lignes dans la grille de la salle.
#define COLS 21         // Nombre de colonnes dans la grille de la salle.
#define TILE_SIZE 32    // Taille d'une tuile en pixels.

// Externes
extern Enemy enemies[];    // Tableau global des ennemis.
extern int enemyCount;     // Nombre total d'ennemis actifs.

// Déclare si un boss spécifique est toujours en vie.
// enemyCount : nombre total d'ennemis actifs.
// type : type spécifique d'ennemi à vérifier.
// Retourne : true si l'ennemi du type donné est en vie, sinon false.
int isBossAlive(int enemyCount, int type);

// Initialise les données du Big Boss pour le combat final.
// Configure la position, les statistiques et les attributs du boss.
void initBigBoss();

// Active une porte de boss lorsque les conditions sont remplies.
// room : identifiant de la salle actuelle.
// enemyCount : nombre total d'ennemis actifs.
void checkAndActivateBossDoor(int room, int enemyCount);

// Gère la collision entre le joueur et la porte de boss.
// renderer : le renderer SDL utilisé pour dessiner.
// playerRect : rectangle représentant le joueur.
// room : identifiant de la salle actuelle (sera modifié).
// mapFilename : chemin du fichier de la carte (sera modifié).
// mapRoom : tableau représentant la salle actuelle (sera modifié).
// saveNumber : identifiant de la sauvegarde actuelle.
// tentative : nombre de tentatives du joueur.
// etage : étage actuel du joueur (sera modifié).
// player : structure représentant le joueur.
// mapData : données de la carte de l'étage (sera modifié).
// stageFilename : chemin du fichier contenant les données d'étage.
void handleBossDoorCollision(SDL_Renderer *renderer, SDL_Rect *playerRect, 
                             int *room, char *mapFilename, int mapRoom[ROWS][COLS], int saveNumber, 
                             int tentative, int *etage, Player *player, int mapData[11][11], char *stageFilename);

#endif // BOSS_HANDLER_H
