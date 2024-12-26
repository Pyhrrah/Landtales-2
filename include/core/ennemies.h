#ifndef ENNEMIES_H
#define ENNEMIES_H

#include <SDL2/SDL.h>

#define MAX_ENNEMIES 25 

typedef struct {
    SDL_Rect rect;    
    int vie;          
    int attaque;      
    int defense;      
    int type;         
    int id;           
} Enemy;

extern Enemy enemies[MAX_ENNEMIES]; 
extern int enemyCount;  

void initEnemies(const char *filename);

void drawEnemies(SDL_Renderer *renderer);

void updateEnemies(SDL_Renderer *renderer);

void clearEnemies();

void removeEnemy(Enemy enemies[], int *enemyCount, int index, const char *enemyFilename);

#endif
