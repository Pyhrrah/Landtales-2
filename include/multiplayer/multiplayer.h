#ifndef MULTIPLAYER_MODE_H
#define MULTIPLAYER_MODE_H

#include <SDL2/SDL.h>

/**
 * Démarre le mode multijoueur du jeu.
 * 
 * Cette fonction initialise et lance le mode multijoueur, en établissant la communication
 * nécessaire entre les clients ou avec le serveur, tout en utilisant un renderer SDL pour 
 * l'affichage du jeu.
 * 
 * @param renderer : Le renderer SDL utilisé pour dessiner le jeu en mode multijoueur.
 */
void start_multiplayer_mode(SDL_Renderer *renderer);

#endif // MULTIPLAYER_MODE_H
