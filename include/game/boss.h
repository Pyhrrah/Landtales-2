#ifndef BOSS_HANDLER_H
#define BOSS_HANDLER_H

#include <SDL2/SDL.h>
#include "./../../include/core/ennemies.h"
#include "./../../include/core/player.h"
#include "./../../include/game/projectile.h"


#define ROWS 15         // Nombre de lignes dans la grille de la salle.
#define COLS 21         // Nombre de colonnes dans la grille de la salle.
#define TILE_SIZE 32    // Taille d'une tuile en pixels.


// Externes
extern Enemy enemies[];    // Tableau global des ennemis.
extern int enemyCount;     // Nombre total d'ennemis actifs.

/**
 * Structure représentant un boss.
 * @param ennemyCount : nombre d'ennemis dans la salle.
 * @param type : type de boss.
 */
int isBossAlive(int enemyCount, int type);

/**
 * Initialise le boss.
 */
void initBigBoss();

/**
 * Vérifie et active la porte de boss.
 */
void checkAndActivateBossDoor(int room, int enemyCount);

/**
 * Gère les collisions avec la porte pour accéder au boss.
 * @param renderer : le renderer.
 * @param playerRect : le rectangle du joueur.
 * @param room : la salle actuelle.
 * @param mapFilename : le nom du fichier de la carte.
 * @param mapRoom : la carte de la salle.
 * @param saveNumber : le numéro de la sauvegarde.
 * @param tentative : le numéro de tentative.
 * @param etage : l'étage actuel.
 * @param player : le joueur.
 * @param mapData : la carte.
 * @param stageFilename : le nom du fichier de l'étage.
 */
void handleBossDoorCollision(SDL_Renderer *renderer, SDL_Rect *playerRect, 
                             int *room, char *mapFilename, int mapRoom[ROWS][COLS], int saveNumber, 
                             int tentative, int *etage, Player *player, int mapData[11][11], char *stageFilename);

/**
 *  Initialise les projectiles du boss.
 */
void initProjectilesBoss();

/**
 * Lance un projectile avec un cooldown pour le boss.
 * @param bossRect : le rectangle du boss.
 * @param playerRect : le rectangle du joueur.
 */
void launchProjectileWithCooldownBoss(SDL_Rect *bossRect, SDL_Rect *playerRect);

/**
 * Gère les projectiles du boss.
 * @param renderer : le renderer.
 * @param mapRoom : la carte de la salle.  
 * @param player : le joueur.
 * @param projectileTexture : la texture du projectile.
 */
void handleProjectilesBoss(SDL_Renderer *renderer, int mapRoom[ROWS][COLS], Player *player, SDL_Texture *projectileTexture);


#endif // BOSS_HANDLER_H
