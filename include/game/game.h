#ifndef GAME_MODE_H
#define GAME_MODE_H

#include <SDL2/SDL.h>

/**
 * Démarre le mode de jeu.
 * 
 * Cette fonction initialise et lance le mode de jeu en utilisant un renderer SDL pour l'affichage.
 * Elle gère les événements et la logique liée au mode de jeu.
 * 
 * @param renderer : Le renderer SDL utilisé pour dessiner le jeu.
 */
void start_game_mode(SDL_Renderer *renderer);

#endif // GAME_MODE_H
