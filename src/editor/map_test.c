#include "./../../include/editor/map_test.h"
#include "./../../include/editor/colors.h"
#include <stdio.h>
#include "./../../include/core/player.h"
#include "./../../include/utils/sdl_utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

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
int frame = 0;  
char orientation = 'D';  

SDL_Texture *player_texture = NULL;  

int load_player_texture(SDL_Renderer *renderer) {
    char skin_path[100] = "./assets/images/sprite/player/1.png";
    
    player_texture = IMG_LoadTexture(renderer, skin_path);  
    if (!player_texture) {
        printf("Erreur lors du chargement de la texture du joueur : %s\n", SDL_GetError());
        return 0;
    }
    return 1;
}

// Fonction pour vérifier la validité du mouvement
int is_valid_move(int x, int y, int grid[GRID_HEIGHT][GRID_WIDTH]) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
        return 0;  
    }

    if (grid[y][x] == WALL_FRONT || grid[y][x] == WALL_LEFT || grid[y][x] == WALL_RIGHT ||
        grid[y][x] == WATER_BLOCK || grid[y][x] == ROCK || grid[y][x] == DEBRIS || grid[y][x] == STUMP) {
        return 0;  
    }

    return 1;  
}

// Fonction pour dessiner le joueur
void drawPlayer(SDL_Renderer *renderer, int player_x, int player_y, char orientation, int frame) {
    SDL_Rect srcRect, dstRect;
    int frameWidth = 32;  
    int frameHeight = 32; 

    int row = 0; 
    switch (orientation) {
        case 'D': row = 0; break; 
        case 'L': row = 1; break;  
        case 'R': row = 2; break; 
        case 'U': row = 3; break;  
    }


    srcRect.x = (frame % 3) * frameWidth;  
    srcRect.y = row * frameHeight;
    srcRect.w = frameWidth;
    srcRect.h = frameHeight;

    dstRect.x = player_x * CELL_SIZE;
    dstRect.y = player_y * CELL_SIZE;
    dstRect.w = frameWidth;
    dstRect.h = frameHeight;

    SDL_RenderCopy(renderer, player_texture, &srcRect, &dstRect);
}

// Fonction pour dessiner la carte
void draw_test_map(SDL_Renderer *renderer, int grid[GRID_HEIGHT][GRID_WIDTH]) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
    SDL_RenderClear(renderer);

    const int screenHeight = 544;
    const int hudHeight = 544 - (32 * 15);

    static int textures_loaded = 0;
    if (!textures_loaded) {
        load_textures(renderer); 
        if (!load_player_texture(renderer)) {  
            printf("Erreur de chargement de la texture du joueur\n");
            return;
        }
        textures_loaded = 1;
    }

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            SDL_Rect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};

            int cell_id = grid[y][x];

            if (cell_id >= 0 && (unsigned long)cell_id < sizeof(textures) / sizeof(textures[0])) {
                SDL_Texture *current_texture = textures[cell_id];
                if (current_texture != NULL) {
                    SDL_RenderCopy(renderer, current_texture, NULL, &cell);
                    continue; 
                }
            }

            if (cell_id == PLAYER_SPAWN) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  
            } else if (cell_id == TARGET_SPAWN) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  
            } else {
                SDL_SetRenderDrawColor(renderer, colors[cell_id].r, colors[cell_id].g, colors[cell_id].b, colors[cell_id].a);
            }
            SDL_RenderFillRect(renderer, &cell);
        }
    }
        char skin_path[100] = "./assets/images/sprite/player/1.png";
        int leftImageY = screenHeight - hudHeight + 10;

        SDL_Texture *leftImageTexture = IMG_LoadTexture(renderer, skin_path);
        if (!leftImageTexture) {
            printf("Erreur lors du chargement de l'image : %s\n", IMG_GetError());
        } else {
            
            
            SDL_Rect srcRect = {0, 0, 32, 32};  
            SDL_Rect destRect = {25, leftImageY, 48, 48};  

            SDL_RenderCopy(renderer, leftImageTexture, &srcRect, &destRect);

            SDL_DestroyTexture(leftImageTexture);
        }

        // Ecrire du texte sur le reste de la zone disponible : "Rejoignez l'autre spawn pour sauvegarder cette carte."
        TTF_Font *textFooter = load_font("./assets/fonts/font.ttf", 18);
        if (!textFooter) {
            fprintf(stderr, "Impossible de charger la police.\n");
            return;
        }
        SDL_Color textColor = {0, 0, 0, 255};
        render_text(renderer, "Rejoignez l'autre spawn pour sauvegarder cette carte.", 100, leftImageY+10, textFooter, textColor);
    
        drawPlayer(renderer, player_x, player_y, orientation, frame); 
    

    SDL_RenderPresent(renderer); 
}

// Test de la carte
int test_map(SDL_Renderer *renderer, int grid[GRID_HEIGHT][GRID_WIDTH]) {
    int found_spawn = 0, target_spawn = 0;

    // Recherche des positions de spawn du joueur et de la cible
    for (int y = 0; y < GRID_HEIGHT; y++) {  
        for (int x = 0; x < GRID_WIDTH; x++) { 
            if (grid[y][x] == PLAYER_SPAWN) {
                found_spawn = 1;
                player_x = x;  
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
    for(int i = 0; i < GRID_HEIGHT; i++) {
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
                } else if (event.key.keysym.sym == SDLK_z) {
                    if (is_valid_move(player_x, player_y - 1, grid)) {
                        player_y--;
                        frame = (frame + 1) % 3;
                        orientation = 'U';
                    }
                } else if (event.key.keysym.sym == SDLK_s) {
                    if (is_valid_move(player_x, player_y + 1, grid)) {
                        player_y++; 
                        frame = (frame + 1) % 3;
                        orientation = 'D';
                    }
                } else if (event.key.keysym.sym == SDLK_q) {
                    if (is_valid_move(player_x - 1, player_y, grid)) {
                        player_x--; 
                        frame = (frame + 1) % 3;
                        orientation = 'L';
                    }
                } else if (event.key.keysym.sym == SDLK_d) {
                    if (is_valid_move(player_x + 1, player_y, grid)) {
                        player_x++; 
                        frame = (frame + 1) % 3;
                        orientation = 'R';
                    }
                }
            }
        }

        // Si le joueur atteint la cible, retourner 1
        if (grid[player_y][player_x] == TARGET_SPAWN) {
            return 1;
        }

        SDL_Delay(16);
    }

    return 0;
}