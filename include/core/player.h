#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    int health;
    int maxHealth;
    int damage;
    int defense;
    SDL_Rect rect;
    int arrowCount;
    int maxArrows;
    Uint32 lastRegenTime;
    Uint32 lastArrowTime;
    Uint32 lastLightningUse;
    int lightningCooldown;
    bool lightningReady;
    int id;
} Player;

void initPlayer(Player *player, int x, int y, int w, int h);
void regenerateHealth(Player *player);
void recoverArrow(Player *player);
void swordAttack(Player *player);
void arrowAttack(Player *player);
void lightningAttack(Player *player);
void takeDamage(Player *player, int damage);
void updatePlayer(Player *player);

#endif
