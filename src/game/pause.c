#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "./../../include/game/pause.h"

// Fonction pour dessiner le menu de pause
void drawPauseMenu(SDL_Renderer *renderer, int map[11][11], int salle) {
    int bossRoom = 2; // Toujours la salle 2 pour le boss

    // Charger l'image de fond
    SDL_Surface *backgroundSurface = IMG_Load("./assets/images/fondPause.png");
    if (!backgroundSurface) {
        printf("Erreur lors du chargement de l'image de fond: %s\n", IMG_GetError());
        return;
    }
    SDL_Texture *backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);
    if (!backgroundTexture) {
        printf("Erreur lors de la création de la texture de fond: %s\n", SDL_GetError());
        return;
    }

    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    SDL_DestroyTexture(backgroundTexture);

    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 24);
    if (!font) {
        printf("Erreur lors de l'ouverture de la police: %s\n", TTF_GetError());
        return;
    }

    int buttonWidth = 200;
    int buttonHeight = 60;
    SDL_Rect resumeButton = {640 - buttonWidth - 30, 200, buttonWidth, buttonHeight};
    SDL_Rect quitButton = {640 - buttonWidth - 30, 300, buttonWidth, buttonHeight};

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Vert pour "Reprendre"
    SDL_RenderFillRect(renderer, &resumeButton);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge pour "Quitter"
    SDL_RenderFillRect(renderer, &quitButton);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanchir les contours
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

    // Code pour dessiner la minimap
    int mapSize = 11;  // La carte est 11x11
    int tileSize = 30; // Taille des cases de la map
    int mapStartX = 50; // Position X de départ pour la mini-map
    int mapStartY = 150; // Position Y de départ pour la mini-map

    for (int i = 0; i < mapSize; i++) {
        for (int j = 0; j < mapSize; j++) {
            SDL_Rect tileRect = {mapStartX + j * tileSize, mapStartY + i * tileSize, tileSize, tileSize};
            int currentRoom = map[i][j];

            if (currentRoom >= 1 && currentRoom <= 36) { 
                SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); 
            } else if (currentRoom == 0) { 
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); 
            } else if (currentRoom == -1) { 
                continue; 
            } else if (currentRoom == -2) { 
                continue; 
            }

            SDL_RenderFillRect(renderer, &tileRect); 

            if (currentRoom == 0) { 
                if (j > 0 && map[i][j-1] >= 1 && map[i][j-1] <= 36 && map[i][j-1] != currentRoom) { 
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); 
                    SDL_RenderDrawLine(renderer, tileRect.x, tileRect.y + tileSize / 2, tileRect.x + tileSize / 4, tileRect.y + tileSize / 2);
                }
                if (j < mapSize - 1 && map[i][j+1] >= 1 && map[i][j+1] <= 36 && map[i][j+1] != currentRoom) { 
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); 
                    SDL_RenderDrawLine(renderer, tileRect.x + tileSize, tileRect.y + tileSize / 2, tileRect.x + tileSize * 3 / 4, tileRect.y + tileSize / 2);
                }
                if (i > 0 && map[i-1][j] >= 1 && map[i-1][j] <= 36 && map[i-1][j] != currentRoom) { 
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); 
                    SDL_RenderDrawLine(renderer, tileRect.x + tileSize / 2, tileRect.y, tileRect.x + tileSize / 2, tileRect.y + tileSize / 4);
                }
                if (i < mapSize - 1 && map[i+1][j] >= 1 && map[i+1][j] <= 36 && map[i+1][j] != currentRoom) {
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); 
                    SDL_RenderDrawLine(renderer, tileRect.x + tileSize / 2, tileRect.y + tileSize, tileRect.x + tileSize / 2, tileRect.y + tileSize * 3 / 4);
                }
            }

            if (salle == currentRoom) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
                SDL_Rect playerRect = {tileRect.x + tileSize / 2 - 5, tileRect.y + tileSize / 2 - 5, 10, 10}; 
                SDL_RenderFillRect(renderer, &playerRect); 
            }

            if (currentRoom == bossRoom) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
                SDL_Rect bossRect = {tileRect.x + tileSize / 2 - 5, tileRect.y + tileSize / 2 - 5, 10, 10}; 
                SDL_RenderFillRect(renderer, &bossRect); 
            }
        }
    }

    SDL_RenderPresent(renderer);
}


// Fonction pour gérer l'événement de pause
int handlePauseMenu(SDL_Event *event, int *gamePaused) {
    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
        *gamePaused = 1;
        return 1;
    }
    return 0;
}

// Fonction pour gérer le clic sur les boutons du menu de pause
int handleButtonClick(SDL_Event *event, SDL_Rect resumeButton, SDL_Rect quitButton, int *gamePaused, int *running) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int x = event->button.x;
        int y = event->button.y;

        if (x >= resumeButton.x && x <= resumeButton.x + resumeButton.w &&
            y >= resumeButton.y && y <= resumeButton.y + resumeButton.h) {
            *gamePaused = 0;
            return 1;
        }

        if (x >= quitButton.x && x <= quitButton.x + quitButton.w &&
            y >= quitButton.y && y <= quitButton.y + quitButton.h) {
            *running = 0;
            *gamePaused = 0;
            return 1;
        }
    }
    return 0;
}