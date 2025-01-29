#ifndef VIDEO_H
#define VIDEO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

// Initialisation de l'audio
int init_audio();

// Nettoyage des ressources audio
void cleanup_audio(Mix_Music *music);

// Lecture d'un fichier audio
Mix_Music *play_audio(const char *filename);

// Lecture et affichage d'une image vidéo
void display_video_frame(AVCodecContext *ctx, AVPacket *pkt, AVFrame *frame, SDL_Rect *rect,
                         SDL_Texture *texture, SDL_Renderer *renderer, double fpsrendering);

// Lecture d'un fichier vidéo
void play_video(const char *filename, SDL_Renderer *renderer);

#endif // VIDEO_H