#include "./../../include/editor/colors.h"
#include <SDL2/SDL_image.h>

// Couleurs des tuiles

SDL_Color colors[] = {
    {100, 255, 100, 255}, 
    {70, 200, 70, 255}, 
    {50, 150, 50, 255}, 
    {30, 100, 30, 255}, 
    {10, 50, 10, 255}, 
    {150, 75, 0, 255}, 
    {100, 100, 100, 255}, 
    {100, 100, 100, 255}, 
    {0, 0, 255, 255},
    {128, 128, 128, 255}, 
    {169, 169, 169, 255}, 
    {139, 69, 19, 255}, 
    {128, 0, 128, 255}, 
    {255, 192, 203, 255}, 
};

size_t get_colors_count() {
    return sizeof(colors) / sizeof(colors[0]);
}

SDL_Texture *textures[14];
// Fonction pour charger les textures
void load_textures(SDL_Renderer *renderer) {
    const char *texture_paths[] = {
        "assets/images/sprite/map/sol1.png",  // SOL
        "assets/images/sprite/map/sol2.png",  // SOL_VARIANT_1
        "assets/images/sprite/map/sol3.png",  // SOL_VARIANT_2
        "assets/images/sprite/map/sol4.png",  // SOL_VARIANT_3
        "assets/images/sprite/map/sol5.png",  // SOL_VARIANT_4
        "assets/images/sprite/map/mur1.png",  // WALL_FRONT
        "assets/images/sprite/map/mur2.png",  // WALL_LEFT
        "assets/images/sprite/map/mur3.png",  // WALL_RIGHT
        "assets/images/sprite/map/water.png", // WATER_BLOCK
        "assets/images/sprite/map/rock.png",  // ROCK
        "assets/images/sprite/map/debris.png",// DEBRIS
        "assets/images/sprite/map/stump.png",  // STUMP
        "assets/images/sprite/map/spawn1.png",  // SPAWN_PLAYER_1
        "assets/images/sprite/map/spawn2.png",  // SPAWN_PLAYER_2
    };

    size_t texture_count = sizeof(texture_paths) / sizeof(texture_paths[0]);

    for (size_t i = 0; i < texture_count; i++) {
        textures[i] = IMG_LoadTexture(renderer, texture_paths[i]);
    }
}