#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./../../include/editor/grid.h"
#include "./../../include/editor/map_test.h"
#include "./../../include/editor/colors.h"
#include "./../../include/utils/sdl_utils.h"

#define GRID_WIDTH 21  
#define GRID_HEIGHT 15  
#define CELL_SIZE 32
#define UNDO_STACK_SIZE 100
#define WINDOW_WIDTH 672
#define WINDOW_HEIGHT 544
#define CAROUSEL_HEIGHT 64

int grid[GRID_HEIGHT][GRID_WIDTH];  // Grille de 15x21
int undoStack[UNDO_STACK_SIZE][GRID_HEIGHT][GRID_WIDTH];
int undoIndex = -1;
char *selectedFile = NULL;


/*

File grid.c, this file contains the functions to manage the grid of the game

*/

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
int currentMapNumber = 0;

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
        // Si aucun fichier n'est spécifié, initialiser la grille vide
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                grid[y][x] = 0;
            }
        }
        currentMapNumber = 0;  // Aucune map chargée
    } else {
        if (sscanf(file_path, "./data/editor/map%d.txt", &currentMapNumber) != 1) {
            fprintf(stderr, "Erreur : Impossible d'extraire le numéro de la map du fichier %s\n", file_path);
            currentMapNumber = 0;  
        }

        FILE *file = fopen(file_path, "r");
        if (!file) {
            perror("Erreur lors de l'ouverture du fichier");
            return;
        }

        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (fscanf(file, "%d", &grid[y][x]) != 1) {
                    fprintf(stderr, "Erreur lors de la lecture de la grille\n");
                    fclose(file);
                    return;
                }
            }
        }

        fclose(file);
    }
}

void save_grid(SDL_Renderer *renderer) {
    if (test_map(renderer, grid)) {
        create_directory("./data/editor");

        char filename[256];

        if (currentMapNumber > 0) {
            // Réécrire le fichier existant
            snprintf(filename, sizeof(filename), "./data/editor/map%d.txt", currentMapNumber);
        } else {
            // Trouver un nouveau numéro de map disponible
            int map_number = 1;
            FILE *file_check;

            do {
                snprintf(filename, sizeof(filename), "./data/editor/map%d.txt", map_number);
                file_check = fopen(filename, "r");
                if (file_check) {
                    fclose(file_check);
                    map_number++;
                }
            } while (file_check);

            currentMapNumber = map_number;  // Mettre à jour le numéro de la map
        }

        FILE *file = fopen(filename, "w");
        if (!file) {
            perror("Erreur lors de l'ouverture du fichier");
            return;
        }

        // Écrire la grille dans le fichier
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                fprintf(file, "%d ", grid[y][x]);
            }
            fprintf(file, "\n");
        }

        fclose(file);
        printf("Fichier sauvegardé avec succès : %s\n", filename);
    }
}

// Empile la grille actuelle dans la pile d'annulation
void push_undo() {
    if (undoIndex < UNDO_STACK_SIZE - 1) {
        undoIndex++;
        for (int y = 0; y < GRID_HEIGHT; y++) {  
            for (int x = 0; x < GRID_WIDTH; x++) {  
                undoStack[undoIndex][y][x] = grid[y][x]; 
            }
        }
    }
}

// Annule la dernière action
void undo() {
    if (undoIndex >= 0) {
        for (int y = 0; y < GRID_HEIGHT; y++) {  
            for (int x = 0; x < GRID_WIDTH; x++) {  
                grid[y][x] = undoStack[undoIndex][y][x];
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
    static int textures_loaded = 0; // Charger les textures une seule fois
    if (!textures_loaded) {
        load_textures(renderer);
        textures_loaded = 1;
    }

    // Effacer l'écran avec une couleur de fond
    SDL_RenderClear(renderer);

    size_t colors_count = get_colors_count();

    // Dessiner la grille
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            SDL_Rect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            int cell_id = grid[y][x];

            if (cell_id < 0 || (unsigned long)cell_id >= sizeof(textures) / sizeof(textures[0])) {
                continue;
            }

            SDL_Texture *current_texture = textures[cell_id];
            SDL_Color current_color = colors[cell_id];

            if (current_texture != NULL) {
                render_texture(renderer, current_texture, cell.x, cell.y, cell.w, cell.h);
            } else {
                SDL_SetRenderDrawColor(renderer, current_color.r, current_color.g, current_color.b, current_color.a);
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }

    // Dessiner la légende en bas (couleurs et sprites)
    for (int i = 0; i < (int)colors_count; i++) {
        SDL_Rect item_pos = {i * CELL_SIZE, WINDOW_HEIGHT - CELL_SIZE, CELL_SIZE, CELL_SIZE};
        SDL_Texture *legend_texture = (i < (int)(sizeof(textures) / sizeof(textures[0]))) ? textures[i] : NULL;

        if (legend_texture != NULL) {
            render_texture(renderer, legend_texture, item_pos.x, item_pos.y, item_pos.w, item_pos.h);
        } else {
            SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
            SDL_RenderFillRect(renderer, &item_pos);
        }

        if (i == selectedColorIndex) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  
            SDL_RenderDrawRect(renderer, &item_pos);  
        }
    }

    SDL_Rect save_button = {WINDOW_WIDTH - 100, WINDOW_HEIGHT - 50, 90, 40};
    SDL_Rect reset_button = {WINDOW_WIDTH - 200, WINDOW_HEIGHT - 50, 80, 40};
    
    SDL_Color buttonTextColor = {255, 255, 255, 255};  
    SDL_Color saveButtonColor = {150, 150, 150, 255};  
    SDL_Color resetButtonColor = {100, 100, 100, 255};
    
    render_button(renderer, &save_button, "Tester", buttonTextColor, saveButtonColor, font);
    render_button(renderer, &reset_button, "Reset", buttonTextColor, resetButtonColor, font);

    // Afficher tout ce qui a été dessiné
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
