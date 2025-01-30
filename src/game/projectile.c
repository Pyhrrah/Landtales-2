#include "./../../include/game/projectile.h"
#include <math.h>
#include <stdio.h>

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
    return proj->rect.x < 0 || proj->rect.y < 0 ||
           proj->rect.x >= COLS * TILE_SIZE || proj->rect.y >= ROWS * TILE_SIZE;
}

// Vérification des collisions entre les projectiles et le joueur
void checkProjectileCollisions(Player *player) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active && SDL_HasIntersection(&projectiles[i].rect, &player->rect)) {
            projectiles[i].active = 0;
            player->vie -= 10; // Réduit la vie du joueur de 10
            printf("Projectile touche le joueur ! Vie restante : %d\n", player->vie);
        }
    }
}

// Rendu des projectiles
void renderProjectiles(SDL_Renderer *renderer) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge pour les projectiles
            SDL_RenderFillRect(renderer, &projectiles[i].rect);
        }
    }
}

// Supprimer les projectiles inactifs
void clearInactiveProjectiles() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) {
            projectiles[i].rect.x = 0;
            projectiles[i].rect.y = 0;
        }
    }
}
