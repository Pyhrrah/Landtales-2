#ifndef EDITOR_MODE_H
#define EDITOR_MODE_H

#include <SDL2/SDL.h>

/**
 * Démarre le mode éditeur.
 * Cette fonction permet à l'utilisateur d'entrer dans un mode spécial
 * pour créer ou modifier des niveaux ou des cartes de jeu.
 * 
 * @param renderer : Le renderer SDL utilisé pour dessiner les éléments
 *                   du mode éditeur à l'écran.
 */
void start_editor_mode(SDL_Renderer *renderer);

#endif // EDITOR_MODE_H
