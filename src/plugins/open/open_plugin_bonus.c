#include <stdio.h>
#include <dlfcn.h>
#include <SDL2/SDL.h>
#include "./../../../include/core/player.h"
#include "./../../../include/core/ennemies.h"

#ifdef _WIN32
    #include <windows.h>
    typedef HMODULE LibraryHandle;
    #define OuvrirPlugin(path) LoadLibrary(path)
    #define LancerPlugin(handle, symbol) GetProcAddress(handle, symbol)
    #define FermerPlugin(handle) FreeLibrary(handle)
#else
    #include <dlfcn.h>
    typedef void* LibraryHandle;
    #define OuvrirPlugin(path) dlopen(path, RTLD_LAZY)
    #define LancerPlugin(handle, symbol) dlsym(handle, symbol)
    #define FermerPlugin(handle) dlclose(handle)
#endif



void (*spawnBonus)(int, int);
void (*checkBonusCollision)(Player*);
void (*drawBonuses)(SDL_Renderer*);
void (*openChest)(Player*);

void *pluginHandle = NULL;

int loadPlugin(const char *pluginPath) {
    pluginHandle = OuvrirPlugin(pluginPath);
    if (!pluginHandle) {
        fprintf(stderr, "Erreur lors du chargement de la bibliothèque: %s\n", dlerror());
        return 0;  
    }

    spawnBonus = LancerPlugin(pluginHandle, "spawnBonus");
    checkBonusCollision = LancerPlugin(pluginHandle, "checkBonusCollision");
    drawBonuses = LancerPlugin(pluginHandle, "drawBonuses");
    openChest = LancerPlugin(pluginHandle, "openChest");   

    if (!spawnBonus || !checkBonusCollision || !drawBonuses) {
        fprintf(stderr, "Erreur d'accès aux fonctions du plugin: %s\n", dlerror());
        FermerPlugin(pluginHandle);  
        return 0;
    }

    printf("Plugin chargé avec succès.\n");
    return 1; 
}

void unloadPlugin() {
    if (pluginHandle) {
        FermerPlugin(pluginHandle);
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

void openChestWithPlugin(Player *player) {
    if (openChest) {
        openChest(player);
    }
}
