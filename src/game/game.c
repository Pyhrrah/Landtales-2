#include "SDL/SDL.h"
#include <stdio.h>
#include "./../../include/game.h"

void start_game_mode(SDL_Surface *screen) {
    int running = 1;
    SDL_Event event;

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 255, 0));
    SDL_Flip(screen);
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
    
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0)); 
    SDL_Flip(screen);
    printf("Retour au menu...\n");
}
