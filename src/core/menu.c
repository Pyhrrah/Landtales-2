#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_audio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./../../include/editor/editor.h"
#include "./../../include/multiplayer/multiplayer.h"
#include "./../../include/settings.h"
#include "./../../include/game/game.h"
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

int init_audio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Erreur: SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    if (Mix_Init(MIX_INIT_MP3) == 0) {
        fprintf(stderr, "Erreur: Impossible d'initialiser SDL_mixer: %s\n", Mix_GetError());
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Erreur: Mix_OpenAudio failed: %s\n", Mix_GetError());
        return -1;
    }

    return 0;
}

void cleanup_audio() {
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

void play_audio(const char *filename) {
    Mix_Music *music = Mix_LoadMUS(filename);
    if (!music) {
        fprintf(stderr, "Erreur: Impossible de charger la musique: %s\n", Mix_GetError());
        return;
    }

    if (Mix_PlayMusic(music, -1) == -1) {
        fprintf(stderr, "Erreur: Impossible de jouer la musique: %s\n", Mix_GetError());
        Mix_FreeMusic(music);
        return;
    }
}


void display_video_frame(AVCodecContext *ctx, AVPacket *pkt, AVFrame *frame, SDL_Rect *rect,
                         SDL_Texture *texture, SDL_Renderer *renderer, double fpsrendering) {
    if (avcodec_send_packet(ctx, pkt) < 0) return;
    if (avcodec_receive_frame(ctx, frame) < 0) return;

    SDL_UpdateYUVTexture(texture, rect,
                         frame->data[0], frame->linesize[0],
                         frame->data[1], frame->linesize[1],
                         frame->data[2], frame->linesize[2]);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_RenderPresent(renderer);
    SDL_Delay((Uint32)(fpsrendering * 1000));
}

void play_video(const char *filename, SDL_Renderer *renderer) {

    AVFormatContext *format_ctx = NULL;
    if (avformat_open_input(&format_ctx, filename, NULL, NULL) != 0) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le fichier %s\n", filename);
        return;
    }

    if (avformat_find_stream_info(format_ctx, NULL) < 0) {
        fprintf(stderr, "Erreur: impossible de récupérer les infos du flux\n");
        avformat_close_input(&format_ctx);
        return;
    }

    int video_stream_index = -1;
    for (unsigned int i = 0; i < (unsigned int)format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        fprintf(stderr, "Erreur: aucun flux vidéo trouvé\n");
        avformat_close_input(&format_ctx);
        return;
    }

    AVCodecParameters *codec_params = format_ctx->streams[video_stream_index]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codec_params->codec_id);
    if (!codec) {
        fprintf(stderr, "Erreur: codec non trouvé\n");
        avformat_close_input(&format_ctx);
        return;
    }

    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codec_params);

    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le codec\n");
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return;
    }

    
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
                                             codec_params->width, codec_params->height);

    SDL_Rect rect = {0, 0, codec_params->width, codec_params->height};
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    int fps = format_ctx->streams[video_stream_index]->avg_frame_rate.num /
              format_ctx->streams[video_stream_index]->avg_frame_rate.den;
    double fpsrendering = 1.0 / fps;

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        if (av_read_frame(format_ctx, packet) >= 0) {
            if (packet->stream_index == video_stream_index) {
                display_video_frame(codec_ctx, packet, frame, &rect, texture, renderer, fpsrendering);
            }
            av_packet_unref(packet);
        } else {
            break;
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
}

// Fonction pour afficher du texte à l'écran
void render_text(SDL_Renderer *renderer, const char *text, int x, int y, TTF_Font *font, SDL_Color color) {
    SDL_Surface *text_surface = TTF_RenderUTF8_Solid(font, text, color);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    if (text_texture) {
        SDL_Rect text_location = {x, y, text_surface->w, text_surface->h};
        SDL_RenderCopy(renderer, text_texture, NULL, &text_location);
        SDL_DestroyTexture(text_texture);
    }
    SDL_FreeSurface(text_surface);
}

void render_menu(SDL_Renderer *renderer, int selected_option, SDL_Texture *background, TTF_Font *font) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL); 

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

    SDL_Surface *bg_surface = IMG_Load("./assets/images/fond.png");
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

    TTF_Font *font = TTF_OpenFont("./assets/fonts/font.ttf", 28);
    if (!font) {
        fprintf(stderr, "Impossible de charger la police : %s\n", TTF_GetError());
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
                        if (!music_played) {
                            play_audio("./assets/music/audio_video.mp3"); 
                            music_played = 1;
                        }
                        play_video("./assets/video/video.mp4", renderer);
                        cleanup_audio();
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

    
    SDL_DestroyTexture(background);
    TTF_CloseFont(font);
}

