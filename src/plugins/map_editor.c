// map_editor.c
#include <stdio.h>

// Fonction pour générer une map de 15 x 21

void generate_grid(int grid[15][21]) {
    // Remplir la grille de 1
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 15; j++) {
            grid[i][j] = 6;
        }
    }
}
