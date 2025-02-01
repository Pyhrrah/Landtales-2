#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include <SDL2/SDL.h>
#include "./../core/player.h"   
#include "./../core/ennemies.h" 

/**
 * Charger un plugin
 * @param pluginPath : le chemin du plugin
 */
int loadPlugin(const char *pluginPath);

/**
 * Décharger un plugin
 */
void unloadPlugin();

/**
 * Créer un bonus
 * @param x : la position en x du bonus
 * @param y : la position en y du bonus
 */
void createBonus(int x, int y);

/**
 * Mettre à jour les bonus
 * @param player : le joueur
 */
int checkPlayerBonusCollision(Player *player);

/**
 * Rendre les bonus
 * @param renderer : le renderer
 */
void renderBonuses(SDL_Renderer *renderer);

/**
 * Ouvrir un coffre avec un plugin
 * @param player : le joueur
 */
void openChestWithPlugin(Player *player);

#endif // PLUGIN_LOADER_H
