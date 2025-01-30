#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

/**
 * Initialisation de SDL, TTF, de la fenêtre et du renderer
 * @param window : la fenêtre
 * @param renderer : le renderer
 * @param title : le titre de la fenêtre
 * @param width : la largeur de la fenêtre
 * @param height : la hauteur de la fenêtre
 */
int initSDL(SDL_Window **window, SDL_Renderer **renderer, const char *title, int width, int height);

/**
 * Fermeture de SDL, de la fenêtre et du renderer
 * @param window : la fenêtre
 * @param renderer : le renderer
 */
void closeSDL(SDL_Window *window, SDL_Renderer *renderer);

/**
* Rendre du texte à l'écran
* @param renderer : le renderer
* @param text : le texte à afficher
* @param x : la position en x du texte
* @param y : la position en y du texte
* @param font : la police du texte
* @param color : la couleur du texte
 */
void render_text(SDL_Renderer *renderer, const char *text, int x, int y, TTF_Font *font, SDL_Color color);

/**
 * Charger une texture
 * @param renderer : le renderer
 * @param file_path : le chemin du fichier de la texture
 */
SDL_Texture* load_texture(SDL_Renderer *renderer, const char *file_path);

/**
 * Rendre une texture à l'écran
 * @param renderer : le renderer
 * @param texture : la texture à afficher
 * @param x : la position en x de la texture
 * @param y : la position en y de la texture
 * @param width : la largeur de la texture
 * @param height : la hauteur de la texture
 */
void render_texture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int width, int height);

/**
 * Charger une police
 * @param file_path : le chemin du fichier de la police
 * @param font_size : la taille de la police
 */
TTF_Font* load_font(const char *file_path, int font_size);

/**
 * Rendre un bouton à l'écran
 * @param renderer : le renderer
 * @param button : le rectangle du bouton
 * @param text : le texte du bouton
 * @param textColor : la couleur du texte
 * @param backgroundColor : la couleur de fond
 * @param font : la police du texte
 */
void render_button(SDL_Renderer *renderer, SDL_Rect *button, const char *text, SDL_Color textColor, SDL_Color backgroundColor, TTF_Font *font);



#endif // SDL_UTILS_H