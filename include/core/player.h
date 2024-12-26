#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>
#include "ennemies.h"

typedef struct {
    SDL_Rect rect;              
    int vie;                    
    int attaque;               
    int defense;                
    char orientation;           
    int id;                     
    bool regen_dispo;           
    time_t last_regen;          
    int argent;                 
} Player;

typedef struct {
    SDL_Rect rect;
} Attack;

void initPlayer(Player *player, int x, int y, int w, int h, int vie);

void attackWithSword(SDL_Renderer *renderer, Player *player, char orientation, Enemy enemies[], int *enemyCount, const char *enemyFilename);

void regeneratePlayer(Player *player);

void useLightning(SDL_Renderer *renderer, Player *player);

#endif
