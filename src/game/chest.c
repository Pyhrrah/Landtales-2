#include <stdio.h>
#include <stdlib.h>
#include "./../../include/core/player.h"

#define TILE_SIZE 32
#define ROWS 15
#define COLS 21


/*

Fichier chest.c, ce dernier contient les fonctions pour gérer les coffres du jeu (ouverture, collision, etc.)
 */

// Fonction pour vérifier la direction du coffre
int checkChestDirection(Player *player, int mapRoom[ROWS][COLS], int playerX, int playerY) {

    // Vérification de la direction du joueur

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

// Fonction pour mettre à jour le fichier de la carte
void updateMapFile(const char *filename) {

    // Ouverture du fichier
    printf("Ouverture du fichier de stage. %s\n", filename);
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        printf("Erreur d'ouverture du fichier de stage.\n");
        return;
    }

    // Lecture des données
    int x, y, tileValue;
    int data[1000][3];  
    int count = 0;

    // Récupération des données
    while (fscanf(file, "%d %d %d", &x, &y, &tileValue) != EOF) {
        data[count][0] = x;
        data[count][1] = y;
        data[count][2] = tileValue;
        count++;
    }

    fclose(file);  

    // Modification de la valeur du coffre
    for (int i = 0; i < count; i++) {
        if (data[i][2] == 19) {
            data[i][2] = 1;  
        }
    }

    // Réécriture des données
    file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erreur d'ouverture du fichier pour écriture.\n");
        return;
    }

    // Réécriture des données
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d %d %d\n", data[i][0], data[i][1], data[i][2]);
    }

    printf("Valeur de la case modifiée. %s\n", filename);

    fclose(file);  
}

int checkChestCollision(Player* player, int mapRoom[ROWS][COLS], int saveNumber, int room) {
    int tileX = player->rect.x / TILE_SIZE;
    int tileY = player->rect.y / TILE_SIZE;

    // Concaténation du nom du fichier
    char filename[100];
    snprintf(filename, sizeof(filename), "./data/game/save%d/map/Salle%02d/salle%02d.txt", saveNumber, room, room);

    // Vérification de la collision
    if (tileX >= 0 && tileX < COLS && tileY >= 0 && tileY < ROWS) {
        if (checkChestDirection(player, mapRoom, tileX, tileY)) {
            updateMapFile(filename);  
            return 1;  
        }
    }

    return 0;  
}


