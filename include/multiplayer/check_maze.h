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

bool load_grid(const char *filename, int grid[ROWS][COLS]);

bool is_walkable(int tile);

bool find_spawn_points(int grid[ROWS][COLS], int *start_x, int *start_y, int *end_x, int *end_y);

bool is_maze_solvable(int grid[ROWS][COLS]);

#endif