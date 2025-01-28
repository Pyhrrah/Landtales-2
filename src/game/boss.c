#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <time.h>
#include "./../../include/core/ennemies.h"
#include "./../../include/core/player.h"
#include "./../../include/game/file.h"
#include "./../../include/game/map.h"
#include "./../../include/game/boss.h"

#define ROWS 15
#define COLS 21
#define TILE_SIZE 32
#define MAX_PROJECTILES 100

extern Enemy enemies[];
extern int enemyCount;

SDL_Rect bossDoor = {0, 0, 0, 0};

// Structure pour les projectiles

Projectile projectiles[MAX_PROJECTILES];

void initProjectilesBoss() {
    printf("Initialisation des projectiles pour le boss final.\n");
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = 0;
    }
}

void spawnProjectileBoss(SDL_Rect *bossRect, SDL_Rect *playerRect) {
    printf("Projectile lancé par le boss.\n");
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) {
            projectiles[i].rect.x = bossRect->x + bossRect->w / 2;
            projectiles[i].rect.y = bossRect->y + bossRect->h / 2;
            projectiles[i].rect.w = 16;
            projectiles[i].rect.h = 16;
            
            float angle = atan2(playerRect->y - projectiles[i].rect.y, playerRect->x - projectiles[i].rect.x);
            projectiles[i].dx = cos(angle) * 5;
            projectiles[i].dy = sin(angle) * 5;

            projectiles[i].active = 1;
            break;
        }
    }
}

void updateProjectilesBoss(int mapRoom[ROWS][COLS]) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            projectiles[i].rect.x += (int)projectiles[i].dx;
            projectiles[i].rect.y += (int)projectiles[i].dy;

            // Vérifier si le projectile sort de l'écran
            if (projectiles[i].rect.x < 0 || projectiles[i].rect.y < 0 ||
                projectiles[i].rect.x >= COLS * TILE_SIZE || projectiles[i].rect.y >= ROWS * TILE_SIZE) {
                projectiles[i].active = 0;
                continue;
            }

            // Vérifier les collisions avec les obstacles
            int gridX = projectiles[i].rect.x / TILE_SIZE;
            int gridY = projectiles[i].rect.y / TILE_SIZE;

            if (mapRoom[gridY][gridX] == 9) { // Exemple : l'ID 9 casse le projectile
                projectiles[i].active = 0;
            }
        }
    }
}

void checkProjectileCollisionsBoss(Player *player) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active && SDL_HasIntersection(&projectiles[i].rect, &player->rect)) {
            projectiles[i].active = 0;
            player->vie -= 10; // Réduit la vie du joueur de 10
            printf("Projectile touche le joueur ! Vie restante : %d\n", player->vie);
        }
    }
}

void renderProjectilesBoss(SDL_Renderer *renderer) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge pour les projectiles
            SDL_RenderFillRect(renderer, &projectiles[i].rect);
        }
    }
}

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
    enemies[0].rect.x = 320 - 32;
    enemies[0].rect.y = 224 - 32;
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

void launchProjectileWithCooldownBoss(SDL_Rect *bossRect, SDL_Rect *playerRect) {
    static Uint32 lastProjectileTime = 0;
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime > lastProjectileTime + 3000) { // 3 secondes d'intervalle
        spawnProjectileBoss(bossRect, playerRect);
        lastProjectileTime = currentTime;
    }
}

void handleProjectilesBoss(SDL_Renderer *renderer, int mapRoom[ROWS][COLS], Player *player) {
    printf("Gestion des projectiles du boss.\n");
    // Mettre à jour les projectiles
    updateProjectilesBoss(mapRoom);

    // Vérifier les collisions entre les projectiles et le joueur
    checkProjectileCollisionsBoss(player);

    // Afficher les projectiles à l'écran
    renderProjectilesBoss(renderer);
}