#include <SDL2/SDL.h>
#include <stdio.h>
#include "./../../include/settings.h"

void open_settings(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    printf("Paramètres ouverts\n");
    SDL_Delay(2000); 
}
