#include <stdio.h>
#include "./../../../include/core/player.h"


#ifdef _WIN32
    #include <windows.h>
    typedef HMODULE LibraryHandle;
    #define OuvrirPlugin(path) LoadLibrary(path)
    #define LancerPlugin(handle, symbol) GetProcAddress(handle, symbol)
    #define FermerPlugin(handle) FreeLibrary(handle)

    const char *plugin_path = "./plugins/map_damage.dll";

    // Fonction pour obtenir les erreurs sur Windows
    #define getError() GetLastError()
#else
    #include <dlfcn.h>
    typedef void* LibraryHandle;
    #define OuvrirPlugin(path) dlopen(path, RTLD_LAZY)
    #define LancerPlugin(handle, symbol) dlsym(handle, symbol)
    #define FermerPlugin(handle) dlclose(handle)

    // Fonction pour obtenir les erreurs sur Linux
    const char *chemin_plugin = "./plugins/plugin_damage.so";

    
    #define getError() dlerror()
#endif

// Fonction pour calculer les dégâts infligés par le joueur en appelant la fonction du plugin
int calculerDegats(int playerAttack, int ennemyDefense, ArmeType type) {
    void *plugin = OuvrirPlugin(chemin_plugin);
    if (!plugin) {
        fprintf(stderr, "Erreur lors du chargement du plugin : %s\n", getError());
        return 10;
    }

    int (*damage)(int playerAttack, int ennemyDefense, ArmeType type) = LancerPlugin(plugin, "calculateDamage");
    if (!damage) {
        fprintf(stderr, "Erreur : fonction 'calculateDamage' non trouvée dans le plugin\n");
        FermerPlugin(plugin);
        return 10;
    }

    int degat = damage(playerAttack, ennemyDefense, type);

    FermerPlugin(plugin);

    return degat;
}
