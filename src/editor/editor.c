#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./../../include/editor/map_test.h"
#include "./../../include/editor/grid.h"
#include "./../../include/plugins/open_map_editor.h"
#include "./../../include/utils/sdl_utils.h"

#define WINDOW_WIDTH 672
#define WINDOW_HEIGHT 544
#define CELL_SIZE 32
#define GRID_WIDTH 21
#define GRID_HEIGHT 15
#define CAROUSEL_HEIGHT 64
#define UNDO_STACK_SIZE 100
#define EMPTY -1  

// Initialisation de la grille
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

int currentObjectID = SOL; 
int isDragging = 0;
TTF_Font *font = NULL;
int scrollOffset;


// Initialisation de la grille
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

                    for (int i = 0; i < GRID_HEIGHT; i++) {
                        for (int j = 0; j < GRID_WIDTH; j++) {
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
                        grid[y][x] = currentObjectID; 
                    }
                }
            }
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                isDragging = 0;
            }
        } else if (event.type == SDL_MOUSEMOTION) {
            if (isDragging) {
                grid[y][x] = currentObjectID;  
            }
        }
    }
}



// Dessine la grille et les boutons
void launch_editor(SDL_Renderer *renderer) {
    draw(renderer);

    int running = 1;
    while (running) {
        handle_editor_events(renderer, &running);

        draw(renderer);  
    }
}


// Affichage de la liste des fichiers
int render_file_list(SDL_Renderer *renderer, TTF_Font *font, SDL_Rect *fileRects, SDL_Rect *deleteButtons, char fileNames[100][256], int *scrollOffset) {
    SDL_Color textColor = {255, 255, 255, 255};

    system("mkdir -p ./data/editor");

    FILE *fp = popen("ls ./data/editor", "r");
    if (!fp) {
        fprintf(stderr, "Erreur lors de l'ouverture du dossier\n");
        return 0;
    }

    int scroll_start_y = 100;
    int scroll_end_y = WINDOW_HEIGHT - 100;
    int visible_height = scroll_end_y - scroll_start_y;
    int file_spacing = 50;

    char filename[256];
    int y_offset = scroll_start_y - *scrollOffset; 
    int fileCount = 0;
    int maxWidth = WINDOW_WIDTH - 150;

    while (fgets(filename, sizeof(filename), fp) != NULL) {
        filename[strcspn(filename, "\n")] = 0;  
        strncpy(fileNames[fileCount], filename, 256);

        SDL_Surface *fileSurface = TTF_RenderUTF8_Blended_Wrapped(font, filename, textColor, maxWidth);
        if (fileSurface) {
            SDL_Texture *fileTexture = SDL_CreateTextureFromSurface(renderer, fileSurface);

            if (y_offset >= scroll_start_y && y_offset + fileSurface->h <= scroll_end_y) {
                SDL_Rect fileRect = {50, y_offset, fileSurface->w, fileSurface->h};
                fileRects[fileCount] = fileRect;
                SDL_RenderCopy(renderer, fileTexture, NULL, &fileRect);
            }

            SDL_FreeSurface(fileSurface);
            SDL_DestroyTexture(fileTexture);
        }

        SDL_Texture *buttonTexture = load_texture(renderer, "./assets/images/boutons/Supprimer.png");

        if (y_offset >= scroll_start_y && y_offset + 30 <= scroll_end_y) {
            deleteButtons[fileCount] = (SDL_Rect) {WINDOW_WIDTH - 100, y_offset, 80, 30};

            if (buttonTexture) {
                // Si la texture est chargée, dessiner l'image
                render_texture(renderer, buttonTexture, deleteButtons[fileCount].x, deleteButtons[fileCount].y, 
                            deleteButtons[fileCount].w, deleteButtons[fileCount].h);
            } else {
                // Sinon, dessiner un rectangle rouge
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer, &deleteButtons[fileCount]);
            }
        }

        // Libération de la texture à la fin du programme
        if (buttonTexture) {
            SDL_DestroyTexture(buttonTexture);
        }

        y_offset += file_spacing;
        fileCount++;

        if (y_offset > scroll_end_y) {
            break;
        }
    }

    pclose(fp);

    int total_list_height = fileCount * file_spacing;

    if (total_list_height <= visible_height) {
        *scrollOffset = 0;  
    } else if (*scrollOffset > total_list_height - visible_height) {
        *scrollOffset = total_list_height - visible_height;  
    }

    SDL_Rect clip_rect = {0, scroll_start_y, WINDOW_WIDTH, scroll_end_y - scroll_start_y};
    SDL_RenderSetClipRect(renderer, &clip_rect);
    SDL_RenderSetClipRect(renderer, NULL);

    return fileCount;
}

// Gestion des événements du menu
char* handle_menu_events(SDL_Renderer *renderer, SDL_Rect *new_room_button, SDL_Rect *fileRects, SDL_Rect *deleteButtons, char fileNames[100][256], int *fileCount) {
    SDL_Event event;
    char *fichier = NULL;  
    int running = 1;

    SDL_Rect quit_button = {WINDOW_WIDTH - 150, WINDOW_HEIGHT - 100, 120, 40};
    SDL_Rect generation_button = {20, 20, 200, 60};  // Bouton "Génération"

    // Charger les textures des boutons avec la fonction load_texture
    SDL_Texture* newRoomButtonTexture = load_texture(renderer, "./assets/images/boutons/NouvelleMap.png");
    SDL_Texture* quitButtonTexture = load_texture(renderer, "./assets/images/boutons/Quitter.png");
    SDL_Texture* generationButtonTexture = load_texture(renderer, "./assets/images/boutons/Generation.png");

    if (!newRoomButtonTexture || !quitButtonTexture || !generationButtonTexture) {
        fprintf(stderr, "Erreur lors du chargement des images des boutons\n");
        return NULL;
    }

    SDL_Texture* backgroundTexture = load_texture(renderer, "./assets/images/fondVierge.png");
    if (!backgroundTexture) {
        fprintf(stderr, "Erreur de chargement de l'image de fond : %s\n", IMG_GetError());
        return NULL;
    }

    while (running) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

        render_texture(renderer, newRoomButtonTexture, new_room_button->x, new_room_button->y, new_room_button->w, new_room_button->h);
        render_texture(renderer, quitButtonTexture, quit_button.x, quit_button.y, quit_button.w, quit_button.h);
        render_texture(renderer, generationButtonTexture, generation_button.x, generation_button.y, generation_button.w, generation_button.h);

        render_file_list(renderer, font, fileRects, deleteButtons, fileNames, &scrollOffset);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;

                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (x >= new_room_button->x && x <= new_room_button->x + new_room_button->w &&
                        y >= new_room_button->y && y <= new_room_button->y + new_room_button->h) {
                        fichier = "nouvelle_map";
                        printf("Bouton 'Créer une Map' cliqué\n");
                        running = 0;  
                    }

                    if (x >= generation_button.x && x <= generation_button.x + generation_button.w &&
                        y >= generation_button.y && y <= generation_button.y + generation_button.h) {
                        fichier = "generation";
                        printf("Bouton 'Génération' cliqué\n");
                        running = 0;  
                    }

                    for (int i = 0; i < *fileCount; i++) {
                        if (x >= fileRects[i].x && x <= fileRects[i].x + fileRects[i].w &&
                            y >= fileRects[i].y && y <= fileRects[i].y + fileRects[i].h) {
                            if (fichier) free(fichier);
                            fichier = malloc(strlen("./data/editor/") + strlen(fileNames[i]) + 1);
                            sprintf(fichier, "./data/editor/%s", fileNames[i]);  
                            printf("Fichier sélectionné : %s\n", fichier);
                            running = 0;  
                            break;
                        }

                        if (x >= deleteButtons[i].x && x <= deleteButtons[i].x + deleteButtons[i].w &&
                            y >= deleteButtons[i].y && y <= deleteButtons[i].y + deleteButtons[i].h) {
                            SDL_MessageBoxButtonData buttons[2] = {
                                {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Oui"},
                                {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Non"}
                            };

                            SDL_MessageBoxData messageboxdata = {
                                SDL_MESSAGEBOX_INFORMATION,
                                NULL,
                                "Confirmation",
                                "Êtes-vous sûr de vouloir supprimer ce fichier ?",
                                SDL_arraysize(buttons),
                                buttons,
                                NULL
                            };

                            int button_id;
                            SDL_ShowMessageBox(&messageboxdata, &button_id);

                            if (button_id == 0) {  
                                char filePath[512];
                                sprintf(filePath, "./data/editor/%s", fileNames[i]);
                                if (remove(filePath) == 0) {
                                    printf("Fichier '%s' supprimé avec succès.\n", fileNames[i]);
                                    *fileCount = render_file_list(renderer, font, fileRects, deleteButtons, fileNames, &scrollOffset);  
                                } else {
                                    printf("Erreur lors de la suppression du fichier '%s'.\n", fileNames[i]);
                                }
                                break;  
                            }
                        }
                    }

                    if (x >= quit_button.x && x <= quit_button.x + quit_button.w &&
                        y >= quit_button.y && y <= quit_button.y + quit_button.h) {
                        running = 0; 
                    }
                }
            } else if (event.type == SDL_MOUSEWHEEL) {
                if (event.wheel.y > 0) {
                    scrollOffset -= 50; 
                    if (scrollOffset < 0) scrollOffset = 0;  
                } else if (event.wheel.y < 0) {
                    scrollOffset += 50; 
                }

                *fileCount = render_file_list(renderer, font, fileRects, deleteButtons, fileNames, &scrollOffset);
            }
        }
    }

    SDL_DestroyTexture(newRoomButtonTexture);
    SDL_DestroyTexture(quitButtonTexture);
    SDL_DestroyTexture(generationButtonTexture);

    SDL_DestroyTexture(backgroundTexture);

    return fichier;  
}

// Affichage du menu
void show_menu(SDL_Renderer *renderer) {
    SDL_Surface* backgroundSurface = IMG_Load("./assets/images/fondVierge.png");
    if (!backgroundSurface) {
        fprintf(stderr, "Erreur de chargement de l'image de fond : %s\n", IMG_GetError());
        return;
    }
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);


    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  
    SDL_RenderClear(renderer);


    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    // Charger la police en utilisant la fonction load_font
    TTF_Font *font = load_font("./assets/fonts/font.ttf", 28);
    if (!font) {
        fprintf(stderr, "Erreur lors de l'ouverture de la police\n");
        SDL_DestroyTexture(backgroundTexture); // Nettoyer la texture en cas d'erreur
        return;
    }

    // Définir le bouton "Nouvelle salle"
    SDL_Rect new_room_button = {WINDOW_WIDTH - 250, 20, 200, 60};

    // Variables pour la liste des fichiers
    SDL_Rect fileRects[100], deleteButtons[100];
    char fileNames[100][256];
    int fileCount = 0;
    int scrollOffset = 0; // Ajouter une variable pour gérer le défilement

    fileCount = render_file_list(renderer, font, fileRects, deleteButtons, fileNames, &scrollOffset);

    SDL_RenderPresent(renderer);

    // Gérer les événements du menu
    char *selectedFile = handle_menu_events(renderer, &new_room_button, fileRects, deleteButtons, fileNames, &fileCount);

    // Fermer la police
    TTF_CloseFont(font);

    SDL_DestroyTexture(backgroundTexture);

    if (selectedFile) {
        if (strcmp(selectedFile, "nouvelle_map") == 0) {
            init_grid("");  
            launch_editor(renderer);
        } else if (strcmp(selectedFile, "generation") == 0) {
            load_and_run_plugin(grid);
            launch_editor(renderer);
        } else {
            printf("Fichier sélectionné : %s\n", selectedFile);
            init_grid(selectedFile);  
            launch_editor(renderer);  
        }
    } else {
        free(selectedFile);
        selectedFile = NULL;
        printf("Menu quitté sans sélection\n");
    }
}

// Lancement du mode éditeur
void start_editor_mode(SDL_Renderer *renderer) {
    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur lors de l'initialisation de TTF: %s\n", TTF_GetError());
        return;
    }

    font = TTF_OpenFont("./assets/fonts/font.ttf", 28);
    if (!font) {
        fprintf(stderr, "Erreur lors de l'ouverture de la police: %s\n", TTF_GetError());
        return;
    }

    show_menu(renderer);

    printf("Mode éditeur lancé\n");

    TTF_CloseFont(font);
    TTF_Quit();
}

