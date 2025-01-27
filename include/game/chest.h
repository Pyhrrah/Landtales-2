#ifndef CHEST_H
#define CHEST_H


#include <stdio.h>
#include <stdlib.h>
#include "./../../include/core/player.h"

#define TILE_SIZE 32
#define ROWS 15
#define COLS 21

int checkChestCollision(Player* player, int mapRoom[ROWS][COLS], const char *filename);
int checkChestDirection(Player *player, int mapRoom[ROWS][COLS], int playerX, int playerY);


void updateMapFile(const char *filename);


#endif // CHEST_H
