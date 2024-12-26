#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>
#include "./../../include/core/ennemies.h"

// Structure pour le joueur
typedef struct {
    SDL_Rect rect;              
    int vie;                    
    int attaque;                
    int defense;                
    char orientation;           
    int id;                     
    bool regen_dispo;           
    time_t last_regen;          
    int argent;  
    time_t last_lightning;               
} Player;

// Structure pour les attaques
typedef struct {
    SDL_Rect rect;  
    bool active;
    int duration;
    int frameCounter;
} Attack;

// Initialisation du joueur
void initPlayer(Player *player, int x, int y, int w, int h, int vie) {
    player->rect.x = x;
    player->rect.y = y;
    player->rect.w = w;
    player->rect.h = h;
    player->vie = vie;          
    player->attaque = 15;      
    player->defense = 10;      
    player->orientation = 'B';  
    player->id = 1;             
    player->regen_dispo = true; 
    player->last_regen = 0; 
    player->argent = 0;     
    player->last_lightning = 0;    
}
// Fonction d'attaque à l'épée
void attackWithSword(SDL_Renderer *renderer, Player *player, char orientation, Enemy enemies[], int *enemyCount, const char *enemyFilename) {
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

    printf("Sword position: (%d, %d), size: (%d, %d)\n", swordRect.x, swordRect.y, swordRect.w, swordRect.h);

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
void createLightningAtMouse(SDL_Renderer *renderer) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    SDL_Rect lightningRect = {mouseX - 20, mouseY - 20, 40, 40};  
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  
    SDL_RenderFillRect(renderer, &lightningRect);
}


// Fonction pour utiliser la foudre
void useLightning(SDL_Renderer *renderer, Player *player) {
    time_t currentTime = time(NULL);
    double secondsSinceLastLightning = difftime(currentTime, player->last_lightning);
    
    if (secondsSinceLastLightning >= 120) {
        createLightningAtMouse(renderer);  
        
        player->last_lightning = currentTime;
        
        printf("Foudre utilisée !\n");
    } else {
        int timeLeft = 120 - (int)secondsSinceLastLightning;
        printf("Temps restant avant la réutilisation de la foudre : %d secondes\n", timeLeft);
    }
}



