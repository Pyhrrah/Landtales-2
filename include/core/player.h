#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <time.h>
#include "ennemies.h"

/**
 * Enumération des types d'armes disponibles.
 * - EPEE : épée
 * - ARC : arc
 * - FOUDRE : foudre
 */
typedef enum {
    EPEE,  // 0 : épée
    ARC,   // 1 : arc
    FOUDRE // 2 : foudre
} ArmeType;

/**
 * Structure représentant une flèche tirée par le joueur.
 * - rect : rectangle SDL définissant la position et la taille de la flèche.
 * - orientation : direction de la flèche (N, S, E, W).
 * - active : indique si la flèche est actuellement active ou non.
 */
typedef struct {
    SDL_Rect rect;
    char orientation;
    int active; // Remplacement de bool par int
} Arrow;

/**
 * Structure représentant le joueur.
 * - rect : rectangle SDL définissant la position et la taille du joueur.
 * - max_vie : points de vie maximum du joueur.
 * - vie : points de vie actuels du joueur.
 * - attaque : points d'attaque du joueur.
 * - defense : points de défense du joueur.
 * - orientation : direction actuelle du joueur (N, S, E, W).
 * - id : identifiant unique du joueur.
 * - regen_dispo : indique si la régénération est disponible.
 * - last_regen : dernier moment où la régénération a été utilisée.
 * - argent : argent possédé par le joueur.
 * - last_lightning : dernier moment où l'attaque éclair a été utilisée.
 * - arrows : tableau de flèches tirées par le joueur.
 * - arrowCount : nombre actuel de flèches actives.
 * - arrowTimestamps : horodatages des flèches pour gérer les délais.
 */
typedef struct {
    SDL_Rect rect;
    int max_vie;
    int vie;
    int attaque;
    int defense;
    char orientation;
    int id;
    int regen_dispo;  // Remplacement de bool par int
    int argent;
    int arrowCount;
    Arrow arrows[3];
    time_t arrowTimestamps[3];
    time_t last_regen;
    time_t last_lightning;
} Player;

/**
 * Structure représentant une attaque en cours.
 * - rect : rectangle SDL définissant la position et la zone d'effet de l'attaque.
 */
typedef struct {
    SDL_Rect rect;
} Attack;

/*
* Structure représentant une foudre.
* - rect : rectangle SDL définissant la position et la taille de la foudre.
* - framesLeft : nombre de frames restantes avant la fin de l'attaque.
* - active : indique si la foudre est actuellement active.
*/
typedef struct {
    SDL_Rect rect;       // Position et taille de la foudre
    int framesLeft;      // Nombre de frames restantes
    int active;          // Indique si la foudre est active
} Lightning;


/*
* Structure représentant une animation de régénération.
* - rects : tableau de rectangles SDL définissant les carrés de l'animation.
* - offsets : tableau de points SDL définissant les décalages des particules par rapport au centre du joueur.
* - framesLeft : nombre de frames restantes pour l'animation.
* - active : indique si l'animation est active.
*/
typedef struct {
    SDL_Rect rects[20];      // Les rectangles des particules
    SDL_Point offsets[20];   // Les offsets des particules par rapport au centre du joueur
    int framesLeft;          // Durée restante de l'animation en frames
    int active;              // 1 si l'animation est active, 0 sinon
} RegenAnimation;

/**
 * Initialise le joueur avec des paramètres spécifiques.
 * @param player : pointeur vers la structure Player à initialiser.
 * @param x, y : position initiale du joueur.
 * @param w, h : largeur et hauteur du joueur.
 * @param vie : points de vie initiaux.
 * @param argent : argent initial.
 * @param attaque : points d'attaque initiaux.
 * @param defense : points de défense initiaux.
 * @param max_vie : points de vie maximum.
 */
void initPlayer(Player *player, int x, int y, int w, int h, int vie, int argent, int attaque, int defense, int max_vie);

/**
 * Effectue une attaque avec une épée.
 * @param renderer : renderer SDL pour dessiner l'attaque.
 * @param player : pointeur vers le joueur effectuant l'attaque.
 * @param orientation : direction de l'attaque.
 * @param enemyCount : pointeur vers le nombre d'ennemis restants (sera modifié).
 * @param enemyFilename : chemin du fichier contenant les données des ennemis.
 */
void attackWithSword(SDL_Renderer *renderer, Player *player, char orientation, int *enemyCount, const char *enemyFilename);

/**
 * Régénère les points de vie du joueur si possible.
 * @param player : pointeur vers la structure Player à régénérer.
 */
void regeneratePlayer(Player *player);

/**
 * Utilise une attaque spéciale (éclair).
 * @param renderer : renderer SDL pour dessiner l'attaque.
 * @param player : pointeur vers le joueur effectuant l'attaque.
 * @param enemyCount : pointeur vers le nombre d'ennemis restants (sera modifié).
 * @param enemyFilename : chemin du fichier contenant les données des ennemis.
 */
void useLightning(Player *player, int *enemyCount, const char *enemyFilename);

/**
 * Tire une flèche dans la direction actuelle du joueur.
 * @param player : pointeur vers le joueur effectuant le tir.
 */
void shootArrow(Player *player);

/**
 * Met à jour les flèches en fonction de leur mouvement et des collisions.
 * @param player : pointeur vers le joueur possédant les flèches.
 * @param mapRoom : tableau représentant la carte de la salle.
 * @param enemyCount : pointeur vers le nombre d'ennemis restants (sera modifié).
 * @param enemyFilename : chemin du fichier contenant les données des ennemis.
 */
void updateArrows(Player *player, int mapRoom[15][21], int *enemyCount, const char *enemyFilename);

/**
 * Dessine les flèches actives sur l'écran.
 * @param renderer : renderer SDL pour dessiner les flèches.
 * @param player : pointeur vers le joueur possédant les flèches.
 */
void renderArrows(SDL_Renderer *renderer, Player *player);


/*
* Crée une foudre à la position de la souris.
* @param renderer : renderer SDL pour dessiner la foudre.
*/
void updateAndRenderLightning(SDL_Renderer *renderer);

/**
 * Vérifie si au moins une flèche est active.
 * @param player : pointeur vers le joueur à vérifier.
 * @return true si une flèche est active, sinon false.
 */
int areArrowsActive(Player *player); // Remplacement de bool par int

/**
 * Retourne le temps restant avant que l'attaque éclair soit disponible.
 * @param player : pointeur vers le joueur.
 * @return Temps restant en millisecondes.
 */
int getLightningCooldown(Player *player);

/**
 * Retourne le temps restant avant que la régénération soit disponible.
 * @param player : pointeur vers le joueur.
 * @return Temps restant en millisecondes.
 */
int getRegenCooldown(Player *player);

/**
 * Met à jour et affiche l'animation de régénération.
 * @param renderer : renderer SDL pour dessiner l'animation.
 * @param player : pointeur vers le joueur à régénérer.
 */
void updateAndRenderRegenAnimation(SDL_Renderer *renderer, Player *player);

#endif // PLAYER_H
