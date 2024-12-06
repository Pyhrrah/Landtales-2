#ifndef MAP_TEST_H
#define MAP_TEST_H

#include <SDL2/SDL.h>

#define GRID_WIDTH 21  
#define GRID_HEIGHT 15 

int is_valid_move(int x, int y, int grid[GRID_WIDTH][GRID_HEIGHT]);

int test_map(SDL_Renderer *renderer, int grid[GRID_WIDTH][GRID_HEIGHT]);

#endif
