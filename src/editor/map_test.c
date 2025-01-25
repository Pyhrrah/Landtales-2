#include "./../../include/editor/map_test.h"
#include "./../../include/editor/colors.h"
#include <SDL2/SDL.h>
#include <stdio.h>

#define SOL 1
#define SOL_VARIANT_1 2
#define SOL_VARIANT_2 3
#define SOL_VARIANT_3 4
#define SOL_VARIANT_4 5
#define WALL_FRONT 5
#define WALL_LEFT 6
#define WALL_RIGHT 7
#define WATER_BLOCK 8
#define ROCK 9
#define DEBRIS 10
#define STUMP 11
#define PLAYER_SPAWN 12  
#define TARGET_SPAWN 13 

#define GRID_WIDTH 21  // Nombre de colonnes (21)
#define GRID_HEIGHT 15 // Nombre de lignes (15)
#define CELL_SIZE 32

int player_x = -1;  
int player_y = -1;  

// Vérifie si un mouvement est valide (en fonction de la tuile concernée)
int is_valid_move(int x, int y, int grid[GRID_HEIGHT][GRID_WIDTH]) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
        return 0;  // Sortie si la position est en dehors des limites de la grille
    }

    // Vérifie si la cellule contient un bloc impassable
    if (grid[y][x] == WALL_FRONT || grid[y][x] == WALL_LEFT || grid[y][x] == WALL_RIGHT ||
        grid[y][x] == WATER_BLOCK || grid[y][x] == ROCK || grid[y][x] == DEBRIS || grid[y][x] == STUMP) {
        return 0;  // Mouvement invalide s'il y a un mur ou un autre bloc impassable
    }

    return 1;  // Mouvement valide
}

// Dessine la carte de test
void draw_test_map(SDL_Renderer *renderer, int grid[GRID_HEIGHT][GRID_WIDTH]) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
    SDL_RenderClear(renderer);

    // Charger les textures si ce n'est pas déjà fait
    static int textures_loaded = 0; 
    if (!textures_loaded) {
        load_textures(renderer);
        textures_loaded = 1;
    }

    // Parcours de la grille par lignes (y) et colonnes (x)
    for (int y = 0; y < GRID_HEIGHT; y++) {  
        for (int x = 0; x < GRID_WIDTH; x++) { 
            SDL_Rect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};

            int cell_id = grid[y][x];

            // Dessiner la cellule avec une texture si elle existe
            if (cell_id >= 0 && (unsigned long)cell_id < sizeof(textures) / sizeof(textures[0])) {
                SDL_Texture *current_texture = textures[cell_id];
                if (current_texture != NULL) {
                    SDL_RenderCopy(renderer, current_texture, NULL, &cell);
                    continue; // Passer à la cellule suivante après avoir dessiné la texture
                }
            }

            // Si aucune texture n'est disponible, utiliser les couleurs par défaut
            if (cell_id == PLAYER_SPAWN) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Vert pour le spawn du joueur
            } else if (cell_id == TARGET_SPAWN) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Rouge pour la cible
            } else {
                SDL_SetRenderDrawColor(renderer, colors[cell_id].r, colors[cell_id].g, colors[cell_id].b, colors[cell_id].a);
            }
            SDL_RenderFillRect(renderer, &cell);
        }
    }

    // Dessiner le joueur si sa position est valide
    if (player_x != -1 && player_y != -1) {
        SDL_SetRenderDrawColor(renderer, 56, 60, 126, 205);  // Couleur de joueur
        SDL_Rect player_rect = {player_x * CELL_SIZE, player_y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
        SDL_RenderFillRect(renderer, &player_rect); 
    }

    SDL_RenderPresent(renderer); 
}


// Test de la carte
// Gestion d'un joueur pour tester la map
int test_map(SDL_Renderer *renderer, int grid[GRID_HEIGHT][GRID_WIDTH]) {
    int found_spawn = 0, target_spawn = 0;

    // Recherche des positions de spawn du joueur et de la cible
    for (int y = 0; y < GRID_HEIGHT; y++) {  
        for (int x = 0; x < GRID_WIDTH; x++) { 
            if (grid[y][x] == PLAYER_SPAWN) {
                found_spawn = 1;
                player_x = x;  // x et y sont inversés par rapport aux définitions de coordonnées
                player_y = y;
            }
            if (grid[y][x] == TARGET_SPAWN) {
                target_spawn = 1;
            }
        }
    }

    if (!found_spawn) {
        printf("Le spawn du joueur doit être présent.\n");
        return 0; 
    }
    if (!target_spawn) {
        printf("La cible doit être présente.\n");
        return 0; 
    }

    int running = 1;
    SDL_Event event;

    // Affichage de la carte
    printf("Déplacement du joueur avec les touches fléchées.\n");
    for(int i = 0; i < GRID_HEIGHT; i++) {  // Affichage de la carte ligne par ligne
        for(int j = 0; j < GRID_WIDTH; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }

    while (running) {
        draw_test_map(renderer, grid); 

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0; 
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                } else if (event.key.keysym.sym == SDLK_UP) {
                    if (is_valid_move(player_x, player_y - 1, grid)) {
                        player_y--;  // Déplacement vers le haut
                    }
                } else if (event.key.keysym.sym == SDLK_DOWN) {
                    if (is_valid_move(player_x, player_y + 1, grid)) {
                        player_y++;  // Déplacement vers le bas
                    }
                } else if (event.key.keysym.sym == SDLK_LEFT) {
                    if (is_valid_move(player_x - 1, player_y, grid)) {
                        player_x--;  // Déplacement vers la gauche
                    }
                } else if (event.key.keysym.sym == SDLK_RIGHT) {
                    if (is_valid_move(player_x + 1, player_y, grid)) {
                        player_x++;  // Déplacement vers la droite
                    }
                }
            }
        }

        if (grid[player_y][player_x] == TARGET_SPAWN) {
            return 1;
        }
    }

    return 0;
}
