#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include "menu.h"
#include "./../../include/game.h"
#include "./../../include/editor.h"
#include "./../../include/multiplayer.h"
#include "./../../include/settings.h"

#define TILE_SIZE 32
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960

void render_text(SDL_Surface *screen, const char *text, int x, int y, TTF_Font *font, SDL_Color color) {
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, color);
    if (text_surface != NULL) {
        SDL_Rect text_location = {x, y, 0, 0}; 
        SDL_BlitSurface(text_surface, NULL, screen, &text_location); 
        SDL_FreeSurface(text_surface); 
    }
}

void render_menu(SDL_Surface *screen, int selected_option) {
    SDL_Color white = {255, 255, 255,0};
    SDL_Color black = {0, 0, 0,0};

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL_ttf : %s\n", TTF_GetError());
        return;
    }

    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 28);
    if (font == NULL) {
        fprintf(stderr, "Impossible de charger la police : %s\n", TTF_GetError());
        return;
    }

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    SDL_Rect play_rect = {WINDOW_WIDTH / 2 - 100, 200, 200, 50};
    SDL_Rect editor_rect = {WINDOW_WIDTH / 2 - 100, 300, 200, 50};
    SDL_Rect multiplayer_rect = {WINDOW_WIDTH / 2 - 100, 400, 200, 50};
    SDL_Rect settings_rect = {WINDOW_WIDTH / 2 - 100, 500, 200, 50};

    SDL_Rect *options[] = {&play_rect, &editor_rect, &multiplayer_rect, &settings_rect};
    const char *labels[] = {"Jouer", "Editeur", "Multijoueur", "Parametres"};

    for (int i = 0; i < 4; i++) {
        SDL_Color color = (i == selected_option) ? white : black;
        render_text(screen, labels[i], options[i]->x, options[i]->y, font, color);
    }

    SDL_Flip(screen);

    TTF_CloseFont(font);
    TTF_Quit();
}

void handle_menu(SDL_Surface *screen) {
    int running = 1;
    int selected_option = 0;

    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
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
                                start_game_mode(screen);
                                break;
                            case 1:
                                start_editor_mode(screen);
                                break;
                            case 2:
                                start_multiplayer_mode(screen);
                                break;
                            case 3:
                                open_settings(screen);
                                break;
                        }
                    }
                    break;
            }
        }

        render_menu(screen, selected_option);
        SDL_Delay(100); 
    }
}
