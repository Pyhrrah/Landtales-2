#include "SDL/SDL.h"
#include <stdio.h>
#include "./../../include/editor.h"

void start_editor_mode(SDL_Surface *screen) {
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 0, 0));
    SDL_Flip(screen);
    printf("Mode éditeur lancé\n");
    SDL_Delay(2000);  
}
