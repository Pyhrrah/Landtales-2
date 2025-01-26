#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "./../../../include/core/player.h"
#include "./../../../include/core/ennemies.h"

// Tableau pour stocker les bonus
Bonus bonuses[5];

// Implémentation de la fonction pour générer un bonus à une position donnée
void spawnBonus(int x, int y) {
    for (int i = 0; i < 5; i++) {
        if (!bonuses[i].active) {
            bonuses[i].rect.x = x - 16;  
            bonuses[i].rect.y = y - 16;  
            bonuses[i].rect.w = 32;  
            bonuses[i].rect.h = 32;

            bonuses[i].type = (rand() % 3);  

            bonuses[i].active = 1;
            printf("Bonus de type %d généré à (%d, %d)\n", bonuses[i].type, bonuses[i].rect.x, bonuses[i].rect.y);
            return;
        }
    }
    printf("Le nombre maximum de bonus a été atteint.\n");
}

// Implémentation de la fonction pour vérifier les collisions avec le joueur
void checkBonusCollision(Player *player) {
    for (int i = 0; i < 6; i++) {  
        if (bonuses[i].active && SDL_HasIntersection(&player->rect, &bonuses[i].rect)) {
            if (bonuses[i].type == 0) {  
                player->vie += 10;
                if (player->vie > player->max_vie) {
                    player->vie = player->max_vie;
                }
                printf("Le joueur récupère 10 PV. Vie actuelle : %d\n", player->vie);
            } else if (bonuses[i].type == 1) {  
                player->argent += 5;
                printf("Le joueur récupère 5 pièces. Total d'argent : %d\n", player->argent);
            } else if (bonuses[i].type == 2) {  
                player->attaque += 5;
                printf("Le joueur récupère 5 points d'attaque\n");
            }
            bonuses[i].active = 0;  
        }
    }
}

// Fonction pour dessiner les bonus
void drawBonuses(SDL_Renderer *renderer) {
    for (int i = 0; i < 6; i++) { 
        if (bonuses[i].active) {
            SDL_Texture *bonusTexture = NULL;

            if (bonuses[i].type == 0) {
                bonusTexture = IMG_LoadTexture(renderer, "./assets/images/sprite/bonus/Icons_08.png");
            } else if (bonuses[i].type == 1) {
                bonusTexture = IMG_LoadTexture(renderer, "./assets/images/sprite/bonus/Icons_01.png");
            } else if (bonuses[i].type == 2) {
                bonusTexture = IMG_LoadTexture(renderer, "./assets/images/sprite/bonus/Icons_23.png");
            }

            if (bonusTexture) {
                SDL_RenderCopy(renderer, bonusTexture, NULL, &bonuses[i].rect);
                SDL_DestroyTexture(bonusTexture);
            }
        }
    }
}
