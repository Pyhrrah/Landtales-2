#ifndef PAUSE_H
#define PAUSE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void drawPauseMenu(SDL_Renderer *renderer);
bool handlePauseMenu(SDL_Event *event, bool *gamePaused);
bool handleButtonClick(SDL_Event *event, SDL_Rect resumeButton, SDL_Rect quitButton, bool *gamePaused, bool *running);

#endif
