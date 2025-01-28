#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

// Initialisation de SDL, TTF et création de la fenêtre et du renderer
int initSDL(SDL_Window **window, SDL_Renderer **renderer, const char *title, int width, int height);

// Fermeture propre de SDL, TTF, de la fenêtre et du renderer
void closeSDL(SDL_Window *window, SDL_Renderer *renderer);

// Fonction pour dessiner du texte à l'écran
void render_text(SDL_Renderer *renderer, const char *text, int x, int y, TTF_Font *font, SDL_Color color);

// Fonction pour charger une texture
SDL_Texture* load_texture(SDL_Renderer *renderer, const char *file_path);

// Fonction pour dessiner une texture à l'écran
void render_texture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int width, int height);

// Fonction pour charger une police
TTF_Font* load_font(const char *file_path, int font_size);

// Fonction pour dessiner un bouton
void render_button(SDL_Renderer *renderer, SDL_Rect *button, const char *text, SDL_Color textColor, SDL_Color backgroundColor, TTF_Font *font);



#endif // SDL_UTILS_H