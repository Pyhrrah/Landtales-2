#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./../../include/utils/video.h"
#include "./../../include/editor/editor.h"
#include "./../../include/multiplayer/multiplayer.h"
#include "./../../include/game/game.h"
#include "./../../include/utils/sdl_utils.h"
#define TILE_SIZE 32
#define WINDOW_WIDTH 672
#define WINDOW_HEIGHT 544
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


void render_menu(SDL_Renderer *renderer, int selected_option, SDL_Texture *background, TTF_Font *font, int hover_link) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color grey = {219, 219, 219, 255}; 

    SDL_RenderClear(renderer);
    render_texture(renderer, background, 0, 0, 800, 600); 

    int start_y = 200;
    int spacing = 60;
    const char *labels[] = {"Jouer", "Editeur", "Multijoueur", "Quitter"};

    for (int i = 0; i < 4; i++) {
        SDL_Color color = (i == selected_option) ? white : black;
        render_text(renderer, labels[i], 50, start_y + i * spacing, font, color);
    }

    TTF_Font *fontFooter = load_font("./assets/fonts/font.ttf", 18);
    if (!fontFooter) {
        fprintf(stderr, "Impossible de charger la police.\n");
        SDL_DestroyTexture(background);
        return;
    }

    render_text(renderer, "© Landtales 2 - Tous droits réservés.", 20, WINDOW_HEIGHT - 40, fontFooter, grey);
    render_text(renderer, "Lien du repo ici", 500, WINDOW_HEIGHT - 40, fontFooter, hover_link ? grey : white); 

    SDL_RenderPresent(renderer);
}

void handle_menu(SDL_Renderer *renderer) {
    int running = 1;
    int selected_option = 0;
    SDL_Event event;
    int konami_index = 0;
    int hover_link = 0;  // Indique si la souris est sur le lien

    SDL_Texture *background = load_texture(renderer, "./assets/images/fond.png");
    if (!background) {
        fprintf(stderr, "Erreur lors du chargement de la texture de fond.\n");
        return;
    }

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
                    hover_link = (event.motion.x >= 500 && event.motion.x <= 640 &&
                                  event.motion.y >= WINDOW_HEIGHT - 40 && event.motion.y <= WINDOW_HEIGHT - 20);

                    for (int i = 0; i < 4; i++) {
                        if (event.motion.x >= 50 && event.motion.x <= 250 &&
                            event.motion.y >= 200 + i * 60 && event.motion.y <= 200 + i * 60 + 50) {
                            selected_option = i;
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int x = event.button.x;
                        int y = event.button.y;

                        if (hover_link) {
                            SDL_OpenURL("https://github.com/Pyhrrah/Landtales-2");
                        }

                        for (int i = 0; i < 4; i++) {
                            int btn_x = 50;
                            int btn_y = 200 + i * 60;
                            int btn_width = 200;
                            int btn_height = 50;

                            if (x >= btn_x && x <= btn_x + btn_width && y >= btn_y && y <= btn_y + btn_height) {
                                switch (i) {
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
                                        running = 0;
                                        break;
                                }
                            }
                        }
                    }
                    break;
                case SDL_KEYDOWN:
                    konami_input[konami_index] = event.key.keysym.sym;
                    konami_index = (konami_index + 1) % KONAMI_LENGTH;

                    if (check_konami_code() && konami_did == 0) {
                        if (!music_played) {
                            play_audio("./assets/music/audio_video.mp3"); 
                            music_played = 1;
                        }
                        play_video("./assets/video/video.mp4", renderer);
                        check_and_free_music();
                        cleanup_audio();
                        konami_did = 1;
                        running = 0;
                    }

                    if (event.key.keysym.sym == SDLK_UP) {
                        selected_option = (selected_option - 1 + 4) % 4;
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        selected_option = (selected_option + 1) % 4;
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
                                running = 0; 
                                break;
                        }
                    }
                    break;
            }
        }

        render_menu(renderer, selected_option, background, font, hover_link);
        SDL_Delay(100);
    }

    SDL_DestroyTexture(background);
    TTF_CloseFont(font);
}

