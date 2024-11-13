#ifndef GRID_H
#define GRID_H

#include <SDL2/SDL.h>

#define GRID_WIDTH 21
#define GRID_HEIGHT 15
#define UNDO_STACK_SIZE 100

extern int grid[GRID_WIDTH][GRID_HEIGHT];
extern int undoStack[UNDO_STACK_SIZE][GRID_WIDTH][GRID_HEIGHT];
extern int undoIndex;
extern char *selectedFile;

void init_grid(const char *file_path);
void save_grid(SDL_Renderer *renderer);
void push_undo();
void undo();
void reset_grid();
void draw(SDL_Renderer *renderer);
int directory_exists(const char *path);
void create_directory(const char *path);

#endif
