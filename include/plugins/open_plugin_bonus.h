#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include <SDL2/SDL.h>
#include "./../core/player.h"   
#include "./../core/ennemies.h" 

int loadPlugin(const char *pluginPath);

void unloadPlugin();

void createBonus(int x, int y);

void checkPlayerBonusCollision(Player *player);

void renderBonuses(SDL_Renderer *renderer);

#endif // PLUGIN_LOADER_H
