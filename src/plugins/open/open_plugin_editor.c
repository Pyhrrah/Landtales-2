#include <stdio.h>

#ifdef _WIN32
    #include <windows.h>
    typedef HMODULE LibraryHandle;
    #define OuvrirPlugin(path) LoadLibrary(path)
    #define LancerPlugin(handle, symbol) GetProcAddress(handle, symbol)
    #define FermerPlugin(handle) FreeLibrary(handle)

    const char *plugin_path = "./plugins/map_editor.dll";

    // Fonction pour obtenir les erreurs sur Windows
    #define getError() GetLastError()
#else
    #include <dlfcn.h>
    typedef void* LibraryHandle;
    #define OuvrirPlugin(path) dlopen(path, RTLD_LAZY)
    #define LancerPlugin(handle, symbol) dlsym(handle, symbol)
    #define FermerPlugin(handle) dlclose(handle)

    // Fonction pour obtenir les erreurs sur Linux
    const char *plugin_path = "./plugins/map_editor.so";

    
    #define getError() dlerror()
#endif


void load_and_run_plugin(int grid[15][21]) {
    void *plugin = OuvrirPlugin(plugin_path);
    if (!plugin) {
        fprintf(stderr, "Erreur lors du chargement du plugin : %s\n", getError());
        return;
    }

    void (*generate_grid)(int[15][21]) = LancerPlugin(plugin, "generate_grid");
    if (!generate_grid) {
        fprintf(stderr, "Erreur : fonction 'generate_grid' non trouvée dans le plugin\n");
        FermerPlugin(plugin);
        return;
    }

    generate_grid(grid);  

    FermerPlugin(plugin);
}
