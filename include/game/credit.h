#ifndef CREDITS_H
#define CREDITS_H

#include <SDL2/SDL.h>

/**
 * Affiche les crédits du jeu.
 * Cette fonction affiche un générique de fin animé ou statique à l'écran.
 * 
 * @param renderer : Le renderer SDL utilisé pour dessiner les crédits.
 */
void displayCredits(SDL_Renderer *renderer);

/**
 * Sauvegarde l'état de la partie lorsque le joueur a gagné.
 * 
 * @param saveNumber : Numéro de la sauvegarde à mettre à jour.
 * @param tentative : Nombre de tentatives effectuées par le joueur.
 * @param vie : Vie restante du joueur.
 * @param attaque : Niveau d'attaque du joueur.
 * @param defense : Niveau de défense du joueur.
 * @param etage : Numéro de l'étage atteint par le joueur.
 * @param piece : Montant d'argent (pièces) collecté par le joueur.
 * @param room : Numéro de la salle où se trouve le joueur.
 * @param max_vie : Niveau maximum de vie du joueur.
 */
void saveWinGame(int saveNumber, int tentative, int vie, int attaque, int defense, int etage, int piece, int room, int max_vie);

#endif // CREDITS_H
