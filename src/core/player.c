#include <SDL2/SDL.h>
#include <time.h>
#include "./../../include/core/ennemies.h"
#include "./../../include/core/player.h"
#include "./../../include/utils/video.h"
#include "./../../include/plugins/open_plugin_damage.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
Fichier player.c, ce dernier contient les fonctions pour gérer le joueur du jeu (initialisation, attaque, déplacement, etc.)

*/

#define TILE_SIZE 32
#define ROWS 15
#define COLS 21

Lightning lightning = {0};
RegenAnimation regenAnimation = {0};

// Fonction pour charger le skin du joueur depuis un fichier
void load_skin_from_file(const char *filename, char *output_path) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier : %s\n", filename);
        return;
    }

    char skin_name[256];
    if (fgets(skin_name, sizeof(skin_name), file) == NULL) {
        fprintf(stderr, "Erreur lors de la lecture du fichier : %s\n", filename);
        fclose(file);
        return;
    }

    skin_name[strcspn(skin_name, "\n")] = 0;

    const char *base_path = "assets/images/sprite/player/";
    snprintf(output_path, 512, "%s%s", base_path, skin_name);

    fclose(file);
}

// Fonction pour initialiser le joueur
void initPlayer(Player *player, int x, int y, int w, int h, int vie, int argent, int attaque, int defense, int max_vie, SDL_Renderer *renderer) {
    // Initialisation des attributs du joueur
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
    player->frame = 0;

    // Chargement du skin du joueur depuis un fichier
    char skin_path[512];
    load_skin_from_file("assets/skin_config.txt", skin_path);

    SDL_Surface *surface = IMG_Load(skin_path);
    if (!surface) {
        printf("Erreur chargement image joueur : %s\n", IMG_GetError());
        return;
    }

    player->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!player->texture) {
        printf("Erreur création texture joueur : %s\n", SDL_GetError());
    }
}
// Fonction d'attaque à l'épée
void attackWithSword(SDL_Renderer *renderer, Player *player, char orientation, int *enemyCount, const char *enemyFilename) {
    SDL_Rect swordRect;
    swordRect.w = 8;
    swordRect.h = 32;

    // Position de l'épée en fonction de l'orientation du joueur
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

    // Rendu de l'épée
    if (SDL_SetRenderDrawColor(renderer, 206, 206, 206, 255) == 1) {
        printf("Erreur lors du changement de couleur : %s\n", SDL_GetError());
    }
    if (SDL_RenderFillRect(renderer, &swordRect) == 1) {
        printf("Erreur lors du rendu du rectangle : %s\n", SDL_GetError());
    }

    // Vérification des collisions avec les ennemis
    for (int i = 0; i < *enemyCount; i++) {
        if (SDL_HasIntersection(&swordRect, &enemies[i].rect)) {
            int damageToEnemy = calculerDegats(player->attaque, enemies[i].defense, EPEE);

            enemies[i].vie -= damageToEnemy; 
            printf("Ennemi %d touché ! Vie restante : %d\n", i, enemies[i].vie);

            if (enemies[i].vie <= 0) {
                removeEnemy(enemies, enemyCount, i, enemyFilename); 
                i--;
            }
        }
    }
}

// Fonction pour mettre à jour le joueur
void shootArrow(Player *player) {
    // Vérification du cooldown
    Uint32 currentTime = SDL_GetTicks();
    for (int i = 0; i < 3; i++) {
        if (!player->arrows[i].active && currentTime - player->arrowTimestamps[i] >= 10000) {
            player->arrows[i].active = 1; 
            player->arrows[i].orientation = player->orientation;
            
            // Position de la flèche en fonction de l'orientation du joueur
            switch (player->orientation) {
                case 'U':  
                    player->arrows[i].rect.w = 4;    
                    player->arrows[i].rect.h = 16;  
                    player->arrows[i].rect.x = player->rect.x + (player->rect.w / 2) - (player->arrows[i].rect.w / 2);
                    player->arrows[i].rect.y = player->rect.y - player->arrows[i].rect.h;
                    break;
                case 'D':  
                    player->arrows[i].rect.w = 4;    
                    player->arrows[i].rect.h = 16;   
                    player->arrows[i].rect.x = player->rect.x + (player->rect.w / 2) - (player->arrows[i].rect.w / 2);
                    player->arrows[i].rect.y = player->rect.y + player->rect.h;
                    break;
                case 'L': 
                    player->arrows[i].rect.w = 16;   
                    player->arrows[i].rect.h = 4;    
                    player->arrows[i].rect.x = player->rect.x - player->arrows[i].rect.w;
                    player->arrows[i].rect.y = player->rect.y + (player->rect.h / 2) - (player->arrows[i].rect.h / 2);
                    break;
                case 'R': 
                    player->arrows[i].rect.w = 16;   
                    player->arrows[i].rect.h = 4;    
                    player->arrows[i].rect.x = player->rect.x + player->rect.w;
                    player->arrows[i].rect.y = player->rect.y + (player->rect.h / 2) - (player->arrows[i].rect.h / 2);
                    break;
            }

            // Incrémentation du nombre de flèches
            player->arrowCount++;
            player->arrowTimestamps[i] = currentTime;
            break;
        }
    }
}

// Mettre à jour les flèches
void updateArrows(Player *player, int mapRoom[ROWS][COLS], int *enemyCount, const char *enemyFilename) {
    // Vérification des collisions avec les ennemis
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

            // Vérification des collisions avec les murs
            int tileX = player->arrows[i].rect.x / TILE_SIZE;
            int tileY = player->arrows[i].rect.y / TILE_SIZE;
            if (tileX >= 0 && tileX < COLS && tileY >= 0 && tileY < ROWS) {

                // Vérification des collisions avec les murs
                int tileId = mapRoom[tileY][tileX];
                if (tileId == 5 || tileId == 6 || tileId == 7 || 
                    tileId == 12 || tileId == 14 || 
                    tileId == 15 || tileId == 16 || tileId == 17 || tileId == 18 || tileId == 19) {
                        // Désactivation de la flèche
                    player->arrows[i].active = 0; 
                    player->arrowCount--;
                    player->arrowTimestamps[i] = SDL_GetTicks();
                    continue;
                }
            }

            // Vérification des collisions avec les ennemis
            for (int j = 0; j < *enemyCount; j++) {
                if (SDL_HasIntersection(&player->arrows[i].rect, &enemies[j].rect)) {

                    int damageToEnemy = calculerDegats(player->attaque, enemies[j].defense, ARC);

                    // Réduction des PV de l'ennemi

                    enemies[j].vie -= damageToEnemy;
                    printf("Ennemi %d touché par une flèche ! Vie restante : %d\n", j, enemies[j].vie);
                    player->arrows[i].active = 0;
                    player->arrowCount--;
                    player->arrowTimestamps[i] = SDL_GetTicks();

                    // Suppression de l'ennemi si ses PV sont inférieurs ou égaux à 0

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
    // Rendu des flèches
    SDL_SetRenderDrawColor(renderer, 93, 53, 23, 255);
    for (int i = 0; i < 3; i++) {
        if (player->arrows[i].active) {
            SDL_RenderFillRect(renderer, &player->arrows[i].rect);
        }
    }
}

int areArrowsActive(Player *player) {
    // Vérification si des flèches sont actives
    for (int i = 0; i < player->arrowCount; i++) {
        if (player->arrows[i].active) {
            return 1; 
        }
    }
    return 0;
}

void regeneratePlayer(Player *player) {
    // Régénération du joueur
    time_t currentTime = time(NULL);
    double secondsSinceLastRegen = difftime(currentTime, player->last_regen);
    
    // Vérification du cooldown
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
        // Jouer le son de régénération
        playSong("./assets/music/sons/regen.mp3"); 
        player->last_regen = currentTime;
        printf("Régénération effectuée : +%d PV\n", regenAmount);
    } else {
        // Affichage du temps restant avant la régénération
        int timeLeft = 180 - (int)secondsSinceLastRegen;
        printf("Temps restant avant la régénération : %d secondes\n", timeLeft);
    }
}


void updateAndRenderRegenAnimation(SDL_Renderer *renderer, Player *player) {
    // Mise à jour et rendu de l'animation de régénération
    if (regenAnimation.active) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
        // Rendu des particules de régénération
        for (int i = 0; i < 20; i++) {
            int playerCenterX = player->rect.x + player->rect.w / 2;
            int playerCenterY = player->rect.y + player->rect.h / 2;

            regenAnimation.rects[i].x = playerCenterX + regenAnimation.offsets[i].x;
            regenAnimation.rects[i].y = playerCenterY + regenAnimation.offsets[i].y;

            SDL_RenderFillRect(renderer, &regenAnimation.rects[i]);
        }

        regenAnimation.framesLeft--;
        // Désactivation de l'animation une fois le nombre de frames restantes à 0

        if (regenAnimation.framesLeft <= 0) {
            regenAnimation.active = 0;
        }
    }
}

// Fonction pour créer une foudre à la position de la souris
void createLightningAtMouse(SDL_Renderer *renderer, int *enemyCount, const char *enemyFilename) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    // Création d'un rectangle jaune à la position de la souris
    
    SDL_Rect lightningRect = {mouseX - 20, mouseY - 20, 40, 40};  
    // Rendu du rectangle jaune
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  
    SDL_RenderFillRect(renderer, &lightningRect);

    for (int i = 0; i < *enemyCount; i++) {
            if (SDL_HasIntersection(&lightningRect, &enemies[i].rect)) {
                // Calcul des dégâts infligés à l'ennemi
                int damageToEnemy = calculerDegats(30, enemies[i].defense, FOUDRE);
                enemies[i].vie -= damageToEnemy; 
                printf("Ennemi %d touché ! Vie restante : %d\n", i, enemies[i].vie);
                // Suppression de l'ennemi si ses PV sont inférieurs ou égaux à 0
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
    // Vérification du cooldown
    if (secondsSinceLastLightning >= 120) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        // Création d'un rectangle jaune à la position de la souris
        lightning.rect = (SDL_Rect){mouseX - 20, mouseY - 20, 40, 40};
        lightning.framesLeft = 10; 
        lightning.active = 1; 
        playSong("./assets/music/sons/foudre.mp3");     
        // Vérification des collisions avec les ennemis
        for (int i = 0; i < *enemyCount; i++) {
            // Calcul des dégâts infligés à l'ennemi
            if (SDL_HasIntersection(&lightning.rect, &enemies[i].rect)) {
                int damageToEnemy = calculerDegats(player->attaque, enemies[i].defense, FOUDRE);
                enemies[i].vie -= damageToEnemy;
                printf("Ennemi %d touché ! Vie restante : %d\n", i, enemies[i].vie);
                // Suppression de l'ennemi si ses PV sont inférieurs ou égaux à 0
                if (enemies[i].vie <= 0) {
                    removeEnemy(enemies, enemyCount, i, enemyFilename);
                    i--;
                }
            }
        }

        // Mise à jour du cooldown
        player->last_lightning = currentTime;
        printf("Foudre utilisée !\n");
    } else {
        // Affichage du temps restant avant la réutilisation de la foudre
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

        // Rendu de la foudre
        SDL_RenderFillRect(renderer, &lightning.rect);

        lightning.framesLeft--;


        // Désactivation de la foudre une fois le nombre de frames restantes à 0
        if (lightning.framesLeft <= 0) {
            lightning.active = 0;
        }
    }
}

int getLightningCooldown(Player *player) {
    // Récupération du temps restant avant la réutilisation de la foudre
    time_t currentTime = time(NULL);
    int cooldown = 120 - difftime(currentTime, player->last_lightning);
    return (cooldown > 0) ? cooldown : 0;
}

int getRegenCooldown(Player *player) {
    // Récupération du temps restant avant la régénération
    time_t currentTime = time(NULL);
    int cooldown = 180 - difftime(currentTime, player->last_regen);
    return (cooldown > 0) ? cooldown : 0;
}


void drawPlayerWithFrame(Player *player, SDL_Renderer *renderer) {
    // Rendu du joueur
    SDL_Rect srcRect, dstRect;
    int frameWidth = 32;  
    int frameHeight = 32;

    int row = 0;  
    
    // Récupération de la ligne de sprites en fonction de l'orientation du joueur
    switch (player->orientation) {
        case 'D': row = 0; break; 
        case 'L': row = 1; break; 
        case 'R': row = 2; break; 
        case 'U': row = 3; break; 
    }

    // Récupération du rectangle source et destination en fonction du frame actuel
    srcRect.x = (player->frame % 3) * frameWidth; 
    srcRect.y = row * frameHeight;  
    srcRect.w = frameWidth;
    srcRect.h = frameHeight;

    //  Rendu du joueur
    dstRect.x = player->rect.x;
    dstRect.y = player->rect.y;
    dstRect.w = frameWidth;
    dstRect.h = frameHeight;

    // Rendu du joueur
    SDL_RenderCopy(renderer, player->texture, &srcRect, &dstRect);
}