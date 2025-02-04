#ifndef PROJECTILES_H
#define PROJECTILES_H

#include <SDL2/SDL.h>
#include "./../core/player.h"

// Définir la taille maximale des projectiles
#define MAX_PROJECTILES 100
#define TILE_SIZE 32
#define ROWS 15
#define COLS 21

/**
 * Structure représentant un projectile
 * - rect : rectangle SDL définissant la position et la taille du projectile.
 * - dx, dy : direction et vitesse du projectile.
 * - active : indique si le projectile est actif ou non.
 */
typedef struct {
    SDL_Rect rect;  // Position et taille du projectile
    float dx, dy;   // Direction et vitesse du projectile
    int active;     // Si le projectile est actif ou non
} Projectile;

// Déclaration du tableau de projectiles
extern Projectile projectiles[MAX_PROJECTILES];


/**
 * Initialiser les projectiles
 */
void initProjectiles();

/**
 * Faire apparaître un projectile
 * @param sourceRect : le rectangle de l'ennemi qui tire
 * @param targetRect : le rectangle de la cible
 * @param speed : la vitesse du projectile
 */
void spawnProjectile(SDL_Rect *sourceRect, SDL_Rect *targetRect, float speed);

/**
 * Mettre à jour les projectiles
 * @param mapRoom : la map
 */
void updateProjectiles(int mapRoom[ROWS][COLS]);


/**
 * Vérifier les collisions entre les projectiles et le joueur
 * @param player : le joueur
 */
void checkProjectileCollisions(Player *player);

/**
 * Rendre les projectiles
 * @param renderer : le renderer
 * @param projectileTexture : la texture du projectile
 */
void renderProjectiles(SDL_Renderer *renderer, SDL_Texture *projectileTexture);


/**
 * Nettoyer les projectiles inactifs
 */
void clearInactiveProjectiles();

/**
 * Vérifier si un projectile est en dehors de l'écran
 * @param proj : le projectile
 * @return 1 si le projectile est en dehors de l'écran, 0 sinon
 */
int isProjectileOutOfBounds(Projectile *proj);

#endif // PROJECTILES_H
