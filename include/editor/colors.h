#ifndef COLORS_H
#define COLORS_H

#include <SDL2/SDL.h>
extern SDL_Texture *textures[14];
/**
 * Tableau global contenant les couleurs disponibles.
 * Chaque couleur est représentée par une structure SDL_Color (r, g, b, a).
 */
extern SDL_Color colors[];

/**
 * Renvoie le nombre total de couleurs disponibles dans le tableau `colors`.
 * @return Le nombre de couleurs (de type size_t).
 */
size_t get_colors_count();

void load_textures(SDL_Renderer *renderer);

#endif // COLORS_H
