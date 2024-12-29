#ifndef MAZE_SOLVER_H
#define MAZE_SOLVER_H

#include <stdbool.h>

// Dimensions de la grille
#define ROWS 15
#define COLS 21

// Identifiants des tuiles
enum {
    SOL = 1,
    SOL_VARIANT_1,
    SOL_VARIANT_2,
    SOL_VARIANT_3,
    SOL_VARIANT_4,
    WALL_FRONT,
    WALL_LEFT,
    WALL_RIGHT,
    WATER_BLOCK,
    ROCK,
    DEBRIS,
    STUMP,
    SPAWN_PLAYER_1,
    SPAWN_PLAYER_2
};

/**
 * Charge une grille de jeu depuis un fichier.
 * 
 * Cette fonction lit les données d'un fichier pour remplir la grille du labyrinthe 
 * avec les valeurs correspondant à chaque tuile (sol, mur, obstacles, etc.).
 * 
 * @param filename : Le nom du fichier contenant les données de la grille.
 * @param grid : Tableau 2D représentant la grille du labyrinthe.
 * 
 * @return true si la grille a été chargée avec succès, sinon false.
 */
bool load_grid(const char *filename, int grid[ROWS][COLS]);

/**
 * Vérifie si une tuile est traversable.
 * 
 * Cette fonction vérifie si une tuile donnée (représentée par un identifiant) 
 * peut être traversée par le joueur ou non.
 * 
 * @param tile : L'identifiant de la tuile à vérifier.
 * 
 * @return true si la tuile est traversable, sinon false.
 */
bool is_walkable(int tile);

/**
 * Trouve les points de départ et d'arrivée du labyrinthe.
 * 
 * Cette fonction cherche les positions de départ et d'arrivée du joueur dans la grille, 
 * en recherchant les tuiles spécifiées comme "SPAWN_PLAYER_1" et "SPAWN_PLAYER_2".
 * 
 * @param grid : Tableau 2D représentant la grille du labyrinthe.
 * @param start_x : Pointeur vers la coordonnée x du point de départ.
 * @param start_y : Pointeur vers la coordonnée y du point de départ.
 * @param end_x : Pointeur vers la coordonnée x du point d'arrivée.
 * @param end_y : Pointeur vers la coordonnée y du point d'arrivée.
 * 
 * @return true si les points de départ et d'arrivée ont été trouvés, sinon false.
 */
bool find_spawn_points(int grid[ROWS][COLS], int *start_x, int *start_y, int *end_x, int *end_y);

/**
 * Vérifie si le labyrinthe est solvable.
 * 
 * Cette fonction détermine si le labyrinthe, représenté par la grille, peut être résolu 
 * en vérifiant si un chemin existe entre le point de départ et le point d'arrivée.
 * 
 * @param grid : Tableau 2D représentant la grille du labyrinthe.
 * 
 * @return true si le labyrinthe est solvable, sinon false.
 */
bool is_maze_solvable(int grid[ROWS][COLS]);

#endif // MAZE_SOLVER_H
