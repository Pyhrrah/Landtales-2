#include "./../../../include/core/player.h"

// Fonction pour calculer les dégâts infligés par le joueur aux ennemis

/*

Ce plugin permet de calculer les dégâts infligés par le joueur aux ennemis en fonction de l'attaque du joueur, de la défense de l'ennemi et du type d'arme utilisé.

Types d'armes :
- EPEE : dégâts normaux
- ARC : dégâts divisés par 2
- FOUDRE : dégâts maximaux


L'utilisateur est libre de personnaliser les dégâts infligés par les armes en modifiant les valeurs de dégâts dans le code.
*/

/**
 * Calculer les dégâts infligés par le joueur
 * @param playerAttack : l'attaque du joueur
 * @param ennemyDefense : la défense de l'ennemi
 * @param type : le type d'arme utilisé
 */
int calculateDamage(int playerAttack, int ennemyDefense, ArmeType type) {
    int damage;

    switch (type) {
        case EPEE:
            damage = playerAttack - ennemyDefense;  
            break;
        case ARC:
            damage = playerAttack - (ennemyDefense * 2);  
            break;
        case FOUDRE:
            damage = playerAttack;  
            break;
        default:
            damage = playerAttack - ennemyDefense;  
            break;
    }

    if (damage < 0) {
        damage = 0; 
    }

    damage += (rand() % 11) - 6;

    if (damage < 0) {
        damage = 0; 
    }

    switch (type) {
        case EPEE:
            damage += 5;  
            break;
        case ARC:
            damage += 2;  
            break;
        case FOUDRE:
            damage += 10; 
            break;
        default:
            break;
    }
    printf("Dégâts infligés : %d\n", damage);
    return damage;
}