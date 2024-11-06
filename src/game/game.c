#include <SDL2/SDL.h>
#include <stdio.h>
#include "./../../include/game.h"

void start_game_mode(SDL_Renderer *renderer) {
    int running = 1;
    SDL_Event event;

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    printf("Mode histoire lancé\n");

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0; 
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0; 
                }
            }
        }
        SDL_Delay(100); 
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    printf("Retour au menu...\n");
}
