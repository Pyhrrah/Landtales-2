#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void handle_menu(SDL_Renderer *renderer);

void render_menu(SDL_Renderer *renderer, int selected_option);

void render_text(SDL_Renderer *renderer, const char *text, int x, int y, TTF_Font *font, SDL_Color color);

#endif
