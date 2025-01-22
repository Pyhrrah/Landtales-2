// Dans editor.c
#include <dlfcn.h>
#include <stdio.h>

const char *plugin_path = "./plugins/map_editor.so";

void load_and_run_plugin(int grid[15][21]) {
    void *plugin = dlopen(plugin_path, RTLD_NOW);
    if (!plugin) {
        fprintf(stderr, "Erreur lors du chargement du plugin : %s\n", dlerror());
        return;
    }

    void (*generate_grid)(int[15][21]) = dlsym(plugin, "generate_grid");
    if (!generate_grid) {
        fprintf(stderr, "Erreur : fonction 'generate_grid' non trouvée dans le plugin\n");
        dlclose(plugin);
        return;
    }

    generate_grid(grid);  
    dlclose(plugin);
}
