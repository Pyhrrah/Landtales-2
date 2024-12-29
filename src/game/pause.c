#include <SDL2/SDL_ttf.h>
#include "./../../include/game/pause.h"

// Fonction pour dessiner le menu de pause
void drawPauseMenu(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 24);
    if (!font) {
        printf("Erreur lors de l'ouverture de la police: %s\n", TTF_GetError());
        return;
    }

    int buttonWidth = 200;
    int buttonHeight = 60;
    SDL_Rect resumeButton = {640 - buttonWidth - 30, 200, buttonWidth, buttonHeight};
    SDL_Rect quitButton = {640 - buttonWidth - 30, 300, buttonWidth, buttonHeight};

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &resumeButton);  

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &quitButton);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &resumeButton);
    SDL_RenderDrawRect(renderer, &quitButton);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, "Reprendre", textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    int textWidth = 0, textHeight = 0;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
    SDL_Rect textRectResume = {resumeButton.x + (resumeButton.w - textWidth) / 2,
                               resumeButton.y + (resumeButton.h - textHeight) / 2,
                               textWidth, textHeight};

    SDL_RenderCopy(renderer, textTexture, NULL, &textRectResume);
    SDL_DestroyTexture(textTexture);

    textSurface = TTF_RenderUTF8_Solid(font, "Quitter", textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
    SDL_Rect textRectQuit = {quitButton.x + (quitButton.w - textWidth) / 2,
                             quitButton.y + (quitButton.h - textHeight) / 2,
                             textWidth, textHeight};

    SDL_RenderCopy(renderer, textTexture, NULL, &textRectQuit);
    SDL_DestroyTexture(textTexture);

    SDL_RenderPresent(renderer);
}

// Fonction pour gérer l'événement de pause
bool handlePauseMenu(SDL_Event *event, bool *gamePaused) {
    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
        *gamePaused = true;
        return true;
    }
    return false;
}

// Fonction pour gérer le clic sur les boutons du menu de pause
bool handleButtonClick(SDL_Event *event, SDL_Rect resumeButton, SDL_Rect quitButton, bool *gamePaused, bool *running) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int x = event->button.x;
        int y = event->button.y;

        if (x >= resumeButton.x && x <= resumeButton.x + resumeButton.w &&
            y >= resumeButton.y && y <= resumeButton.y + resumeButton.h) {
            *gamePaused = false;
            return true;
        }

        if (x >= quitButton.x && x <= quitButton.x + quitButton.w &&
            y >= quitButton.y && y <= quitButton.y + quitButton.h) {
            *running = false;
            *gamePaused = false;
            return true;
        }
    }
    return false;
}