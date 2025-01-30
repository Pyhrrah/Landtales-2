#ifndef GRID_H
#define GRID_H

#include <SDL2/SDL.h>

// Définitions des dimensions de la grille
#define GRID_WIDTH 21  // Largeur de la grille en nombre de cases 
#define GRID_HEIGHT 15 // Hauteur de la grille en nombre de cases

/**
 * Nombre maximum de grilles pouvant être stockées dans la pile d'annulation.
 */
#define UNDO_STACK_SIZE 100

/**
 * Valeur par défaut pour une case vide dans la grille.
 */
extern int grid[GRID_HEIGHT][GRID_WIDTH];

/**
 * Pile de grilles pour stocker les états précédents de la grille.
 */
extern int undoStack[UNDO_STACK_SIZE][GRID_HEIGHT][GRID_WIDTH];

/**
 * Indice de la pile d'annulation pour suivre la dernière modification.
 */
extern int undoIndex;

/**
 * Chemin du fichier actuellement sélectionné.
 */
extern char *selectedFile;

/**
 * Initialise la grille en chargeant son contenu à partir d'un fichier.
 * 
 * @param file_path : Chemin du fichier à charger dans la grille.
 */
void init_grid(const char *file_path);

/**
 * Sauvegarde la grille actuelle dans un fichier.
 * @param renderer : Le renderer SDL utilisé pour afficher des messages ou confirmations.
 */
void save_grid(SDL_Renderer *renderer);

/**
 * Enregistre l'état actuel de la grille dans la pile d'annulations.
 */
void push_undo();

/**
 * Annule la dernière modification en rétablissant l'état précédent de la grille.
 */
void undo();

/**
 * Réinitialise la grille à son état vide ou par défaut.
 */
void reset_grid();

/**
 * Dessine la grille à l'écran.
 * @param renderer : Le renderer SDL utilisé pour afficher la grille.
 */
void draw(SDL_Renderer *renderer);

/**
 * Vérifie si un répertoire existe.
 * @param path : Chemin du répertoire à vérifier.
 * @return 1 si le répertoire existe, 0 sinon.
 */
int directory_exists(const char *path);

/**
 * Crée un répertoire à l'emplacement spécifié.
 * @param path : Chemin du répertoire à créer.
 */
void create_directory(const char *path);

#endif // GRID_H
