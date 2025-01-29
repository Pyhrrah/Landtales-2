#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./../../include/utils/video.h"
#include "./../../include/editor/editor.h"
#include "./../../include/multiplayer/multiplayer.h"
#include "./../../include/settings.h"
#include "./../../include/game/game.h"
#include "./../../include/utils/sdl_utils.h"
#define TILE_SIZE 32
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960
#define KONAMI_LENGTH 7 

const SDL_Keycode konami_code[KONAMI_LENGTH] = {
    SDLK_t, SDLK_r, SDLK_a, SDLK_n, SDLK_c, SDLK_h, SDLK_o
};

SDL_Keycode konami_input[KONAMI_LENGTH];

int check_konami_code() {
    for (int i = 0; i < KONAMI_LENGTH; i++) {
        if (konami_input[i] != konami_code[i]) {
            return 0;  
        }
    }
    return 1;  
}


void render_menu(SDL_Renderer *renderer, int selected_option, SDL_Texture *background, TTF_Font *font) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    SDL_RenderClear(renderer);
    render_texture(renderer, background, 0, 0, 800, 600); // Utilisation de render_texture

    int start_y = 200;
    int spacing = 60;
    const char *labels[] = {"Jouer", "Editeur", "Multijoueur", "Paramètres"};

    for (int i = 0; i < 4; i++) {
        SDL_Color color = (i == selected_option) ? white : black;
        render_text(renderer, labels[i], 50, start_y + i * spacing, font, color);
    }

    SDL_RenderPresent(renderer);
}

void handle_menu(SDL_Renderer *renderer) {
    int running = 1;
    int selected_option = 0;
    SDL_Event event;
    int konami_index = 0;

    // Chargement de la texture d'arrière-plan
    SDL_Texture *background = load_texture(renderer, "./assets/images/fond.png");
    if (!background) {
        fprintf(stderr, "Erreur lors du chargement de la texture de fond.\n");
        return;
    }

    // Chargement de la police
    TTF_Font *font = load_font("./assets/fonts/font.ttf", 28);
    if (!font) {
        fprintf(stderr, "Impossible de charger la police.\n");
        SDL_DestroyTexture(background);
        return;
    }

    int konami_did = 0;
    int music_played = 0;  

    if (init_audio() < 0) {
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
                    konami_input[konami_index] = event.key.keysym.sym;
                    konami_index = (konami_index + 1) % KONAMI_LENGTH;

                    if (check_konami_code() && konami_did == 0) {
                        Mix_Music *music = NULL;
                        if (!music_played) {
                            music = play_audio("./assets/music/audio_video.mp3"); 
                            music_played = 1;
                        }
                        play_video("./assets/video/video.mp4", renderer);
                        cleanup_audio(music);
                        konami_did = 1;

                        running = 0;
                    }

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

    // Nettoyage des ressources
    SDL_DestroyTexture(background);
    TTF_CloseFont(font);
}

