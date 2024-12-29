#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/**
 * Gère les interactions du menu principal.
 * @param renderer : le renderer SDL utilisé pour afficher les éléments du menu.
 */
void handle_menu(SDL_Renderer *renderer);

/**
 * Dessine le menu principal en mettant en surbrillance l'option sélectionnée.
 * @param renderer : le renderer SDL utilisé pour afficher les éléments du menu.
 * @param selected_option : l'index de l'option actuellement sélectionnée.
 */
void render_menu(SDL_Renderer *renderer, int selected_option);

/**
 * Affiche un texte sur l'écran à une position donnée.
 * @param renderer : le renderer SDL utilisé pour dessiner le texte.
 * @param text : le texte à afficher.
 * @param x : position x du texte sur l'écran.
 * @param y : position y du texte sur l'écran.
 * @param font : police utilisée pour rendre le texte.
 * @param color : couleur du texte (structure SDL_Color).
 */
void render_text(SDL_Renderer *renderer, const char *text, int x, int y, TTF_Font *font, SDL_Color color);

#endif // MENU_H
