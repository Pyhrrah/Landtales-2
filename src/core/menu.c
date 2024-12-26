#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "./../../include/editor/editor.h"
#include "./../../include/multiplayer/multiplayer.h"
#include "./../../include/settings.h"
#include "./../../include/game/game.h"

#define TILE_SIZE 32
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960

// Fonction pour afficher du texte à l'écran
void render_text(SDL_Renderer *renderer, const char *text, int x, int y, TTF_Font *font, SDL_Color color) {
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    
    if (text_texture) {
        SDL_Rect text_location = {x, y, text_surface->w, text_surface->h};
        SDL_RenderCopy(renderer, text_texture, NULL, &text_location);
        SDL_DestroyTexture(text_texture);
    }
    SDL_FreeSurface(text_surface);
}

// Fonction pour afficher le menu
void render_menu(SDL_Renderer *renderer, int selected_option, SDL_Texture *background, TTF_Font *font) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL); 

    render_text(renderer, "Landtales 2", 50, 50, font, white);

    int start_y = 200;
    int spacing = 60;
    const char *labels[] = {"Jouer", "Editeur", "Multijoueur", "Parametres"};

    for (int i = 0; i < 4; i++) {
        SDL_Color color = (i == selected_option) ? white : black;
        render_text(renderer, labels[i], 50, start_y + i * spacing, font, color);
    }

    SDL_RenderPresent(renderer);
}

// Fonction pour gérer le menu
void handle_menu(SDL_Renderer *renderer) {
    int running = 1;
    int selected_option = 0;
    SDL_Event event;

    SDL_Surface *bg_surface = IMG_Load("./assets/images/wallpaperMenu.jpg");
    if (!bg_surface) {
        fprintf(stderr, "Erreur lors du chargement de l'image de fond : %s\n", IMG_GetError());
        return;
    }
    SDL_Texture *background = SDL_CreateTextureFromSurface(renderer, bg_surface);
    SDL_FreeSurface(bg_surface);
    
    if (!background) {
        fprintf(stderr, "Erreur lors de la création de la texture de fond : %s\n", SDL_GetError());
        return;
    }

    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 28);
    if (!font) {
        fprintf(stderr, "Impossible de charger la police : %s\n", TTF_GetError());
        SDL_DestroyTexture(background);
        return;
    }

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_MOUSEMOTION:
                    for (int i = 0; i < 4; i++) {
                        if (event.motion.x >= 50 && event.motion.x <= 250 &&
                            event.motion.y >= 200 + i * 60 && event.motion.y <= 200 + i * 60 + 50) {
                            selected_option = i;
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        switch (selected_option) {
                            case 0:
                                start_game_mode(renderer);
                                break;
                            case 1:
                                start_editor_mode(renderer); 
                                break;
                            case 2:
                                start_multiplayer_mode(renderer); 
                                break;
                            case 3:
                                open_settings(renderer); 
                                break;
                        }
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_UP) {
                        selected_option--;
                        if (selected_option < 0) selected_option = 3;
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        selected_option++;
                        if (selected_option > 3) selected_option = 0;
                    } else if (event.key.keysym.sym == SDLK_RETURN) {
                        switch (selected_option) {
                            case 0:
                                start_game_mode(renderer);
                                break;
                            case 1:
                                start_editor_mode(renderer); 
                                break;
                            case 2:
                                start_multiplayer_mode(renderer); 
                                break;
                            case 3:
                                open_settings(renderer); 
                                break;
                        }
                    }
                    break;
            }
        }

        render_menu(renderer, selected_option, background, font);
        SDL_Delay(100);
    }

    SDL_DestroyTexture(background);
    TTF_CloseFont(font);
}
