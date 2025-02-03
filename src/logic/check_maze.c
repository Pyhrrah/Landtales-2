#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 15
#define COLS 21


/*

Fichier de vérification de la grille de jeu, on vérifie si le labyrinthe est solvable pour le mode multijoueur

*/

// Enumération des différents types de tuiles
enum {
    SOL = 0,
    SOL_VARIANT_1 = 1,
    SOL_VARIANT_2 = 2,
    SOL_VARIANT_3 = 3,
    SOL_VARIANT_4 = 4,
    WALL_FRONT = 5,
    WALL_LEFT = 6,
    WALL_RIGHT = 7,
    WATER_BLOCK = 8,
    ROCK = 9,
    DEBRIS = 10,
    STUMP = 11,
    SPAWN_PLAYER_1 = 12,
    SPAWN_PLAYER_2 = 13
};


// Charge une grille depuis un fichier
int load_grid(const char *filename, int grid[ROWS][COLS]) {
    char filepath[256] = "./data/editor/";
    strcat(filepath, filename);

    // Ouverture du fichier
    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return 0;
    }

    // Lecture de la grille pour la charger en mémoire et vérifier si elle est solvable
    for (int i = 0; i < ROWS; i++) {  
        for (int j = 0; j < COLS; j++) { 
            if (fscanf(file, "%d", &grid[i][j]) != 1) {
                fprintf(stderr, "Erreur lors de la lecture de la grille\n");
                fclose(file);
                return 0;
            }
        }
    }

    fclose(file);
    return 1;
}

// Vérifie si une tuile est franchissable
int is_walkable(int tile) {
    // Les tuiles franchissables sont les suivantes : SOL, SOL_VARIANT_1, SOL_VARIANT_2, SOL_VARIANT_3, SOL_VARIANT_4, SPAWN_PLAYER_1, SPAWN_PLAYER_2
    // Les autres tuiles sont des obstacles
    switch (tile) {
        case SOL:
        case SOL_VARIANT_1:
        case SOL_VARIANT_2:
        case SOL_VARIANT_3:
        case SOL_VARIANT_4:
        case SPAWN_PLAYER_1:
        case SPAWN_PLAYER_2:
            return 1;
        default:
            return 0;
    }
}

// Vérifie si une tuile est un mur pour après vérifier si le labyrinthe est solvable
int find_spawn_points(int grid[ROWS][COLS], int *start_x, int *start_y, int *end_x, int *end_y) {
    int found_player_1 = 0;
    int found_player_2 = 0;
    // Recherche des points de spawn pour les joueurs
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j] == SPAWN_PLAYER_1) {
                *start_x = i;
                *start_y = j;
                found_player_1 = 1;
            } else if (grid[i][j] == SPAWN_PLAYER_2) {
                *end_x = i;
                *end_y = j;
                found_player_2 = 1;
            }

            if (found_player_1 && found_player_2) {
                return 1;
            }
        }
    }

    if (!found_player_1) {
        printf("Erreur : point de spawn pour le joueur 1 introuvable.\n");
    }
    if (!found_player_2) {
        printf("Erreur : point de spawn pour le joueur 2 introuvable.\n");
    }

    return 0;
}

// Vérifie si le labyrinthe est solvable
// Utilisation de l'algorithme de recherche en largeur (BFS)

/*

Sources : 
https://www.geeksforgeeks.org/breadth-first-search-or-bfs-for-a-graph/

*/


int is_maze_solvable(int grid[ROWS][COLS]) {
    int start_x, start_y, end_x, end_y;

    if (!find_spawn_points(grid, &start_x, &start_y, &end_x, &end_y)) {
        fprintf(stderr, "Impossible de trouver les deux points de spawn\n");
        return 0;
    }

    // Initialisation de la grille de visite
    int visited[ROWS][COLS] = {0};
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};  

    int queue[ROWS * COLS][2];
    int front = 0, rear = 0;

    // Ajout du point de départ dans la file, le but ici est de trouver le point de spawn du joueur 2
    queue[rear][0] = start_x;
    queue[rear][1] = start_y;
    rear++;
    visited[start_x][start_y] = 1;

    // Fonctionnement : tant que la file n'est pas vide, on récupère le premier élément de la file, on le retire de la file et on regarde 
    // si c'est le point de spawn du joueur 2
    while (front < rear) {
        int x = queue[front][0];
        int y = queue[front][1];
        front++;

        // Si on a trouvé le point de spawn du joueur 2, on retourne 1
        if (x == end_x && y == end_y) {
            return 1;
        }

        // Sinon, on regarde les 4 directions
        for (int d = 0; d < 4; d++) {
            int nx = x + directions[d][0];
            int ny = y + directions[d][1];

            if (nx >= 0 && ny >= 0 && nx < ROWS && ny < COLS && !visited[nx][ny] && is_walkable(grid[nx][ny])) {
                visited[nx][ny] = 1;
                queue[rear][0] = nx;
                queue[rear][1] = ny;
                rear++;
            }
        }
    }

    return 0;
}