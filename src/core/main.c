#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "./../../include/core/menu.h"
#include "./../../include/utils/sdl_utils.h"


#define WINDOW_WIDTH 672
#define WINDOW_HEIGHT 544


int main(int argc, char * argv[]) {
    char * throw;
    sscanf(argv[argc-1], "%ms", &throw);

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!initSDL(&window, &renderer, "Landtales 2", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return 1; 
    }

    handle_menu(renderer);

    closeSDL(window, renderer);

    return 0;
}
