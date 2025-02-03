#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h> 
#include "./../../include/core/player.h"
#include <stdio.h>

/*

Fichier hud.c, ce dernier contient les fonctions pour afficher le HUD du jeu (vie, pièces, etc.)

*/

// Fonction pour afficher le HUD avec les images de foudre et régénération
void renderHUD(SDL_Renderer *renderer, Player *player, int tentative) {

    const int screenWidth = 672;
    const int screenHeight = 544;
    const int hudHeight = 544 - (32 * 15);

    // Fond du HUD
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect hudRect = {0, screenHeight - hudHeight, screenWidth, hudHeight};
    SDL_RenderFillRect(renderer, &hudRect);

    // Charger l'image du joueur à partir du fichier
    char skin_path[512];
    load_skin_from_file("assets/skin_config.txt", skin_path);

    // Affichage de l'image du joueur
    SDL_Texture *leftImageTexture = IMG_LoadTexture(renderer, skin_path);
    if (!leftImageTexture) {
        printf("Erreur lors du chargement de l'image : %s\n", IMG_GetError());
    } else {
        int leftImageY = screenHeight - hudHeight + 10;
        
        SDL_Rect srcRect = {0, 0, 32, 32};  
        SDL_Rect destRect = {25, leftImageY, 48, 48};  

        SDL_RenderCopy(renderer, leftImageTexture, &srcRect, &destRect);

        SDL_DestroyTexture(leftImageTexture);
    }

    // Couleur du texte
    SDL_Color textColor = {255, 255, 255, 255};

    // Charger la police
    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 16);
    if (!font) {
        printf("Erreur de chargement de la police: %s\n", TTF_GetError());
        return;
    }

    // Décalage de 80 vers la droite
    int xOffset = 80;

    float healthPercentage = (float)player->vie / player->max_vie;
    int barWidth = 200;
    int barHeight = 10;
    SDL_Rect healthBar = {25 + xOffset, screenHeight - hudHeight + 10, barWidth, barHeight};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    SDL_RenderFillRect(renderer, &healthBar);

    // Barre de vie
    SDL_Rect healthFill = {healthBar.x, healthBar.y, (int)(barWidth * healthPercentage), barHeight};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &healthFill);

    // Texte de la vie
    char healthText[50];
    sprintf(healthText, "Vie: %d / %d", player->vie, player->max_vie);
    SDL_Surface *healthSurface = TTF_RenderUTF8_Solid(font, healthText, textColor);
    SDL_Texture *healthTexture = SDL_CreateTextureFromSurface(renderer, healthSurface);
    SDL_Rect healthTextRect = {50 + xOffset, screenHeight - hudHeight + 25, healthSurface->w, healthSurface->h};
    SDL_RenderCopy(renderer, healthTexture, NULL, &healthTextRect);
    SDL_FreeSurface(healthSurface);
    SDL_DestroyTexture(healthTexture);

    // Texte des pièces
    char coinText[50];
    sprintf(coinText, "Pièces: %d", player->argent);
    SDL_Surface *coinSurface = TTF_RenderUTF8_Solid(font, coinText, textColor);
    SDL_Texture *coinTexture = SDL_CreateTextureFromSurface(renderer, coinSurface);
    SDL_Rect coinTextRect = {50 + barWidth + 20 + xOffset, screenHeight - hudHeight + 10, coinSurface->w, coinSurface->h};
    SDL_RenderCopy(renderer, coinTexture, NULL, &coinTextRect);
    SDL_FreeSurface(coinSurface);
    SDL_DestroyTexture(coinTexture);

    // Texte des tentatives
    char attemptText[50];
    sprintf(attemptText, "Tentatives: %d", tentative);
    SDL_Surface *attemptSurface = TTF_RenderUTF8_Solid(font, attemptText, textColor);
    SDL_Texture *attemptTexture = SDL_CreateTextureFromSurface(renderer, attemptSurface);
    SDL_Rect attemptTextRect = {50 + barWidth + 20 + xOffset, screenHeight - hudHeight + 35, attemptSurface->w, attemptSurface->h};
    SDL_RenderCopy(renderer, attemptTexture, NULL, &attemptTextRect);
    SDL_FreeSurface(attemptSurface);
    SDL_DestroyTexture(attemptTexture);

    // Affichage des images de foudre et régénération
    SDL_Texture *lightningTexture = NULL;
    if (getLightningCooldown(player) > 0) {
        lightningTexture = IMG_LoadTexture(renderer, "./assets/images/sprite/spell/foudre2.png"); 
    } else {
        lightningTexture = IMG_LoadTexture(renderer, "./assets/images/sprite/spell/foudre1.png");
    }

    // Affichage de l'image de la foudre
    if (lightningTexture) {
        SDL_Rect lightningRect = {screenWidth - 80, screenHeight - hudHeight + 10, 30, 30};
        SDL_RenderCopy(renderer, lightningTexture, NULL, &lightningRect);
        
        // Affichage du texte du cooldown
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

    // Affichage de l'image de régénération
    SDL_Texture *regenTexture = NULL;
    if (getRegenCooldown(player) > 0) {
        regenTexture = IMG_LoadTexture(renderer, "./assets/images/sprite/spell/regen2.png");
    } else {
        regenTexture = IMG_LoadTexture(renderer, "./assets/images/sprite/spell/regen1.png");
    }

    // Affichage de l'image de régénération
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

    // Nettoyage de la police
    TTF_CloseFont(font);
}
