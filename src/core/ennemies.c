#include "./../../include/core/ennemies.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "./../../include/plugins/open_plugin_bonus.h"
#include "./../../include/utils/sdl_utils.h"
#include "./../../include/game/projectile.h"


#ifdef _WIN32
    #define PLUGIN_BONUS "./plugins/plugin_bonus.dll"
#else
    #define PLUGIN_BONUS "./plugins/plugin_bonus.so"
#endif

Enemy enemies[MAX_ENNEMIES];  
int enemyCount = 0;      
Bonus bonuses[5];
static Uint32 lastProjectileTime = 0;  


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
            enemies[enemyCount].defense = 1000;
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
    const char *basePath = "./assets/images/sprite/monster/";
    const char *defaultImage = "./assets/images/sprite/monster/monster1.png";

    for (int i = 0; i < enemyCount; i++) {
        char imagePath[128];
        snprintf(imagePath, sizeof(imagePath), "%sIcon%d.png", basePath, enemies[i].type);

        SDL_Texture *enemyTexture = load_texture(renderer, imagePath);
        if (!enemyTexture) {
            enemyTexture = load_texture(renderer, defaultImage);
            if (!enemyTexture) {
                continue; 
            }
        }

        SDL_RenderCopy(renderer, enemyTexture, NULL, &enemies[i].rect);

        SDL_DestroyTexture(enemyTexture);
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
    loadPlugin(PLUGIN_BONUS);
    createBonus(bonusX, bonusY);
    unloadPlugin();
    

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


void launchProjectileWithCooldownEnnemies(SDL_Rect *playerRect) {
    Uint32 currentTime = SDL_GetTicks();

    if (enemyCount <= 0) return;  
    if (currentTime > lastProjectileTime + 3000) {  
        int numShooters = (enemyCount > 1) ? (rand() % (enemyCount / 2 + 1) + 1) : 1; 
        int selectedEnemies[MAX_ENNEMIES] = {0};  

        for (int i = 0; i < numShooters; i++) {
            int randomIndex;
            do {
                randomIndex = rand() % enemyCount;
            } while (selectedEnemies[randomIndex]); 

            selectedEnemies[randomIndex] = 1;  
            spawnProjectile(&enemies[randomIndex].rect, playerRect, 3.0f);
        }

        lastProjectileTime = currentTime;  
    }
}