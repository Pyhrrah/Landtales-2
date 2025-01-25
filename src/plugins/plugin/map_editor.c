// map_editor.c
#include <stdio.h>

// Fonction pour générer une map de 15 x 21

/*

    La map correspond à une grille de 15 lignes et 21 colonnes.

    Chaque case de la grille peut contenir une valeur entière comprise entre 0 et 15 inclus.
    Pour des raisons de cohérences de fonctionnement, nous vous conseillons vivement de respecter les ordres suivants :

    - Liste des tuiles possibles et à quoi elles correspondents : 

    -   0 : SOL -> Aucune collision possible
    -   1 : SOL_VARIANT_1 -> Aucune collision possible
    -   2 : SOL_VARIANT_2 -> Aucune collision possible
    -   3 : SOL_VARIANT_3 -> Aucune collision possible
    -   4 : SOL_VARIANT_4 -> Aucune collision possible
    -   5 : WALL_FRONT -> Collision possible
    -   6 : WALL_LEFT -> Collision possible
    -   7 : WALL_RIGHT -> Collision possible
    -   8 : WATER_BLOCK -> Collision possible
    -   9 : ROCK -> Collision possible
    -   10 : DEBRIS -> Collision possible
    -   11 : STUMP -> Collision possible
    -   12 : SPAWN_PLAYER_1 -> Aucune collision possible
    -   13 : SPAWN_PLAYER_2 -> Aucune collision possibles

    Il ne peux y avoir qu'un seul SPAWN_PLAYER_1 et un seul SPAWN_PLAYER_2 sur la map.
    Si vous n'avez pas donné les deux tuiles, vous pourrez les ajoutes vous-même dans l'éditeur par la suite.


    Vous pouvez personnaliser la map comme vous le souhaitez, mais il est important de respecter les valeurs de tuiles pour le bon fonctionnement du jeu.
};

*/

void generate_grid(int grid[15][21]) {
    // Initialisation de la map avec des tuiles SOL_VARIANT_1
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 21; j++) {
            grid[i][j] = 0;  // SOL
        }
    }

    // Définir les murs
    for (int i = 0; i < 15; i++) {
        grid[i][0] = 6;  // WALL_LEFT
        grid[i][20] = 7; // WALL_RIGHT
    }
    for (int j = 0; j < 21; j++) {
        grid[0][j] = 5;  // WALL_FRONT
        grid[14][j] = 5; // WALL_FRONT
    }

    // Ajouter des obstacles (ROCK, WATER_BLOCK, DEBRIS)
    grid[5][5] = 8;  // WATER_BLOCK
    grid[5][6] = 8;  // WATER_BLOCK
    grid[6][5] = 8;  // WATER_BLOCK
    grid[7][12] = 9; // ROCK
    grid[7][13] = 9; // ROCK
    grid[9][8] = 10;  // DEBRIS
    grid[9][9] = 10;  // DEBRIS

    // Ajouter les spawns des joueurs
    grid[1][1] = 12;  // SPAWN_PLAYER_1
    grid[13][19] = 13; // SPAWN_PLAYER_2
}
