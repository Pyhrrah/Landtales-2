#include "SDL/SDL.h"
#include <stdio.h>
#include "./../../include/multiplayer.h"

void start_multiplayer_mode(SDL_Surface *screen) {
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 255));
    SDL_Flip(screen);
    printf("Mode multijoueur lancé\n");
    SDL_Delay(2000);  
}
