#include <SDL2/SDL.h>
#include <stdio.h>
#include "./../../include/multiplayer.h"

void start_multiplayer_mode(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    printf("Mode multijoueur lancé\n");
    SDL_Delay(2000);  
}
