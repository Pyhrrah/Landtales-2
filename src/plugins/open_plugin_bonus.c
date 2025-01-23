#include <stdio.h>
#include <dlfcn.h>
#include <SDL2/SDL.h>
#include "./../../include/core/player.h"
#include "./../../include/core/ennemies.h"


void (*spawnBonus)(int, int);
void (*checkBonusCollision)(Player*);
void (*drawBonuses)(SDL_Renderer*);

void *pluginHandle = NULL;

int loadPlugin(const char *pluginPath) {
    pluginHandle = dlopen(pluginPath, RTLD_LAZY);
    if (!pluginHandle) {
        fprintf(stderr, "Erreur lors du chargement de la bibliothèque: %s\n", dlerror());
        return 0;  
    }

    spawnBonus = dlsym(pluginHandle, "spawnBonus");
    checkBonusCollision = dlsym(pluginHandle, "checkBonusCollision");
    drawBonuses = dlsym(pluginHandle, "drawBonuses");

    if (!spawnBonus || !checkBonusCollision || !drawBonuses) {
        fprintf(stderr, "Erreur d'accès aux fonctions du plugin: %s\n", dlerror());
        dlclose(pluginHandle);  
        return 0;
    }

    printf("Plugin chargé avec succès.\n");
    return 1; 
}

void unloadPlugin() {
    if (pluginHandle) {
        dlclose(pluginHandle);
        printf("Plugin déchargé.\n");
    }
}

void createBonus(int x, int y) {
    if (spawnBonus) {
        spawnBonus(x, y);
    }
}

void checkPlayerBonusCollision(Player *player) {
    if (checkBonusCollision) {
        checkBonusCollision(player);
    }
}

void renderBonuses(SDL_Renderer *renderer) {
    if (drawBonuses) {
        drawBonuses(renderer);
    }
}
