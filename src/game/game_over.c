#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "./../../include/core/player.h"
#include "./../../include/game/file.h"
#include "./../../include/game/map.h"

// Fonction pour afficher l'écran Game Over
void displayGameOverScreen(SDL_Renderer *renderer, SDL_Event *event, int *running) {
    const int screenWidth = 672;
    const int screenHeight = 544;

    SDL_Surface *backgroundSurface = IMG_Load("./assets/images/fondGameOver.png");
    if (!backgroundSurface) {
        printf("Erreur de chargement de l'image de fond: %s\n", SDL_GetError());
        return;
    }

    SDL_Texture *backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);  

    if (!backgroundTexture) {
        printf("Erreur de création de la texture de fond: %s\n", SDL_GetError());
        return;
    }

    SDL_Rect backgroundRect = {0, 0, screenWidth, screenHeight};
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);  

    SDL_Rect yesButton = {screenWidth / 2 - 150, screenHeight / 2 + 100, 100, 50};
    SDL_Rect noButton = {screenWidth / 2 + 50, screenHeight / 2 + 100, 100, 50};

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    SDL_RenderFillRect(renderer, &yesButton);
    SDL_RenderFillRect(renderer, &noButton);

    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 28);
    if (!font) {
        printf("Erreur de chargement de la police: %s\n", TTF_GetError());
        return;
    }

    SDL_Color textColor = {255, 255, 255, 255}; 

    const char *yesText = "Oui";
    SDL_Surface *yesSurface = TTF_RenderUTF8_Solid(font, yesText, textColor);
    SDL_Texture *yesTexture = SDL_CreateTextureFromSurface(renderer, yesSurface);
    SDL_Rect yesTextRect = {yesButton.x + (yesButton.w - yesSurface->w) / 2, yesButton.y + (yesButton.h - yesSurface->h) / 2, yesSurface->w, yesSurface->h};
    SDL_RenderCopy(renderer, yesTexture, NULL, &yesTextRect);

    const char *noText = "Non";
    SDL_Surface *noSurface = TTF_RenderUTF8_Solid(font, noText, textColor);
    SDL_Texture *noTexture = SDL_CreateTextureFromSurface(renderer, noSurface);
    SDL_Rect noTextRect = {noButton.x + (noButton.w - noSurface->w) / 2, noButton.y + (noButton.h - noSurface->h) / 2, noSurface->w, noSurface->h};
    SDL_RenderCopy(renderer, noTexture, NULL, &noTextRect);

    SDL_FreeSurface(yesSurface);
    SDL_DestroyTexture(yesTexture);
    SDL_FreeSurface(noSurface);
    SDL_DestroyTexture(noTexture);

    SDL_RenderPresent(renderer);

    SDL_bool buttonClicked = SDL_FALSE;
    while (!buttonClicked) {
        while (SDL_PollEvent(event)) {
            if (event->type == SDL_QUIT) {
                *running = SDL_FALSE;
                buttonClicked = SDL_TRUE;
            }
            if (event->type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (x >= yesButton.x && x <= yesButton.x + yesButton.w && y >= yesButton.y && y <= yesButton.y + yesButton.h) {
                    buttonClicked = SDL_TRUE;
                }

                if (x >= noButton.x && x <= noButton.x + noButton.w && y >= noButton.y && y <= noButton.y + noButton.h) {
                    *running = SDL_FALSE;
                    buttonClicked = SDL_TRUE;
                }
            }
        }
    }

    SDL_DestroyTexture(backgroundTexture);

    TTF_CloseFont(font);
}

// Fonction game over mise à jour
void gameOver(Player *player, SDL_Renderer *renderer, int saveNumber, int *room, int *etage, int mapRoom[15][21], int *tentative, int mapData[11][11], char *stageFilename, SDL_Event *event, int *running) {
    if (player->vie <= 0) {
        displayGameOverScreen(renderer, event, running);

        player->vie = player->max_vie;

        clearMapDirectory(saveNumber);
        creerEtageEntier(*etage);
        (*tentative) += 1;

        *room = 0;
        *etage = 1;

        saveGame(saveNumber, *tentative, player->vie, player->attaque, player->defense, *etage, player->argent, *room, player->max_vie);
        clearEnemies();

        player->rect.x = 320;
        player->rect.y = 224;

        loadMap(stageFilename, mapData);

        char roomFilename[100] = "./data/game/lobbyMap.txt";
        loadRoomData(roomFilename, mapRoom);
    }
}
