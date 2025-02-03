#include "./../../include/core/ennemies.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "./../../include/plugins/open_plugin_bonus.h"
#include "./../../include/utils/sdl_utils.h"
#include "./../../include/game/projectile.h"

/*
Fichier ennemies.c, ce dernier contient les fonctions pour gérer les ennemis du jeu (initialisation, affichage, mise à jour, etc.)
*/


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

void loadEnemyStats(const char *configFile) {
    FILE *file = fopen(configFile, "r");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir %s\n", configFile);
        return;
    }

    // Lecture des statistiques des ennemis
    int fileType, vie, attaque, defense;
    while (fscanf(file, "%d %d %d %d", &fileType, &vie, &attaque, &defense) == 4) {
        // Mise à jour des statistiques pour chaque ennemi
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i].type == fileType) {
                enemies[i].vie = vie;
                enemies[i].attaque = attaque;
                enemies[i].defense = defense;
            }
        }
    }
    
    fclose(file);
}

void initEnemies(const char *filename) {
    const char *configFile = "./assets/enemies.txt";
    FILE *file = fopen(filename, "r");

    if (!file) {
        printf("Erreur : Impossible d'ouvrir %s\n", filename);
        return;
    }

    clearEnemies();
    enemyCount = 0;
    int x, y, type;
    // Lecture des ennemis depuis le fichier
    while (fscanf(file, "%d %d %d", &x, &y, &type) == 3 && enemyCount < MAX_ENNEMIES) {
        enemies[enemyCount] = (Enemy){ .rect = {x, y, 32, 32}, .type = type, .id = enemyCount };
        enemyCount++;
    }

    fclose(file);
    
    // Charger les statistiques pour tous les ennemis
    loadEnemyStats(configFile);
}

// Fonction pour dessiner les ennemis
void drawEnemies(SDL_Renderer *renderer) {
    const char *basePath = "./assets/images/sprite/monster/";
    const char *defaultImage = "./assets/images/sprite/monster/monster1.png";

    // Pour chaque ennemi, on charge son image et on la dessine
    for (int i = 0; i < enemyCount; i++) {
        char imagePath[128];
        snprintf(imagePath, sizeof(imagePath), "%sIcon%d.png", basePath, enemies[i].type);

        SDL_Texture *enemyTexture = load_texture(renderer, imagePath);
        // Si le chargement de l'image a échoué, on charge l'image par défaut
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
    // On réinitialise le nombre d'ennemis à 0 à chaque changement de salle / étage / sortie du jeu
    enemyCount = 0;

}


// Fonction pour supprimer un ennemi d'un fichier
void removeEnemy(Enemy enemies[], int *enemyCount, int index, const char *enemyFilename) {
    if (index < 0 || index >= *enemyCount) {
        printf("Indice d'ennemi invalide : %d\n", index);
        return;
    }

    printf("Suppression de l'ennemi %d (Type %d)\n", index, enemies[index].type);

    // Création du bonus au centre de l'ennemi
    int bonusX = enemies[index].rect.x + (enemies[index].rect.w / 2) - (bonuses[0].rect.w / 2);
    int bonusY = enemies[index].rect.y + (enemies[index].rect.h / 2) - (bonuses[0].rect.h / 2);

    // Décalage des ennemis restants
    for (int i = index; i < *enemyCount - 1; i++) {
        enemies[i] = enemies[i + 1];
    }

    (*enemyCount)--;

    // Ajout du bonus
    loadPlugin(PLUGIN_BONUS);
    createBonus(bonusX, bonusY);
    unloadPlugin();

    // Mise à jour du fichier des ennemis
    FILE *file = fopen(enemyFilename, "w");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier %s\n", enemyFilename);
        return;
    }

    // Réécriture complète du fichier
    for (int i = 0; i < *enemyCount; i++) {
        fprintf(file, "%d %d %d\n", 
                enemies[i].rect.x, 
                enemies[i].rect.y, 
                enemies[i].type);
    }

    fclose(file);
    printf("Fichier %s mis à jour avec succès.\n", enemyFilename);
}


void launchProjectileWithCooldownEnnemies(SDL_Rect *playerRect) {
    Uint32 currentTime = SDL_GetTicks();
    // Si le nombre d'ennemis est nul, on ne fait rien
    if (enemyCount <= 0) return;  
    // Si le temps écoulé depuis le dernier projectile est supérieur à 3 secondes, on tire
    if (currentTime > lastProjectileTime + 3000) {  
        // On tire sur un nombre aléatoire d'ennemis (entre 1 et enemyCount / 2)
        int numShooters = (enemyCount > 1) ? (rand() % (enemyCount / 2 + 1) + 1) : 1; 
        int selectedEnemies[MAX_ENNEMIES] = {0};  

        for (int i = 0; i < numShooters; i++) {
            // On choisit un ennemi aléatoire qui n'a pas encore tiré
            int randomIndex;
            do {
                randomIndex = rand() % enemyCount;
            } while (selectedEnemies[randomIndex]); 
            // On marque l'ennemi comme ayant tiré
            selectedEnemies[randomIndex] = 1;  
            // On tire un projectile depuis l'ennemi avec une vitesse de 5.0f
            spawnProjectile(&enemies[randomIndex].rect, playerRect, 5.0f);
        }

        lastProjectileTime = currentTime;  
    }
}