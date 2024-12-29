#ifndef HUD_H
#define HUD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./../../include/core/player.h"

/**
 * Affiche le HUD (Head-Up Display) du jeu.
 * 
 * Cette fonction est responsable de l'affichage de l'interface utilisateur, 
 * incluant des informations sur le joueur (comme la santé ou les ressources), 
 * ainsi que le nombre de tentatives effectuées. Le tout est rendu via le renderer SDL.
 * 
 * @param renderer : Le renderer SDL utilisé pour dessiner le HUD.
 * @param player : Pointeur vers la structure du joueur contenant ses données actuelles.
 * @param tentative : Le nombre de tentatives effectuées par le joueur.
 */
void renderHUD(SDL_Renderer *renderer, Player *player, int tentative);

#endif // HUD_H
