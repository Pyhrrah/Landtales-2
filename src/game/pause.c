#include "./../../include/game/pause.h"
#include "./../../include/utils/sdl_utils.h"

// Fonction pour dessiner le menu de pause
void drawPauseMenu(SDL_Renderer *renderer, int map[11][11], int salle) {
    int bossRoom = 2; // Toujours la salle 2 pour le boss

    // Charger l'image de fond avec render_texture
    SDL_Texture *backgroundTexture = load_texture(renderer, "./assets/images/fondPause.png");
    if (!backgroundTexture) {
        printf("Erreur lors du chargement de l'image de fond\n");
        return;
    }

    render_texture(renderer, backgroundTexture, 0, 0, 672, 544);
    SDL_DestroyTexture(backgroundTexture);

    TTF_Font *font = load_font("./assets/fonts/font.ttf", 24);
    if (!font) {
        printf("Erreur lors de l'ouverture de la police: %s\n", TTF_GetError());
        return;
    }

    int buttonWidth = 200;
    int buttonHeight = 60;
    SDL_Rect resumeButton = {640 - buttonWidth - 30, 200, buttonWidth, buttonHeight};
    SDL_Rect quitButton = {640 - buttonWidth - 30, 300, buttonWidth, buttonHeight};

    SDL_Color resumeButtonColor = {0, 255, 0, 255};  
    SDL_Color quitButtonColor = {255, 0, 0, 255};    
    render_button(renderer, &resumeButton, "Reprendre", (SDL_Color){255, 255, 255, 255}, resumeButtonColor, font);
    render_button(renderer, &quitButton, "Quitter", (SDL_Color){255, 255, 255, 255}, quitButtonColor, font);

    if (salle !=0 && salle != 100) {
        int mapSize = 11;
    int tileSize = 30;
    int mapStartX = 50;
    int mapStartY = 150;

    for (int i = 0; i < mapSize; i++) {
        for (int j = 0; j < mapSize; j++) {
            SDL_Rect tileRect = {mapStartX + j * tileSize, mapStartY + i * tileSize, tileSize, tileSize};
            int currentRoom = map[i][j];

            if (currentRoom == -1 || currentRoom == -2) continue; 

            if (currentRoom >= 1 && currentRoom <= 36) {
                SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);  
            } else if (currentRoom == 0) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  
            }
            SDL_RenderFillRect(renderer, &tileRect);

            if (currentRoom == 0) {
                SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);  
                if (j > 0 && map[i][j-1] >= 1 && map[i][j-1] <= 36 && map[i][j-1] != currentRoom) {
                    SDL_RenderDrawLine(renderer, tileRect.x, tileRect.y + tileSize / 2, tileRect.x + tileSize / 4, tileRect.y + tileSize / 2);
                }
                if (j < mapSize - 1 && map[i][j+1] >= 1 && map[i][j+1] <= 36 && map[i][j+1] != currentRoom) {
                    SDL_RenderDrawLine(renderer, tileRect.x + tileSize, tileRect.y + tileSize / 2, tileRect.x + tileSize * 3 / 4, tileRect.y + tileSize / 2);
                }
                if (i > 0 && map[i-1][j] >= 1 && map[i-1][j] <= 36 && map[i-1][j] != currentRoom) {
                    SDL_RenderDrawLine(renderer, tileRect.x + tileSize / 2, tileRect.y, tileRect.x + tileSize / 2, tileRect.y + tileSize / 4);
                }
                if (i < mapSize - 1 && map[i+1][j] >= 1 && map[i+1][j] <= 36 && map[i+1][j] != currentRoom) {
                    SDL_RenderDrawLine(renderer, tileRect.x + tileSize / 2, tileRect.y + tileSize, tileRect.x + tileSize / 2, tileRect.y + tileSize * 3 / 4);
                }
            }

            if (salle == currentRoom && salle != bossRoom && salle != 0 && salle != 100) {
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