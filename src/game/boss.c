#include "./../../include/core/ennemies.h"
#include "./../../include/core/player.h"
#include "./../../include/game/file.h"
#include "./../../include/game/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define ROWS 15
#define COLS 21
#define TILE_SIZE 32

extern Enemy enemies[];
extern int enemyCount;

SDL_Rect bossDoor = {0, 0, 0, 0};

int isBossAlive(int enemyCount, int type) {
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].type == type) { 
            return 1;
        }
    }
    return 0;
}

void initBigBoss() {
    clearEnemies();
    enemies[0].rect.x = 320-32;
    enemies[0].rect.y = 224-32;
    enemies[0].rect.w = 64;
    enemies[0].rect.h = 64;
    enemies[0].vie = 500;
    enemies[0].attaque = 20;
    enemies[0].defense = 10;
    enemies[0].type = 100;
    enemies[0].id = 0;
    enemyCount = 1;
}

void checkAndActivateBossDoor(int room, int enemyCount) {
    if (room == 2) {
        if (isBossAlive(enemyCount, 1) != 1) {
            bossDoor.x = 320;
            bossDoor.y = 160;
            bossDoor.w = TILE_SIZE;
            bossDoor.h = TILE_SIZE;
        }
    }
}

void handleBossDoorCollision(SDL_Renderer *renderer, SDL_Rect *playerRect, 
                             int *room, char *mapFilename, int mapRoom[ROWS][COLS], int saveNumber, 
                             int tentative, int *etage, Player *player, int mapData[11][11], char *stageFilename) {
    static SDL_Texture *bossDoorTexture = NULL;

    if (bossDoorTexture == NULL) {
        bossDoorTexture = IMG_LoadTexture(renderer, "./assets/images/sprite/map/spawn1.png");
        if (!bossDoorTexture) {
            printf("Erreur lors du chargement de l'image de la porte du boss : %s\n", IMG_GetError());
            return;
        }
    }

    if (bossDoor.w > 0 && bossDoor.h > 0) {
        SDL_RenderCopy(renderer, bossDoorTexture, NULL, &bossDoor);

        if (SDL_HasIntersection(playerRect, &bossDoor)) {
            printf("Joueur entre dans la porte, retour au lobby.\n");
            
            *room = 0;
            sprintf(mapFilename, "./data/game/lobbyMap.txt");
            loadRoomData(mapFilename, mapRoom);

            (*etage) += 1;

            bossDoor.w = 0;
            bossDoor.h = 0;

            clearEnemies();

            clearMapDirectory(saveNumber);

            if (*etage != 4) {
                creerEtageEntier(*etage);
                loadMap(stageFilename, mapData);
            }

            saveGame(saveNumber, tentative, player->vie, player->attaque, player->defense, *etage, player->argent, *room, player->max_vie);
        }
    }
}
