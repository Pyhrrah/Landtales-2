#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "menu.h"

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de la SDL : %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *screen = SDL_SetVideoMode(1280, 960, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) {
        fprintf(stderr, "Impossible de passer en mode vidéo : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_WM_SetCaption("Roguelike RPG - Menu Principal", NULL);

    handle_menu(screen);

    SDL_Quit();
    return 0;
}
