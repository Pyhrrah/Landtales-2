#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "./../../include/map_test.h"
#include "./../../include/colors.h"

#define WINDOW_WIDTH 672
#define WINDOW_HEIGHT 544
#define CELL_SIZE 32
#define GRID_WIDTH 21  
#define GRID_HEIGHT 15  
#define CAROUSEL_HEIGHT 64
#define UNDO_STACK_SIZE 100
#define EMPTY -1  

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



int grid[GRID_WIDTH][GRID_HEIGHT];
int currentObjectID = SOL; 
int isDragging = 0;
int undoStack[UNDO_STACK_SIZE][GRID_WIDTH][GRID_HEIGHT];
int undoIndex = -1;
TTF_Font *font = NULL;

void init_grid(const char *file_path) {
    if (file_path == NULL) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                grid[x][y] = SOL;
            }
        }
    } else {
        FILE *file = fopen(file_path, "r");
        if (!file) {
            perror("Erreur lors de l'ouverture du fichier");
            return;
        }

        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (fscanf(file, "%d", &grid[x][y]) != 1) {
                    fprintf(stderr, "Erreur lors de la lecture de la grille\n");
                    fclose(file);
                    return;
                }
            }
        }

        fclose(file);
        printf("Grille chargée depuis %s\n", file_path);
    }
}

void save_grid(SDL_Renderer *renderer) {
    if (test_map(renderer, grid)) {
        struct stat st = {0};
        if (stat("./data/editor", &st) == -1) {
            mkdir("./data/editor", 0700);
        }

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
            perror("Erreur lors de l'ouverture du fichier");
            return;
        }

        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                fprintf(file, "%d ", grid[x][y]);
            }
            fprintf(file, "\n");
        }

        fclose(file);
        printf("Grille sauvegardée dans %s\n", selectedFile);
    }
}



void push_undo() {
    if (undoIndex < UNDO_STACK_SIZE - 1) {
        undoIndex++;
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                undoStack[undoIndex][x][y] = grid[x][y];
            }
        }
    }
}

void undo() {
    if (undoIndex >= 0) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                grid[x][y] = undoStack[undoIndex][x][y];
            }
        }
        undoIndex--;
    }
}

void reset_grid() {
    push_undo();
    init_grid(selectedFile ? selectedFile : "");
}

void draw(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

size_t colors_count = get_colors_count();

    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            SDL_Rect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_SetRenderDrawColor(renderer, colors[grid[x][y]].r, colors[grid[x][y]].g, colors[grid[x][y]].b, colors[grid[x][y]].a);
            SDL_RenderFillRect(renderer, &cell);
        }
    }

    for (size_t i = 0; i < colors_count; i++) {
        SDL_Rect item_pos = {i * CELL_SIZE, WINDOW_HEIGHT - CELL_SIZE, CELL_SIZE, CELL_SIZE};
        SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
        SDL_RenderFillRect(renderer, &item_pos);
    }

    SDL_Rect save_button = {WINDOW_WIDTH - 100, WINDOW_HEIGHT - CAROUSEL_HEIGHT, 80, CAROUSEL_HEIGHT - 10};
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderFillRect(renderer, &save_button);
    
    SDL_Rect reset_button = {WINDOW_WIDTH - 200, WINDOW_HEIGHT - CAROUSEL_HEIGHT, 80, CAROUSEL_HEIGHT - 10};
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &reset_button);

    if (font != NULL) {
        SDL_Surface *save_surface = TTF_RenderText_Solid(font, "Save", (SDL_Color){255, 255, 255, 255});
        SDL_Texture *save_texture = SDL_CreateTextureFromSurface(renderer, save_surface);
        SDL_Rect save_rect = {WINDOW_WIDTH - 95, WINDOW_HEIGHT - CAROUSEL_HEIGHT + 5, save_surface->w, save_surface->h};
        SDL_RenderCopy(renderer, save_texture, NULL, &save_rect);
        SDL_FreeSurface(save_surface);
        SDL_DestroyTexture(save_texture);

        SDL_Surface *reset_surface = TTF_RenderText_Solid(font, "Reset", (SDL_Color){255, 255, 255, 255});
        SDL_Texture *reset_texture = SDL_CreateTextureFromSurface(renderer, reset_surface);
        SDL_Rect reset_rect = {WINDOW_WIDTH - 195, WINDOW_HEIGHT - CAROUSEL_HEIGHT + 5, reset_surface->w, reset_surface->h};
        SDL_RenderCopy(renderer, reset_texture, NULL, &reset_rect);
        SDL_FreeSurface(reset_surface);
        SDL_DestroyTexture(reset_texture);
    }

    SDL_RenderPresent(renderer);
}

char* show_menu(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect new_room_button = {WINDOW_WIDTH - 250, 20, 200, 60}; 
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    SDL_RenderFillRect(renderer, &new_room_button);

    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 28);

    if (font != NULL) {
        SDL_Color textColor = {255, 255, 255, 255}; 

        SDL_Surface *textSurface = TTF_RenderUTF8_Blended_Wrapped(font, "Créer une Map", textColor, new_room_button.w);
        if (textSurface != NULL) {
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {
                new_room_button.x + (new_room_button.w - textSurface->w) / 2,
                new_room_button.y + (new_room_button.h - textSurface->h) / 2,
                textSurface->w,
                textSurface->h
            };
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        } else {
            fprintf(stderr, "Erreur lors du rendu du texte : %s\n", TTF_GetError());
        }

        FILE *fp = popen("ls ./data/editor", "r");
        if (fp == NULL) {
            fprintf(stderr, "Erreur lors de l'ouverture du dossier\n");
            TTF_CloseFont(font);
            return NULL;
        }

        char filename[256];
        int y_offset = 100;
        SDL_Rect fileRects[100];
        SDL_Rect delete_buttons[100]; 
        char fileNames[100][256];
        int fileCount = 0;


        while (fgets(filename, sizeof(filename), fp) != NULL) {
            filename[strcspn(filename, "\n")] = 0; 
            strncpy(fileNames[fileCount], filename, 256);

            SDL_Surface *fileSurface = TTF_RenderUTF8_Blended_Wrapped(font, filename, textColor, 200); 
            if (fileSurface != NULL) {
                SDL_Texture *fileTexture = SDL_CreateTextureFromSurface(renderer, fileSurface);
                SDL_Rect fileRect = {50, y_offset, fileSurface->w, fileSurface->h};
                fileRects[fileCount] = fileRect;
                SDL_RenderCopy(renderer, fileTexture, NULL, &fileRect);
                SDL_FreeSurface(fileSurface);
                SDL_DestroyTexture(fileTexture);
            } else {
                fprintf(stderr, "Erreur lors du rendu du texte de fichier : %s\n", TTF_GetError());
            }

            delete_buttons[fileCount] = (SDL_Rect) {WINDOW_WIDTH - 100, y_offset, 80, 30}; 
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
            SDL_RenderFillRect(renderer, &delete_buttons[fileCount]);

            SDL_Surface *deleteTextSurface = TTF_RenderUTF8_Blended_Wrapped(font, "Effacer", textColor, delete_buttons[fileCount].w);
            if (deleteTextSurface != NULL) {
                SDL_Texture *deleteTextTexture = SDL_CreateTextureFromSurface(renderer, deleteTextSurface);
                SDL_Rect deleteTextRect = {
                    delete_buttons[fileCount].x + (delete_buttons[fileCount].w - deleteTextSurface->w) / 2,
                    delete_buttons[fileCount].y + (delete_buttons[fileCount].h - deleteTextSurface->h) / 2,
                    deleteTextSurface->w,
                    deleteTextSurface->h
                };
                SDL_RenderCopy(renderer, deleteTextTexture, NULL, &deleteTextRect);
                SDL_FreeSurface(deleteTextSurface);
                SDL_DestroyTexture(deleteTextTexture);
            } else {
                fprintf(stderr, "Erreur lors du rendu du texte 'Effacer' : %s\n", TTF_GetError());
            }

            y_offset += 50;
            fileCount++;
        }
        pclose(fp);

        SDL_RenderPresent(renderer);

        char *selectedFile = NULL;
        int running = 1;

        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = 0;
                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int x = event.button.x;
                    int y = event.button.y;

                    if (event.button.button == SDL_BUTTON_LEFT &&
                        x >= new_room_button.x && x <= new_room_button.x + new_room_button.w &&
                        y >= new_room_button.y && y <= new_room_button.y + new_room_button.h) {
                        running = 0; 
                    }

                    for (int i = 0; i < fileCount; i++) {
                        if (x >= fileRects[i].x && x <= fileRects[i].x + fileRects[i].w &&
                            y >= fileRects[i].y && y <= fileRects[i].y + fileRects[i].h) {
                            selectedFile = malloc(strlen("./data/editor/") + strlen(fileNames[i]) + 1);
                            sprintf(selectedFile, "./data/editor/%s", fileNames[i]);
                            running = 0;
                            break;
                        }

                        if (x >= delete_buttons[i].x && x <= delete_buttons[i].x + delete_buttons[i].w &&
                            y >= delete_buttons[i].y && y <= delete_buttons[i].y + delete_buttons[i].h) {
                            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Confirmation", "Êtes-vous sûr de vouloir supprimer ce fichier?", NULL);
                        }
                    }
                }
            }
        }

        TTF_CloseFont(font);
        return selectedFile;
    } else {
        fprintf(stderr, "Erreur lors de l'ouverture de la police : %s\n", TTF_GetError());
    }

    return NULL;
}

void handle_editor_events(SDL_Renderer *renderer, int *running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        int x = event.button.x / CELL_SIZE;
        int y = event.button.y / CELL_SIZE;

        if (event.type == SDL_QUIT) {
            *running = 0;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_z && (SDL_GetModState() & KMOD_CTRL)) {
                undo();
            } else if (event.key.keysym.sym == SDLK_s) {
                save_grid(renderer);
            } else if (event.key.keysym.sym == SDLK_r) {
                reset_grid();
            }
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                SDL_Rect save_button = {WINDOW_WIDTH - 100, WINDOW_HEIGHT - CAROUSEL_HEIGHT, 80, CAROUSEL_HEIGHT - 10};
                SDL_Rect reset_button = {WINDOW_WIDTH - 200, WINDOW_HEIGHT - CAROUSEL_HEIGHT, 80, CAROUSEL_HEIGHT - 10};

                if (event.button.y > WINDOW_HEIGHT - CAROUSEL_HEIGHT) {
                    if (event.button.x >= save_button.x && event.button.x <= save_button.x + save_button.w) {
                        save_grid(renderer);
                    } else if (event.button.x >= reset_button.x && event.button.x <= reset_button.x + reset_button.w) {
                        reset_grid();
                    } else {
                        currentObjectID = event.button.x / CELL_SIZE;
                    }
                } else {
                    int found_player1 = 0, found_player2 = 0;
                    int found_block_12 = 0, found_block_13 = 0;

                    for (int i = 0; i < GRID_WIDTH; i++) {
                        for (int j = 0; j < GRID_HEIGHT; j++) {
                            if (grid[i][j] == SPAWN_PLAYER_1) found_player1 = 1;
                            if (grid[i][j] == SPAWN_PLAYER_2) found_player2 = 1;
                            if (grid[i][j] == 12) found_block_12 = 1; 
                            if (grid[i][j] == 13) found_block_13 = 1; 
                        }
                    }

                    if (((currentObjectID == SPAWN_PLAYER_1 && !found_player1) || 
                        (currentObjectID == SPAWN_PLAYER_2 && !found_player2) || 
                        (currentObjectID != SPAWN_PLAYER_1 && currentObjectID != SPAWN_PLAYER_2)) &&
                        !(found_block_12 && (currentObjectID == 12)) &&
                        !(found_block_13 && (currentObjectID == 13))) {
                        isDragging = 1;
                        push_undo();
                        grid[x][y] = currentObjectID; 
                    }
                }
            }
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                isDragging = 0;
            }
        } else if (event.type == SDL_MOUSEMOTION) {
            if (isDragging) {
                grid[x][y] = currentObjectID;
            }
        }
    }
}

void start_editor_mode(SDL_Renderer *renderer) {
    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur lors de l'initialisation de TTF: %s\n", TTF_GetError());
        return;
    }

    font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 28);
    if (!font) {
        fprintf(stderr, "Erreur lors de l'ouverture de la police: %s\n", TTF_GetError());
        return;
    }

    selectedFile = show_menu(renderer);

    init_grid(selectedFile);
    
    
    printf("Mode éditeur lancé\n");

    int running = 1;
    while (running) {
        handle_editor_events(renderer, &running);
        draw(renderer);
    }

    free(selectedFile);

    TTF_CloseFont(font);
    TTF_Quit();
}

