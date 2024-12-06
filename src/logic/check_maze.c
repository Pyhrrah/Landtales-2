#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 15
#define COLS 21


// Enumération des différents types de tuiles
enum {
    SOL = 1,
    SOL_VARIANT_1 = 2,
    SOL_VARIANT_2 = 3,
    SOL_VARIANT_3 = 4,
    SOL_VARIANT_4 = 5,
    WALL_FRONT = 6,
    WALL_LEFT = 7,
    WALL_RIGHT = 8,
    WATER_BLOCK = 9,
    ROCK = 10,
    DEBRIS = 11,
    SPAWN_PLAYER_1 = 12,
    SPAWN_PLAYER_2 = 13
};


// Charge une grille depuis un fichier
bool load_grid(const char *filename, int grid[ROWS][COLS]) {
    char filepath[256] = "./data/editor/";
    strcat(filepath, filename);

    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return false;
    }

    for (int i = 0; i < ROWS; i++) {  
        for (int j = 0; j < COLS; j++) { 
            if (fscanf(file, "%d", &grid[i][j]) != 1) {
                fprintf(stderr, "Erreur lors de la lecture de la grille\n");
                fclose(file);
                return false;
            }
        }
    }

    fclose(file);
    return true;
}

// Vérifie si une tuile est franchissable
bool is_walkable(int tile) {
    switch (tile) {
        case SOL:
        case SOL_VARIANT_1:
        case SOL_VARIANT_2:
        case SOL_VARIANT_3:
        case SOL_VARIANT_4:
        case SPAWN_PLAYER_1:
        case SPAWN_PLAYER_2:
            return true;
        default:
            return false;
    }
}

// Vérifie si une tuile est un mur
bool find_spawn_points(int grid[ROWS][COLS], int *start_x, int *start_y, int *end_x, int *end_y) {
    bool found_player_1 = false;
    bool found_player_2 = false;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j] == SPAWN_PLAYER_1) {
                *start_x = i;
                *start_y = j;
                found_player_1 = true;
            } else if (grid[i][j] == SPAWN_PLAYER_2) {
                *end_x = i;
                *end_y = j;
                found_player_2 = true;
            }

            if (found_player_1 && found_player_2) {
                return true;
            }
        }
    }

    if (!found_player_1) {
        printf("Erreur : point de spawn pour le joueur 1 introuvable.\n");
    }
    if (!found_player_2) {
        printf("Erreur : point de spawn pour le joueur 2 introuvable.\n");
    }

    return false;
}

// Vérifie si le labyrinthe est solvable
// Utilisation de l'algorithme de recherche en largeur (BFS)
bool is_maze_solvable(int grid[ROWS][COLS]) {
    int start_x, start_y, end_x, end_y;

    if (!find_spawn_points(grid, &start_x, &start_y, &end_x, &end_y)) {
        fprintf(stderr, "Impossible de trouver les deux points de spawn\n");
        return false;
    }

    bool visited[ROWS][COLS] = {false};
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};  

    int queue[ROWS * COLS][2];
    int front = 0, rear = 0;

    queue[rear][0] = start_x;
    queue[rear][1] = start_y;
    rear++;
    visited[start_x][start_y] = true;

    while (front < rear) {
        int x = queue[front][0];
        int y = queue[front][1];
        front++;

        if (x == end_x && y == end_y) {
            return true;
        }

        for (int d = 0; d < 4; d++) {
            int nx = x + directions[d][0];
            int ny = y + directions[d][1];

            if (nx >= 0 && ny >= 0 && nx < ROWS && ny < COLS && !visited[nx][ny] && is_walkable(grid[nx][ny])) {
                visited[nx][ny] = true;
                queue[rear][0] = nx;
                queue[rear][1] = ny;
                rear++;
            }
        }
    }

    return false;
}