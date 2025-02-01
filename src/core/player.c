#include <SDL2/SDL.h>
#include <time.h>
#include "./../../include/core/ennemies.h"
#include "./../../include/core/player.h"
#include "./../../include/utils/video.h"

#define TILE_SIZE 32
#define ROWS 15
#define COLS 21

Lightning lightning = {0};
RegenAnimation regenAnimation = {0};


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
    player->orientation = 'D';
    player->id = 1;
    player->regen_dispo = 1;
    player->argent = argent;
    player->arrowCount = 3;
    for (int i = 0; i < player->arrowCount; i++) {
        player->arrows[i].active = 0;
        player->arrowTimestamps[i] = 0;
    }
    player->last_regen = 0;
    player->last_lightning = 0;
}




// Fonction pour calculer les dégâts en fonction du type d'arme
int calculateDamage(int playerAttack, int ennemyDefense, ArmeType type) {
    int damage;

    // Appliquer la défense de l'ennemi selon le type d'arme
    switch (type) {
        case EPEE:
            damage = playerAttack - ennemyDefense;  // Défense normale pour l'épée
            break;
        case ARC:
            damage = playerAttack - (ennemyDefense / 2);  // Moins de défense contre l'arc
            break;
        case FOUDRE:
            damage = playerAttack;  // Pas de défense contre la foudre
            break;
        default:
            damage = playerAttack - ennemyDefense;  // Défense normale par défaut
            break;
    }

    if (damage < 0) {
        damage = 0;  // Pas de dégâts négatifs
    }

    // Ajouter un facteur de variation aléatoire entre -5 et +5
    damage += (rand() % 11) - 5;

    if (damage < 0) {
        damage = 0;  // Assurer que les dégâts ne soient pas négatifs
    }

    // Ajouter des bonus en fonction du type d'arme
    switch (type) {
        case EPEE:
            damage += 5;  // +5 pour l'épée
            break;
        case ARC:
            damage += 2;  // +2 pour l'arc
            break;
        case FOUDRE:
            damage += 10; // +10 pour la foudre
            break;
        default:
            break;
    }

    return damage;
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
            int damageToEnemy = calculateDamage(player->attaque, enemies[i].defense, EPEE);

            enemies[i].vie -= damageToEnemy; 
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

            int tileX = player->arrows[i].rect.x / TILE_SIZE;
            int tileY = player->arrows[i].rect.y / TILE_SIZE;
            if (tileX >= 0 && tileX < COLS && tileY >= 0 && tileY < ROWS) {
                int tileId = mapRoom[tileY][tileX];
                if (tileId == 5 || tileId == 6 || tileId == 7 || 
                    tileId == 12 || tileId == 14 || 
                    tileId == 15 || tileId == 16 || tileId == 17 || tileId == 18 || tileId == 19) {
                    player->arrows[i].active = 0; 
                    player->arrowCount--;
                    player->arrowTimestamps[i] = SDL_GetTicks();
                    continue;
                }
            }

            for (int j = 0; j < *enemyCount; j++) {
                if (SDL_HasIntersection(&player->arrows[i].rect, &enemies[j].rect)) {

                    int damageToEnemy = calculateDamage(player->attaque, enemies[j].defense, ARC);

                    enemies[j].vie -= damageToEnemy;
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
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    for (int i = 0; i < 3; i++) {
        if (player->arrows[i].active) {
            SDL_RenderFillRect(renderer, &player->arrows[i].rect);
        }
    }
}

int areArrowsActive(Player *player) {
    for (int i = 0; i < player->arrowCount; i++) {
        if (player->arrows[i].active) {
            return 1; 
        }
    }
    return 0;
}

// Fonction de régénération du joueur
void regeneratePlayer(Player *player) {
    time_t currentTime = time(NULL);
    double secondsSinceLastRegen = difftime(currentTime, player->last_regen);
    
    if (secondsSinceLastRegen >= 180) {
        int regenAmount = player->vie / 4;
        player->vie += regenAmount;
        if (player->vie > player->max_vie) {
            player->vie = player->max_vie;
        }

        regenAnimation.framesLeft = 10;  
        regenAnimation.active = 1;

        for (int i = 0; i < 20; i++) {
            regenAnimation.offsets[i].x = (rand() % 42) - 21; 
            regenAnimation.offsets[i].y = (rand() % 42) - 21; 

            regenAnimation.rects[i] = (SDL_Rect){
                0, 0, 5, 5 
            };
        }
        playSong("./assets/music/sons/regen.mp3"); 
        player->last_regen = currentTime;
        printf("Régénération effectuée : +%d PV\n", regenAmount);
    } else {
        int timeLeft = 180 - (int)secondsSinceLastRegen;
        printf("Temps restant avant la régénération : %d secondes\n", timeLeft);
    }
}


void updateAndRenderRegenAnimation(SDL_Renderer *renderer, Player *player) {
    if (regenAnimation.active) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
        
        for (int i = 0; i < 20; i++) {
            int playerCenterX = player->rect.x + player->rect.w / 2;
            int playerCenterY = player->rect.y + player->rect.h / 2;

            regenAnimation.rects[i].x = playerCenterX + regenAnimation.offsets[i].x;
            regenAnimation.rects[i].y = playerCenterY + regenAnimation.offsets[i].y;

            SDL_RenderFillRect(renderer, &regenAnimation.rects[i]);
        }

        regenAnimation.framesLeft--;

        if (regenAnimation.framesLeft <= 0) {
            regenAnimation.active = 0;
        }
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

                int damageToEnemy = calculateDamage(30, enemies[i].defense, FOUDRE);
                enemies[i].vie -= damageToEnemy; 
                printf("Ennemi %d touché ! Vie restante : %d\n", i, enemies[i].vie);

                if (enemies[i].vie <= 0) {
                    removeEnemy(enemies, enemyCount, i, enemyFilename); 
                    i--;
                }
            }
        }
}

// Fonction pour utiliser la foudre
void useLightning(Player *player, int *enemyCount, const char *enemyFilename) {
    time_t currentTime = time(NULL);
    double secondsSinceLastLightning = difftime(currentTime, player->last_lightning);
    
    if (secondsSinceLastLightning >= 120) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        lightning.rect = (SDL_Rect){mouseX - 20, mouseY - 20, 40, 40};
        lightning.framesLeft = 10; 
        lightning.active = 1; 
        playSong("./assets/music/sons/foudre.mp3");     

        for (int i = 0; i < *enemyCount; i++) {
            if (SDL_HasIntersection(&lightning.rect, &enemies[i].rect)) {
                int damageToEnemy = calculateDamage(30, enemies[i].defense, FOUDRE);
                enemies[i].vie -= damageToEnemy;
                printf("Ennemi %d touché ! Vie restante : %d\n", i, enemies[i].vie);

                if (enemies[i].vie <= 0) {
                    removeEnemy(enemies, enemyCount, i, enemyFilename);
                    i--;
                }
            }
        }

        player->last_lightning = currentTime;
        printf("Foudre utilisée !\n");
    } else {
        int timeLeft = 120 - (int)secondsSinceLastLightning;
        printf("Temps restant avant la réutilisation de la foudre : %d secondes\n", timeLeft);
    }
}

void updateAndRenderLightning(SDL_Renderer *renderer) {
    if (lightning.active) {
        if (lightning.framesLeft % 2 == 0) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Jaune
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanc
        }

        SDL_RenderFillRect(renderer, &lightning.rect);

        lightning.framesLeft--;

        if (lightning.framesLeft <= 0) {
            lightning.active = 0;
        }
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
