#include <stdio.h>
#include <stdlib.h>
#include "./../../include/core/player.h"

#define TILE_SIZE 32
#define ROWS 15
#define COLS 21


int checkChestDirection(Player *player, int mapRoom[ROWS][COLS], int playerX, int playerY) {
    if (player->orientation == 'D') {  
        if (playerY + 1 < ROWS && mapRoom[playerY + 1][playerX] == 19) { 
            mapRoom[playerY + 1][playerX] = 1; 
            return 1;  
        }
    }
    else if (player->orientation == 'U') {  
        if (playerY - 1 >= 0 && mapRoom[playerY - 1][playerX] == 19) {
            mapRoom[playerY - 1][playerX] = 1; 
            return 1; 
        }
    }
    else if (player->orientation == 'L') { 
        if (playerX - 1 >= 0 && mapRoom[playerY][playerX - 1] == 19) {
            mapRoom[playerY][playerX - 1] = 1; 
            return 1;  
        }
    }
    else if (player->orientation == 'R') {  
        if (playerX + 1 < COLS && mapRoom[playerY][playerX + 1] == 19) {  
            mapRoom[playerY][playerX + 1] = 1;  
            return 1; 
        }
    }

    return 0;  
}

void updateMapFile(const char *filename) {
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        printf("Erreur d'ouverture du fichier de stage.\n");
        return;
    }

    int x, y, tileValue;
    int data[1000][3];  
    int count = 0;

    while (fscanf(file, "%d %d %d", &x, &y, &tileValue) != EOF) {
        data[count][0] = x;
        data[count][1] = y;
        data[count][2] = tileValue;
        count++;
    }

    fclose(file);  

    for (int i = 0; i < count; i++) {
        if (data[i][2] == 19) {
            data[i][2] = 1;  
        }
    }

    file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erreur d'ouverture du fichier pour écriture.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%d %d %d\n", data[i][0], data[i][1], data[i][2]);
    }

    printf("Valeur de la case modifiée. %s\n", filename);

    fclose(file);  
}

int checkChestCollision(Player* player, int mapRoom[ROWS][COLS], const char *filename) {
    int tileX = player->rect.x / TILE_SIZE;
    int tileY = player->rect.y / TILE_SIZE;

    if (tileX >= 0 && tileX < COLS && tileY >= 0 && tileY < ROWS) {
        if (checkChestDirection(player, mapRoom, tileX, tileY)) {
            updateMapFile(filename);  
            return 1;  
        }
    }

    return 0;  
}


