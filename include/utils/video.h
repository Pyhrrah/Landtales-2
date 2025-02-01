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

// Déclaration de la musique
extern Mix_Music *currentMusic;

/**
 * Initialisation de l'audio
 */
int init_audio();

/**
 * Libération de l'audio
 */
void cleanup_audio();

/**
 * Lecture d'un fichier audio
 */
void play_audio(const char *filename);

/**
 * Affichage des frames de la vidéo
 * @param ctx : le contexte du codec
 * @param pkt : le packet
 * @param frame : la frame
 * @param rect : le rectangle SDL
 * @param texture : la texture SDL
 * @param renderer : le renderer SDL
 * @param fpsrendering : le nombre de frames par seconde
 */
void display_video_frame(AVCodecContext *ctx, AVPacket *pkt, AVFrame *frame, SDL_Rect *rect,
                         SDL_Texture *texture, SDL_Renderer *renderer, double fpsrendering);

/**
 * Lecture d'une vidéo
 * @param filename : le nom du fichier vidéo
 * @param renderer : le renderer SDL
 */
void play_video(const char *filename, SDL_Renderer *renderer);

/**
 * Libération de la musique
 */
void free_music();

/**
 * Vérification et libération de la musique
 */
void check_and_free_music();

/**
 * Lecture d'un son
 * @param sound : le son à jouer
 */
void playSong(char *sound);

#endif // VIDEO_H