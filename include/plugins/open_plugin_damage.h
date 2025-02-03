// open_plugin_damage.h
#ifndef PLUGIN_DAMAGE_H
#define PLUGIN_DAMAGE_H
#include "./../../include/core/player.h"

/**
 * Calculer les dégâts infligés par le joueur
 * @param playerAttack : l'attaque du joueur
 * @param ennemyDefense : la défense de l'ennemi
 * @param type : le type d'arme utilisé
 */
int calculerDegats(int playerAttack, int ennemyDefense, ArmeType type);

#endif // PLUGIN_DAMAGE_H