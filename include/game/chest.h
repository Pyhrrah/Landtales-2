#ifndef CHEST_H
#define CHEST_H

#include <stdio.h>
#include <stdlib.h>
#include "./../../include/core/player.h"

#define TILE_SIZE 32
#define ROWS 15
#define COLS 21

/**
 * Vérifie si le joueur est en collision avec un coffre
 * @param player : le joueur
 * @param mapRoom : la map
 * @param filename : le nom du fichier de la map
 */
int checkChestCollision(Player* player, int mapRoom[ROWS][COLS], int saveNumber, int room);

/**
 * Vérifie si le joueur est en collision avec un coffre
 * @param player : le joueur
 * @param mapRoom : la map
 * @param playerX : la position en x du joueur
 * @param playerY : la position en y du joueur
 */
int checkChestDirection(Player *player, int mapRoom[ROWS][COLS], int playerX, int playerY);

/**
* Update les fichiers de la map une fois le coffre ouvert
* @param filename : le nom du fichier de la map
*/
void updateMapFile(const char *filename);


#endif // CHEST_H
