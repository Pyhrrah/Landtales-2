#ifndef ENNEMIES_H
#define ENNEMIES_H

#include <SDL2/SDL.h>

#define MAX_ENNEMIES 25  // Nombre maximum d'ennemis pouvant exister simultanément.

/**
 * Structure représentant un ennemi.
 * - rect : rectangle SDL définissant la position et la taille de l'ennemi.
 * - vie : points de vie de l'ennemi.
 * - attaque : points d'attaque de l'ennemi.
 * - defense : points de défense de l'ennemi.
 * - type : type de l'ennemi (utilisé pour identifier des comportements ou caractéristiques spécifiques).
 * - id : identifiant unique de l'ennemi.
 */
typedef struct {
    SDL_Rect rect;
    int vie;
    int attaque;
    int defense;
    int type;
    int id;
} Enemy;


/**
 * Structure représentant la configuration d'un ennemi.
 * - vie : points de vie de l'ennemi.
 * - attaque : points d'attaque de l'ennemi.
 * - defense : points de défense de l'ennemi.
 */
typedef struct {
    int vie;
    int attaque;
    int defense;
} EnemyConfig;

/**
 * Structure représentant un bonus.
 * - rect : rectangle SDL définissant la position et la taille du bonus.
 * - type : type du bonus (utilisé pour définir ses effets spécifiques).
 * - active : indique si le bonus est actif ou non.
 */
typedef struct {
    SDL_Rect rect;
    int type;
    int active;  // Remplacement de bool par int
} Bonus;

// Variables globales externes.
extern Enemy enemies[MAX_ENNEMIES];  // Tableau contenant tous les ennemis.
extern int enemyCount;              // Nombre actuel d'ennemis actifs.
extern Bonus bonuses[5];            // Tableau contenant les bonus disponibles.

/**
 * Initialise les ennemis en chargeant les données depuis un fichier.
 * @param filename : chemin du fichier contenant les données des ennemis.
 */
void initEnemies(const char *filename);

/**
 * Dessine les ennemis sur le renderer SDL.
 * @param renderer : le renderer SDL sur lequel dessiner.
 */
void drawEnemies(SDL_Renderer *renderer);

/**
 * Met à jour l'état des ennemis (logique et animations).
 * @param renderer : le renderer SDL utilisé pour des opérations graphiques si nécessaire.
 */
void updateEnemies(SDL_Renderer *renderer);

/**
 * Efface tous les ennemis en réinitialisant leur tableau.
 */
void clearEnemies();

/**
 * Supprime un ennemi spécifique du tableau.
 * @param enemies : tableau des ennemis.
 * @param enemyCount : pointeur sur le nombre actuel d'ennemis (sera décrémenté).
 * @param index : index de l'ennemi à supprimer dans le tableau.
 * @param enemyFilename : chemin du fichier contenant les données des ennemis (pour potentielle mise à jour).
 */
void removeEnemy(Enemy enemies[], int *enemyCount, int index, const char *enemyFilename);


/**
 * Lance un projectile avec un cooldown pour les ennemis.
 * @param playerRect : rectangle du joueur pour déterminer la direction du projectile.
 */
void launchProjectileWithCooldownEnnemies(SDL_Rect *playerRect);


#endif // ENNEMIES_H
