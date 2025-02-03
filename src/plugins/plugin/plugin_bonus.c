#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "./../../../include/core/player.h"
#include "./../../../include/core/ennemies.h"

/*

Plugins pour les bonus du jeu et les coffres

L'utilisation de plugins permet de modifier facilement les fonctionnalités du jeu sans avoir à recompiler le code source.
Vous pouvez ajouter de nouveaux types de bonus, modifier les effets des bonus existants ou ajouter de nouveaux types de coffres.

Les bonus doivent avoir un impact sur le joueur, par exemple en augmentant sa vie, son argent ou son attaque.
Les coffres peuvent contenir des bonus ou des malus pour le joueur.
Un dossier de sprite pour les bonus est fourni dans le dossier assets/images/sprite/bonus/.
 */



// Tableau pour stocker les bonus
Bonus bonuses[5];

// Implémentation de la fonction pour générer un bonus à une position donnée
void spawnBonus(int x, int y) {
    srand(time(NULL));

    // Définir les poids pour chaque type de bonus
    int weights[] = {20, 20, 10, 50};  // Poids pour chaque type (vie, argent, attaque, aucun bonus)
    int totalWeight = 0;

    // Calculer le poids total
    for (int i = 0; i < 4; i++) {
        totalWeight += weights[i];
    }

    // Générer un nombre aléatoire entre 0 et totalWeight - 1
    int randomValue = rand() % totalWeight;

    // Déterminer si un bonus doit être généré
    if (randomValue < weights[3]) {  // Aucun bonus (10 % de chances)
        printf("Aucun bonus généré cette fois.\n");
        return;
    }

    // Trouver un emplacement libre dans le tableau des bonus
    for (int i = 0; i < 5; i++) {
        if (!bonuses[i].active) {
            bonuses[i].rect.x = x - 16;  
            bonuses[i].rect.y = y - 16;  
            bonuses[i].rect.w = 32;  
            bonuses[i].rect.h = 32;

            // Déterminer le type de bonus en fonction des poids restants
            int bonusChoice = 0;
            int cumulativeWeight = weights[0];

            while (randomValue >= cumulativeWeight && bonusChoice < 2) {
                bonusChoice++;
                cumulativeWeight += weights[bonusChoice];
            }

            bonuses[i].type = bonusChoice;  // 0 = vie, 1 = argent, 2 = attaque
            bonuses[i].active = 1;

            printf("Bonus de type %d généré à (%d, %d)\n", bonuses[i].type, bonuses[i].rect.x, bonuses[i].rect.y);
            return;
        }
    }

    printf("Le nombre maximum de bonus a été atteint.\n");
}

// Implémentation de la fonction pour vérifier les collisions avec le joueur
int checkBonusCollision(Player *player) {
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
            return 1;
        }
    }
    return 0;
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


void openChest(Player *player) {
    srand(time(NULL));

    int choice = rand() % 4;  

    switch (choice) {
        case 0:  
            player->max_vie += (rand() % 21) - 10;  
            if (player->max_vie < 1) player->max_vie = 1;  
            player->vie = player->max_vie;  
            printf("La vie du joueur a été modifiée. Vie max : %d, Vie actuelle : %d\n", player->max_vie, player->vie);
            break;

        case 1:  
            player->argent += (rand() % 21) - 10;  
            if (player->argent < 0) player->argent = 0; 
            printf("Le joueur a gagné/perdu de l'argent. Total d'argent : %d\n", player->argent);
            break;

        case 2:  
            player->attaque += (rand() % 11) - 5;  
            if (player->attaque < 0) player->attaque = 0; 
            printf("L'attaque du joueur a été modifiée. Attaque : %d\n", player->attaque);
            break;

        case 3:  
            player->defense += (rand() % 11) - 5;  
            if (player->defense < 0) player->defense = 0;  
            printf("La défense du joueur a été modifiée. Défense : %d\n", player->defense);
            break;
    }
}
