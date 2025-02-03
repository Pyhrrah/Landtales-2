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

    // Fonction pour obtenir les erreurs sur Windows
    #define getError() GetLastError()
#else
    #include <dlfcn.h>
    typedef void* LibraryHandle;
    #define OuvrirPlugin(path) dlopen(path, RTLD_LAZY)
    #define LancerPlugin(handle, symbol) dlsym(handle, symbol)
    #define FermerPlugin(handle) dlclose(handle)

    // Fonction pour obtenir les erreurs sur Linux
    #define getError() dlerror()
#endif

void (*spawnBonus)(int, int);
int (*checkBonusCollision)(Player*);
void (*drawBonuses)(SDL_Renderer*);
void (*openChest)(Player*);

void *pluginHandle = NULL;

int loadPlugin(const char *pluginPath) {
    pluginHandle = OuvrirPlugin(pluginPath);
    if (!pluginHandle) {
        // Affichage de l'erreur appropriée
        fprintf(stderr, "Erreur lors du chargement de la bibliothèque: %s\n", getError());
        return 0;  
    }

    spawnBonus = LancerPlugin(pluginHandle, "spawnBonus");
    checkBonusCollision = LancerPlugin(pluginHandle, "checkBonusCollision");
    drawBonuses = LancerPlugin(pluginHandle, "drawBonuses");
    openChest = LancerPlugin(pluginHandle, "openChest");   

    if (!spawnBonus || !checkBonusCollision || !drawBonuses || !openChest) {
        fprintf(stderr, "Erreur d'accès aux fonctions du plugin: %s\n", getError());
        FermerPlugin(pluginHandle);  
        return 0;
    }

    printf("Plugin chargé avec succès.\n");
    return 1; 
}

// Création des fonctions pour appeler les fonctions du plugin

// Fonction pour décharger le plugin
void unloadPlugin() {
    if (pluginHandle) {
        FermerPlugin(pluginHandle);
        printf("Plugin déchargé.\n");
    }
}
// Fonction pour créer un bonus (coordonnées x et y de l'ennemi mort)
void createBonus(int x, int y) {
    if (spawnBonus) {
        spawnBonus(x, y);
    }
}
// Fonction pour vérifier la collision entre le joueur et le bonus
int checkPlayerBonusCollision(Player *player) {
    if (checkBonusCollision) {
        if(checkBonusCollision(player)){
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}
// Fonction pour dessiner les bonus
void renderBonuses(SDL_Renderer *renderer) {
    if (drawBonuses) {
        drawBonuses(renderer);
    }
}
// Fonction pour ouvrir un coffre
void openChestWithPlugin(Player *player) {
    if (openChest) {
        openChest(player);
    }
}
