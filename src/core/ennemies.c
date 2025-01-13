#include "./../../include/core/ennemies.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

Enemy enemies[MAX_ENNEMIES];  
int enemyCount = 0;      
Bonus bonuses[5];




// Définition des couleurs des ennemis
SDL_Color enemyColors[15] = {
    {255, 0, 0, 255},        
    {255, 165, 0, 255},      
    {255, 255, 0, 255},      
    {0, 255, 0, 255},        
    {0, 0, 255, 255},       
    {75, 0, 130, 255},       
    {238, 130, 238, 255},    
    {255, 20, 147, 255},     
    {255, 99, 71, 255},      
    {0, 255, 255, 255},      
    {255, 105, 180, 255},    
    {255, 140, 0, 255},      
    {64, 224, 208, 255},     
    {127, 255, 0, 255},      
    {255, 182, 193, 255}     
};

// Fonction pour initialiser les ennemis à partir d'un fichier
void initEnemies(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier des ennemis %s\n", filename);
        return;
    }

    clearEnemies();

    int x, y, type;
    int id = 0; 
    while (fscanf(file, "%d %d %d", &x, &y, &type) == 3) {
        if (enemyCount < MAX_ENNEMIES) {
            enemies[enemyCount].rect.x = x;
            enemies[enemyCount].rect.y = y;
            enemies[enemyCount].rect.w = 32; 
            enemies[enemyCount].rect.h = 32; 
            enemies[enemyCount].vie = 100;   
            enemies[enemyCount].attaque = 10; 
            enemies[enemyCount].defense = 5;
            enemies[enemyCount].type = type;
            enemies[enemyCount].id = id++;

            enemyCount++;  
        } else {
            printf("Trop d'ennemis dans le fichier, limite atteinte.\n");
            break;
        }
    }

    fclose(file);
}

// Fonction pour dessiner les ennemis
void drawEnemies(SDL_Renderer *renderer) {
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].type >= 1 && enemies[i].type <= 15) {
            SDL_SetRenderDrawColor(renderer, enemyColors[enemies[i].type - 1].r, 
                                    enemyColors[enemies[i].type - 1].g, 
                                    enemyColors[enemies[i].type - 1].b, 
                                    enemyColors[enemies[i].type - 1].a);
        } else {
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        }

        SDL_RenderFillRect(renderer, &enemies[i].rect);
    }
}

// Fonction pour mettre à jour les ennemis (temporaire)
void updateEnemies(SDL_Renderer *renderer) {
    for (int i = 0; i < enemyCount; i++) {
        enemies[i].rect.x += 2;

        drawEnemies(renderer);
    }
}

// Fonction pour effacer les ennemis
void clearEnemies() {
    printf("Effacement de tous les ennemis\n");
    enemyCount = 0;

}

// Fonction pour initialiser les bonus
void initBonuses() {
    for (int i = 0; i < 5; i++) {
        bonuses[i].active = 0;
    }
}


// Fonction pour faire apparaître un bonus
void spawnBonus(int x, int y, int type) {
    for (int i = 0; i < 5; i++) {
        if (!bonuses[i].active) {
            bonuses[i].rect.x = x;
            bonuses[i].rect.y = y;
            bonuses[i].rect.w = 32 / 2;
            bonuses[i].rect.h = 32 / 2;
            bonuses[i].type = type;
            bonuses[i].active = 1;
            printf("Bonus de type %d généré à (%d, %d)\n", type, x, y);
            return;
        }
    }
    printf("Nombre maximum de bonus atteint, aucun nouveau bonus généré.\n");
}


// Fonction pour supprimer un ennemi d'un fichier
void removeEnemy(Enemy enemies[], int *enemyCount, int index, const char *enemyFilename) {
    if (index < 0 || index >= *enemyCount) {
        printf("Indice d'ennemi invalide : %d\n", index);
        return;
    }

    printf("Suppression de l'ennemi %d\n", index);

    int bonusX = enemies[index].rect.x + (enemies[index].rect.w / 2) - (bonuses[0].rect.w / 2);
    int bonusY = enemies[index].rect.y + (enemies[index].rect.h / 2) - (bonuses[0].rect.h / 2);

    for (int i = index; i < *enemyCount - 1; i++) {
        enemies[i] = enemies[i + 1];
    }
    (*enemyCount)--;

    if (rand() % 10 != 3) {
        int bonusType = rand() % 2;
        spawnBonus(bonusX, bonusY, bonusType);
    }
    

    FILE *file = fopen(enemyFilename, "w");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier %s\n", enemyFilename);
        return;
    }

    for (int i = 0; i < *enemyCount; i++) {
        fprintf(file, "%d %d %d\n", 
                enemies[i].rect.x, 
                enemies[i].rect.y, 
                enemies[i].type);
    }

    fclose(file);
    printf("Le fichier %s a été mis à jour avec succès.\n", enemyFilename);
}


// Fonction pour dessiner les bonus
void drawBonuses(SDL_Renderer *renderer) {
    for (int i = 0; i < 5; i++) {
        if (bonuses[i].active) {
            SDL_SetRenderDrawColor(renderer, bonuses[i].type == 0 ? 0 : 255, bonuses[i].type == 0 ? 255 : 255, 0, 255);  
            SDL_RenderFillRect(renderer, &bonuses[i].rect);
        }
    }
}
