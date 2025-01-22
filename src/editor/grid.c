#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "./../../include/editor/grid.h"
#include "./../../include/editor/map_test.h"
#include "./../../include/editor/colors.h"

#define GRID_WIDTH 15  
#define GRID_HEIGHT 21  
#define CELL_SIZE 32
#define UNDO_STACK_SIZE 100
#define WINDOW_WIDTH 672
#define WINDOW_HEIGHT 544
#define CAROUSEL_HEIGHT 64

int grid[GRID_WIDTH][GRID_HEIGHT];  // Changement ici pour refléter la nouvelle dimension
int undoStack[UNDO_STACK_SIZE][GRID_WIDTH][GRID_HEIGHT];  // Changement ici aussi
int undoIndex = -1;
char *selectedFile = NULL;

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
    SPAWN_PLAYER_2,
};

extern TTF_Font *font;
extern int currentObjectID;

// Tentative de gestion pour Windows et Linux

int directory_exists(const char *path) {
    char command[256];

#ifdef _WIN32
    snprintf(command, sizeof(command), "dir \"%s\" >nul 2>&1", path);  
#else
    snprintf(command, sizeof(command), "ls \"%s\" >/dev/null 2>&1", path);  
#endif

    return system(command) == 0;
}

void create_directory(const char *path) {
    if (!directory_exists(path)) {
#ifdef _WIN32
        char command[256];
        snprintf(command, sizeof(command), "mkdir \"%s\"", path);  
#else
        char command[256];
        snprintf(command, sizeof(command), "mkdir -p \"%s\"", path);  
#endif
        system(command);
    }
}

// Initialisation de la grille 
void init_grid(const char *file_path) {
    if (strcmp(file_path, "") == 0) {
        printf("ici\n");
        for (int x = 0; x < GRID_HEIGHT; x++) {  // Inversé
            for (int y = 0; y < GRID_WIDTH; y++) {  // Inversé
                grid[x][y] = SOL;
            }
        }
    } else {
        printf("%s\n", file_path);
        FILE *file = fopen(file_path, "r");
        if (!file) {
            perror("Erreur lors de l'ouverture du fichier");
            return;
        }

        for (int y = 0; y < GRID_WIDTH; y++) {  // Inversé
            for (int x = 0; x < GRID_HEIGHT; x++) {  // Inversé
                if (fscanf(file, "%d", &grid[x][y]) != 1) {
                    fprintf(stderr, "Erreur lors de la lecture de la grille\n");
                    fclose(file);
                    return;
                }
            }
        }

        fclose(file);

        selectedFile = malloc(strlen(file_path) + 1);
        selectedFile = strcpy(selectedFile, file_path);
    }
}

// Sauvegarde de la grille
void save_grid(SDL_Renderer *renderer) {
    if (test_map(renderer, grid)) {
        create_directory("./data/editor");

        if (!selectedFile) {
            int map_number = 1;
            char new_filename[256];
            FILE *file_check;

            do {
                snprintf(new_filename, sizeof(new_filename), "./data/editor/map%d.txt", map_number);
                file_check = fopen(new_filename, "r");
                if (file_check) {
                    fclose(file_check);
                    map_number++;
                }
            } while (file_check);            
            
            selectedFile = malloc(strlen(new_filename) + 1);
            if (selectedFile) {
                strcpy(selectedFile, new_filename);
            } else {
                fprintf(stderr, "Erreur d'allocation mémoire pour le nom du fichier\n");
                return;
            }
        }

        FILE *file = fopen(selectedFile, "w");
        if (!file) {
            perror("Erreurs lors de l'ouverture du fichier");
            return;
        }

        for (int y = 0; y < GRID_WIDTH; y++) {  // Inversé
            for (int x = 0; x < GRID_HEIGHT; x++) {  // Inversé
                fprintf(file, "%d ", grid[x][y]);
            }
            fprintf(file, "\n");
        }

        fclose(file);

        if (selectedFile) {
            free(selectedFile);
            selectedFile = NULL;
        }
    }
}

// Empile la grille actuelle dans la pile d'annulation
void push_undo() {
    if (undoIndex < UNDO_STACK_SIZE - 1) {
        undoIndex++;
        for (int x = 0; x < GRID_HEIGHT; x++) {  // Inversé
            for (int y = 0; y < GRID_WIDTH; y++) {  // Inversé
                undoStack[undoIndex][x][y] = grid[x][y];
            }
        }
    }
}

// Annule la dernière action
void undo() {
    if (undoIndex >= 0) {
        for (int x = 0; x < GRID_HEIGHT; x++) {  // Inversé
            for (int y = 0; y < GRID_WIDTH; y++) {  // Inversé
                grid[x][y] = undoStack[undoIndex][x][y];
            }
        }
        undoIndex--;
    }
}

// Réinitialise la grille
void reset_grid() {
    push_undo();
    init_grid(selectedFile ? selectedFile : "");
}

int selectedColorIndex = -1;


// Dessine la grille et les boutons
void draw(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    size_t colors_count = get_colors_count();

    for (int x = 0; x < GRID_HEIGHT; x++) {  // Inversé
        for (int y = 0; y < GRID_WIDTH; y++) {  // Inversé
            SDL_Rect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_SetRenderDrawColor(renderer, colors[grid[x][y]].r, colors[grid[x][y]].g, colors[grid[x][y]].b, colors[grid[x][y]].a);
            SDL_RenderFillRect(renderer, &cell);
        }
    }

    for (int i = 0; i < (int)colors_count; i++) {  
        SDL_Rect item_pos = {i * CELL_SIZE, WINDOW_HEIGHT - CELL_SIZE, CELL_SIZE, CELL_SIZE};
        SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
        SDL_RenderFillRect(renderer, &item_pos);

        if (i == selectedColorIndex) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  
            SDL_RenderDrawRect(renderer, &item_pos);  
        }
    }

    SDL_Rect save_button = {WINDOW_WIDTH - 100, WINDOW_HEIGHT - CAROUSEL_HEIGHT, 80, CAROUSEL_HEIGHT - 10};
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderFillRect(renderer, &save_button);
    
    SDL_Rect reset_button = {WINDOW_WIDTH - 200, WINDOW_HEIGHT - CAROUSEL_HEIGHT, 80, CAROUSEL_HEIGHT - 10};
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &reset_button);

    if (font != NULL) {
        SDL_Surface *save_surface = TTF_RenderUTF8_Solid(font, "Save", (SDL_Color){255, 255, 255, 255});
        SDL_Texture *save_texture = SDL_CreateTextureFromSurface(renderer, save_surface);
        SDL_Rect save_rect = {WINDOW_WIDTH - 95, WINDOW_HEIGHT - CAROUSEL_HEIGHT + 5, save_surface->w, save_surface->h};
        SDL_RenderCopy(renderer, save_texture, NULL, &save_rect);
        SDL_FreeSurface(save_surface);
        SDL_DestroyTexture(save_texture);

        SDL_Surface *reset_surface = TTF_RenderUTF8_Solid(font, "Reset", (SDL_Color){255, 255, 255, 255});
        SDL_Texture *reset_texture = SDL_CreateTextureFromSurface(renderer, reset_surface);
        SDL_Rect reset_rect = {WINDOW_WIDTH - 195, WINDOW_HEIGHT - CAROUSEL_HEIGHT + 5, reset_surface->w, reset_surface->h};
        SDL_RenderCopy(renderer, reset_texture, NULL, &reset_rect);
        SDL_FreeSurface(reset_surface);
        SDL_DestroyTexture(reset_texture);
    }

    SDL_RenderPresent(renderer);
}

void handle_mouse_click(int mouseX, int mouseY) {
    size_t colors_count = get_colors_count();

    if (mouseY >= WINDOW_HEIGHT - CELL_SIZE) {
        for (int i = 0; i < (int)colors_count; i++) {  
            if (mouseX >= i * CELL_SIZE && mouseX < (i + 1) * CELL_SIZE) {
                selectedColorIndex = i;  
                break;
            }
        }
    }
}
