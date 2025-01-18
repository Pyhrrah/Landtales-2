#include <SDL2/SDL.h>
#include <time.h>
#include "./../../include/core/ennemies.h"

#define TILE_SIZE 32
#define ROWS 15
#define COLS 21

typedef struct {
    SDL_Rect rect;
    char orientation;
    int active; // Remplacement de bool par int
} Arrow;

// Structure pour le joueur
typedef struct {
    SDL_Rect rect;    
    int max_vie;          
    int vie;                    
    int attaque;                
    int defense;                
    char orientation;           
    int id;                     
    int regen_dispo; // Remplacement de bool par int       
    time_t last_regen;          
    int argent;  
    time_t last_lightning;    
    Arrow arrows[3];
    int arrowCount;
    Uint32 arrowTimestamps[3];           
} Player;

// Structure pour les attaques
typedef struct {
    SDL_Rect rect;
} Attack;

// Initialisation du joueur
void initPlayer(Player *player, int x, int y, int w, int h, int vie, int argent, int attaque, int defense, int max_vie) {
    player->rect.x = x;
    player->rect.y = y;
    player->rect.w = w;
    player->rect.h = h;
    player->max_vie = max_vie;
    player->vie = vie;          
    player->attaque = attaque;      
    player->defense = defense;      
    player->orientation = 'B';  
    player->id = 1;             
    player->regen_dispo = 1; // Initialisation à vrai      
    player->last_regen = 0; 
    player->argent = argent;     
    player->last_lightning = 0;    
    player->arrowCount = 3;
    for (int i = 0; i < player->arrowCount; i++) {
        player->arrows[i].active = 0; // Initialisation à faux
        player->arrowTimestamps[i] = 0;
    }
}

// Vérifier la collision du joueur avec les bonus
void checkBonusCollision(Player *player) {
    for (int i = 0; i < 5; i++) {
        if (bonuses[i].active && SDL_HasIntersection(&player->rect, &bonuses[i].rect)) {
            if (bonuses[i].type == 0) { 

                // ici on regarde     
                if (player->max_vie - player->vie <= 5) {
                    player->vie = player->max_vie;
                } else {
                    player->vie += 5;
                    printf("Le joueur récupère 5 PV. Vie actuelle : %d\n", player->vie);
                }
            } else if (bonuses[i].type == 1) { 
                player->argent += 5;
                printf("Le joueur récupère 1 pièce. Total d'argent : %d\n", player->argent);
            }

            bonuses[i].active = 0;  
        }
    }
}

// Fonction d'attaque à l'épée
void attackWithSword(SDL_Renderer *renderer, Player *player, char orientation, int *enemyCount, const char *enemyFilename) {
    SDL_Rect swordRect;
    swordRect.w = 8;
    swordRect.h = 32;

    switch (orientation) {
        case 'U':  
            swordRect.x = player->rect.x + (player->rect.w / 2) - (swordRect.w / 2);
            swordRect.y = player->rect.y - swordRect.h;  
            break;
        case 'D':  
            swordRect.x = player->rect.x + (player->rect.w / 2) - (swordRect.w / 2);
            swordRect.y = player->rect.y + player->rect.h; 
            break;
        case 'L':  
            swordRect.w = 32; 
            swordRect.h = 8; 
            swordRect.x = player->rect.x - swordRect.w;  
            swordRect.y = player->rect.y + (player->rect.h / 2) - (swordRect.h / 2);
            break;
        case 'R':  
            swordRect.w = 32;  
            swordRect.h = 8; 
            swordRect.x = player->rect.x + player->rect.w;  
            swordRect.y = player->rect.y + (player->rect.h / 2) - (swordRect.h / 2);
            break;
        default:
            printf("Orientation invalide\n");
            return;  
    }

    if (SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255) == 1) {
        printf("Erreur lors du changement de couleur : %s\n", SDL_GetError());
    }
    if (SDL_RenderFillRect(renderer, &swordRect) == 1) {
        printf("Erreur lors du rendu du rectangle : %s\n", SDL_GetError());
    }

    for (int i = 0; i < *enemyCount; i++) {
        if (SDL_HasIntersection(&swordRect, &enemies[i].rect)) {
            enemies[i].vie -= 10; 
            printf("Ennemi %d touché ! Vie restante : %d\n", i, enemies[i].vie);

            if (enemies[i].vie <= 0) {
                removeEnemy(enemies, enemyCount, i, enemyFilename); 
                i--;
            }
        }
    }
}

void shootArrow(Player *player) {
    Uint32 currentTime = SDL_GetTicks();
    for (int i = 0; i < 3; i++) {
        if (!player->arrows[i].active && currentTime - player->arrowTimestamps[i] >= 10000) {
            player->arrows[i].active = 1; // Activation de la flèche
            player->arrows[i].orientation = player->orientation;
            player->arrows[i].rect.w = 8;
            player->arrows[i].rect.h = 8;
            switch (player->orientation) {
                case 'U':
                    player->arrows[i].rect.x = player->rect.x + (player->rect.w / 2) - (player->arrows[i].rect.w / 2);
                    player->arrows[i].rect.y = player->rect.y - player->arrows[i].rect.h;
                    break;
                case 'D':
                    player->arrows[i].rect.x = player->rect.x + (player->rect.w / 2) - (player->arrows[i].rect.w / 2);
                    player->arrows[i].rect.y = player->rect.y + player->rect.h;
                    break;
                case 'L':
                    player->arrows[i].rect.x = player->rect.x - player->arrows[i].rect.w;
                    player->arrows[i].rect.y = player->rect.y + (player->rect.h / 2) - (player->arrows[i].rect.h / 2);
                    break;
                case 'R':
                    player->arrows[i].rect.x = player->rect.x + player->rect.w;
                    player->arrows[i].rect.y = player->rect.y + (player->rect.h / 2) - (player->arrows[i].rect.h / 2);
                    break;
            }
            player->arrowCount++;
            player->arrowTimestamps[i] = currentTime;
            break;
        }
    }
}

// Autres fonctions restent inchangées, sauf conversion de `bool` en `int`...
// Mettre à jour les flèches
void updateArrows(Player *player, int mapRoom[ROWS][COLS], int *enemyCount, const char *enemyFilename) {
    for (int i = 0; i < 3; i++) {
        if (player->arrows[i].active) {
            switch (player->arrows[i].orientation) {
                case 'U':
                    player->arrows[i].rect.y -= TILE_SIZE / 2;
                    break;
                case 'D':
                    player->arrows[i].rect.y += TILE_SIZE / 2;
                    break;
                case 'L':
                    player->arrows[i].rect.x -= TILE_SIZE / 2;
                    break;
                case 'R':
                    player->arrows[i].rect.x += TILE_SIZE / 2;
                    break;
            }

            // Vérifier les collisions avec les murs
            int tileX = player->arrows[i].rect.x / TILE_SIZE;
            int tileY = player->arrows[i].rect.y / TILE_SIZE;
            if (tileX >= 0 && tileX < COLS && tileY >= 0 && tileY < ROWS) {
                int tileId = mapRoom[tileY][tileX];
                if (tileId == 5 || tileId == 6 || tileId == 7 || 
                    tileId == 12 || tileId == 14 || 
                    tileId == 15 || tileId == 16 || tileId == 17 || tileId == 18) {
                    player->arrows[i].active = 0;  // Désactivation de la flèche
                    player->arrowCount--;
                    player->arrowTimestamps[i] = SDL_GetTicks();
                    continue;
                }
            }

            // Vérifier les collisions avec les ennemis
            for (int j = 0; j < *enemyCount; j++) {
                if (SDL_HasIntersection(&player->arrows[i].rect, &enemies[j].rect)) {
                    enemies[j].vie -= 10;
                    printf("Ennemi %d touché par une flèche ! Vie restante : %d\n", j, enemies[j].vie);
                    player->arrows[i].active = 0;
                    player->arrowCount--;
                    player->arrowTimestamps[i] = SDL_GetTicks();

                    if (enemies[j].vie <= 0) {
                        removeEnemy(enemies, enemyCount, j, enemyFilename); 
                        j--;
                    }
                    break;
                }
            }
        }
    }
}

// Rendu des flèches
void renderArrows(SDL_Renderer *renderer, Player *player) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Jaune pour les flèches
    for (int i = 0; i < 3; i++) {
        if (player->arrows[i].active) {
            SDL_RenderFillRect(renderer, &player->arrows[i].rect);
        }
    }
}

int areArrowsActive(Player *player) {
    for (int i = 0; i < player->arrowCount; i++) {
        if (player->arrows[i].active) {
            return 1; // Si une flèche est active
        }
    }
    return 0; // Si aucune flèche n'est active
}

// Fonction de régénération du joueur
void regeneratePlayer(Player *player) {
    time_t currentTime = time(NULL);
    double secondsSinceLastRegen = difftime(currentTime, player->last_regen);
    
    if (secondsSinceLastRegen >= 180) {
        int regenAmount = player->vie / 4;
        player->vie += regenAmount;

        if (player->vie > 100) {
            player->vie = 100;
        }

        player->last_regen = currentTime;
        
        printf("Régénération effectuée : +%d PV\n", regenAmount);
    } else {
        int timeLeft = 180 - (int)secondsSinceLastRegen;
        printf("Temps restant avant la régénération : %d secondes\n", timeLeft);
    }
}

// Fonction pour créer une foudre à la position de la souris
void createLightningAtMouse(SDL_Renderer *renderer, int *enemyCount, const char *enemyFilename) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    SDL_Rect lightningRect = {mouseX - 20, mouseY - 20, 40, 40};  
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  
    SDL_RenderFillRect(renderer, &lightningRect);

    for (int i = 0; i < *enemyCount; i++) {
            if (SDL_HasIntersection(&lightningRect, &enemies[i].rect)) {
                enemies[i].vie -= 30; 
                printf("Ennemi %d touché ! Vie restante : %d\n", i, enemies[i].vie);

                if (enemies[i].vie <= 0) {
                    removeEnemy(enemies, enemyCount, i, enemyFilename); 
                    i--;
                }
            }
        }
}

// Fonction pour utiliser la foudre
void useLightning(SDL_Renderer *renderer, Player *player, int *enemyCount, const char *enemyFilename) {
    time_t currentTime = time(NULL);
    double secondsSinceLastLightning = difftime(currentTime, player->last_lightning);
    
    if (secondsSinceLastLightning >= 120) {
        createLightningAtMouse(renderer, enemyCount, enemyFilename);  
        
        player->last_lightning = currentTime;

        printf("Foudre utilisée !\n");
    } else {
        int timeLeft = 120 - (int)secondsSinceLastLightning;
        printf("Temps restant avant la réutilisation de la foudre : %d secondes\n", timeLeft);
    }
}

int getLightningCooldown(Player *player) {
    time_t currentTime = time(NULL);
    int cooldown = 120 - difftime(currentTime, player->last_lightning);
    return (cooldown > 0) ? cooldown : 0;
}

int getRegenCooldown(Player *player) {
    time_t currentTime = time(NULL);
    int cooldown = 180 - difftime(currentTime, player->last_regen);
    return (cooldown > 0) ? cooldown : 0;
}
