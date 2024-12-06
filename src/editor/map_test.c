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

#define GRID_WIDTH 21  
#define GRID_HEIGHT 15 
#define CELL_SIZE 32

int player_x = -1;  
int player_y = -1;  


// Vérifie si un mouvement est valide (en fonction de la tuile concernée)
int is_valid_move(int x, int y, int grid[GRID_WIDTH][GRID_HEIGHT]) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
        return 0; 
    }

    if (grid[x][y] == WALL_FRONT || grid[x][y] == WALL_LEFT || grid[x][y] == WALL_RIGHT ||
        grid[x][y] == WATER_BLOCK || grid[x][y] == ROCK || grid[x][y] == DEBRIS || grid[x][y] == STUMP) {
        return 0;  
    }

    return 1; 
}

// Dessine la carte de test
void draw_test_map(SDL_Renderer *renderer, int grid[GRID_WIDTH][GRID_HEIGHT]) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
    SDL_RenderClear(renderer);

    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            SDL_Rect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};

            if (grid[x][y] == PLAYER_SPAWN) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
            } else if (grid[x][y] == TARGET_SPAWN) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
            } else {
                SDL_SetRenderDrawColor(renderer, colors[grid[x][y]].r, colors[grid[x][y]].g, colors[grid[x][y]].b, colors[grid[x][y]].a);
            }
            SDL_RenderFillRect(renderer, &cell);
        }
    }

    SDL_SetRenderDrawColor(renderer, 56, 60, 126, 205); 
    SDL_Rect player_rect = {player_x * CELL_SIZE, player_y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
    SDL_RenderFillRect(renderer, &player_rect); 

    SDL_RenderPresent(renderer); 
}

// Test de la carte
// Gestion d'un joueur pour tester la map
int test_map(SDL_Renderer *renderer, int grid[GRID_WIDTH][GRID_HEIGHT]) {
    int found_spawn = 0, target_spawn = 0;

    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            if (grid[x][y] == PLAYER_SPAWN) {
                found_spawn = 1;
                player_x = x;
                player_y = y;
            }
            if (grid[x][y] == TARGET_SPAWN) {
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
                        player_y--;
                    }
                } else if (event.key.keysym.sym == SDLK_DOWN) {
                    if (is_valid_move(player_x, player_y + 1, grid)) {
                        player_y++;
                    }
                } else if (event.key.keysym.sym == SDLK_LEFT) {
                    if (is_valid_move(player_x - 1, player_y, grid)) {
                        player_x--;
                    }
                } else if (event.key.keysym.sym == SDLK_RIGHT) {
                    if (is_valid_move(player_x + 1, player_y, grid)) {
                        player_x++;
                    }
                }
            }
        }

        if (grid[player_x][player_y] == TARGET_SPAWN) {
            return 1; 
        }
    }

    return 0;
}
