#include "./../../include/game/projectile.h"
#include <math.h>
#include <stdio.h>

/*

Fichier projectile.c, ce dernier contient les fonctions pour gérer les projectiles du jeu

*/

// Tableau de projectiles
Projectile projectiles[MAX_PROJECTILES];

// Initialisation des projectiles
void initProjectiles() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = 0;  // Tous les projectiles sont inactifs au début
    }
}

// Génération d'un projectile
void spawnProjectile(SDL_Rect *sourceRect, SDL_Rect *targetRect, float speed) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {

        // Si le projectile n'est pas actif
        if (!projectiles[i].active) {
            // Initialiser la position du projectile à celle de la source
            projectiles[i].rect.x = sourceRect->x + sourceRect->w / 2;
            projectiles[i].rect.y = sourceRect->y + sourceRect->h / 2;
            projectiles[i].rect.w = 16;
            projectiles[i].rect.h = 16;

            // Calculer l'angle de tir vers la cible
            float angle = atan2(targetRect->y - projectiles[i].rect.y, targetRect->x - projectiles[i].rect.x);
            projectiles[i].dx = cos(angle) * speed;
            projectiles[i].dy = sin(angle) * speed;

            projectiles[i].active = 1;
            break;
        }
    }
}

// Mise à jour des projectiles
void updateProjectiles(int mapRoom[ROWS][COLS]) {

    // Mise à jour de la position des projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            projectiles[i].rect.x += (int)projectiles[i].dx;
            projectiles[i].rect.y += (int)projectiles[i].dy;

            // Vérification des sorties d'écran
            if (isProjectileOutOfBounds(&projectiles[i])) {
                projectiles[i].active = 0;
            }

            // Vérification des collisions avec des obstacles
            int gridX = projectiles[i].rect.x / TILE_SIZE;
            int gridY = projectiles[i].rect.y / TILE_SIZE;
            int tileId = mapRoom[gridY][gridX];

            // Si le projectile touche un obstacle (ici les cases avec certains id)
            if (tileId == 5 || tileId == 6 || tileId == 7 || tileId == 12 || tileId == 14 ||
                tileId == 15 || tileId == 16 || tileId == 17 || tileId == 18 || tileId == 19) {
                projectiles[i].active = 0;
            }
        }
    }
}

// Vérification si un projectile est sorti de l'écran
int isProjectileOutOfBounds(Projectile *proj) {
    // Vérification des sorties d'écran, s'il est sorti de l'écran, on le désactive
    return proj->rect.x < 0 || proj->rect.y < 0 ||
           proj->rect.x >= COLS * TILE_SIZE || proj->rect.y >= ROWS * TILE_SIZE;
}

// Vérification des collisions entre les projectiles et le joueur
void checkProjectileCollisions(Player *player) {
    // Vérification des collisions entre les projectiles et le joueur, si un projectile touche le joueur, on réduit sa vie et on désactive le projectile
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active && SDL_HasIntersection(&projectiles[i].rect, &player->rect)) {
            projectiles[i].active = 0;
            player->vie -= 10 * (1 - player->defense / 100); 
            printf("Projectile touche le joueur ! Vie restante : %d\n", player->vie);
        }
    }
}

// Rendu des projectiles
void renderProjectiles(SDL_Renderer *renderer, SDL_Texture *projectileTexture) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            SDL_RenderCopy(renderer, projectileTexture, NULL, &projectiles[i].rect);
        }
    }
}

// Supprimer les projectiles inactifs
void clearInactiveProjectiles() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        // Si le projectile est inactif, on réinitialise sa position
        if (!projectiles[i].active) {
            projectiles[i].rect.x = 0;
            projectiles[i].rect.y = 0;
        }
    }
}
