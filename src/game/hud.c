#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h> 
#include "./../../include/core/player.h"
#include <stdio.h>

// Fonction pour afficher le HUD avec les images de foudre et régénération
void renderHUD(SDL_Renderer *renderer, Player *player, int tentative) {

    const int screenWidth = 672;
    const int screenHeight = 544;
    const int hudHeight = 544 - (32 * 15);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect hudRect = {0, screenHeight - hudHeight, screenWidth, hudHeight};
    SDL_RenderFillRect(renderer, &hudRect);

    SDL_Color textColor = {255, 255, 255, 255};

    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 16);
    if (!font) {
        printf("Erreur de chargement de la police: %s\n", TTF_GetError());
        return;
    }

    float healthPercentage = (float)player->vie / player->max_vie;
    int barWidth = 200;
    int barHeight = 10;
    SDL_Rect healthBar = {25, screenHeight - hudHeight + 10, barWidth, barHeight};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    SDL_RenderFillRect(renderer, &healthBar);

    SDL_Rect healthFill = {healthBar.x, healthBar.y, (int)(barWidth * healthPercentage), barHeight};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &healthFill);

    char healthText[50];
    sprintf(healthText, "Vie: %d / %d", player->vie, player->max_vie);
    SDL_Surface *healthSurface = TTF_RenderUTF8_Solid(font, healthText, textColor);
    SDL_Texture *healthTexture = SDL_CreateTextureFromSurface(renderer, healthSurface);
    SDL_Rect healthTextRect = {50, screenHeight - hudHeight + 25, healthSurface->w, healthSurface->h};
    SDL_RenderCopy(renderer, healthTexture, NULL, &healthTextRect);
    SDL_FreeSurface(healthSurface);
    SDL_DestroyTexture(healthTexture);

    char coinText[50];
    sprintf(coinText, "Pièces: %d", player->argent);
    SDL_Surface *coinSurface = TTF_RenderUTF8_Solid(font, coinText, textColor);
    SDL_Texture *coinTexture = SDL_CreateTextureFromSurface(renderer, coinSurface);
    SDL_Rect coinTextRect = {50 + barWidth + 20, screenHeight - hudHeight + 10, coinSurface->w, coinSurface->h};
    SDL_RenderCopy(renderer, coinTexture, NULL, &coinTextRect);
    SDL_FreeSurface(coinSurface);
    SDL_DestroyTexture(coinTexture);

    char attemptText[50];
    sprintf(attemptText, "Tentatives: %d", tentative);
    SDL_Surface *attemptSurface = TTF_RenderUTF8_Solid(font, attemptText, textColor);
    SDL_Texture *attemptTexture = SDL_CreateTextureFromSurface(renderer, attemptSurface);
    SDL_Rect attemptTextRect = {50 + barWidth + 20, screenHeight - hudHeight + 35, attemptSurface->w, attemptSurface->h};
    SDL_RenderCopy(renderer, attemptTexture, NULL, &attemptTextRect);
    SDL_FreeSurface(attemptSurface);
    SDL_DestroyTexture(attemptTexture);

    SDL_Texture *lightningTexture = NULL;
    if (getLightningCooldown(player) > 0) {
        lightningTexture = IMG_LoadTexture(renderer, "./assets/images/regen_pas_dispo.png"); 
    } else {
        lightningTexture = IMG_LoadTexture(renderer, "./assets/images/regen_dispo.png");
    }

    if (lightningTexture) {
        SDL_Rect lightningRect = {screenWidth - 80, screenHeight - hudHeight + 10, 30, 30};
        SDL_RenderCopy(renderer, lightningTexture, NULL, &lightningRect);
        
        if (getLightningCooldown(player) > 0) {
            char lightningText[12];
            sprintf(lightningText, "%d", getLightningCooldown(player));
            SDL_Surface *lightningSurface = TTF_RenderUTF8_Solid(font, lightningText, textColor);
            SDL_Texture *lightningTextTexture = SDL_CreateTextureFromSurface(renderer, lightningSurface);
            SDL_Rect lightningTextRect = {screenWidth - 80, screenHeight - hudHeight + 45, lightningSurface->w, lightningSurface->h};
            SDL_RenderCopy(renderer, lightningTextTexture, NULL, &lightningTextRect);
            SDL_FreeSurface(lightningSurface);
            SDL_DestroyTexture(lightningTextTexture);
        }
        SDL_DestroyTexture(lightningTexture); 
    }

    SDL_Texture *regenTexture = NULL;
if (getRegenCooldown(player) > 0) {
    regenTexture = IMG_LoadTexture(renderer, "./assets/images/regen_pas_dispo.png");
} else {
    regenTexture = IMG_LoadTexture(renderer, "./assets/images/regen_dispo.png");
}

if (regenTexture) {
    SDL_Rect regenRect = {screenWidth - 40, screenHeight - hudHeight + 10, 30, 30};
    SDL_RenderCopy(renderer, regenTexture, NULL, &regenRect);
    
    if (getRegenCooldown(player) > 0) {
        char regenText[12];
        sprintf(regenText, "%d", getRegenCooldown(player));
        SDL_Surface *regenSurface = TTF_RenderUTF8_Solid(font, regenText, textColor);
        SDL_Texture *regenTextTexture = SDL_CreateTextureFromSurface(renderer, regenSurface);
        SDL_Rect regenTextRect = {screenWidth - 40, screenHeight - hudHeight + 45, regenSurface->w, regenSurface->h};
        SDL_RenderCopy(renderer, regenTextTexture, NULL, &regenTextRect);
        SDL_FreeSurface(regenSurface);
        SDL_DestroyTexture(regenTextTexture);
    }

    SDL_DestroyTexture(regenTexture);
}

    if (regenTexture) {
        SDL_Rect regenRect = {screenWidth - 40, screenHeight - hudHeight + 10, 30, 30};
        SDL_RenderCopy(renderer, regenTexture, NULL, &regenRect);
        SDL_DestroyTexture(regenTexture);
    }

    TTF_CloseFont(font);
}
