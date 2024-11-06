#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void handle_menu(SDL_Surface *screen);

void render_menu(SDL_Surface *screen, int selected_option);

void render_text(SDL_Surface *screen, const char *text, int x, int y, TTF_Font *font, SDL_Color color);

#endif
