#include <SDL/SDL.h>
#include <stdio.h>
#include "./../../include/settings.h"

void open_settings(SDL_Surface *screen) {
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 0)); 
    SDL_Flip(screen);
    printf("Paramètres ouverts\n");
    SDL_Delay(2000); 
}
