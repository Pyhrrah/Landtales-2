#ifndef GAME_OVER_H
#define GAME_OVER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./../core/player.h"

/**
 * Affiche l'écran "Game Over".
 * 
 * @param renderer : Le renderer SDL utilisé pour dessiner l'écran.
 * @param event : Pointeur vers l'événement SDL pour gérer les entrées utilisateur.
 * @param running : Pointeur vers un indicateur de l'état du programme (1 si en cours, 0 pour quitter).
 */
void displayGameOverScreen(SDL_Renderer *renderer, SDL_Event *event, int *running);

/**
 * Gère la logique et l'affichage de l'écran de fin de jeu.
 * 
 * @param player : Pointeur vers la structure du joueur contenant ses données actuelles.
 * @param renderer : Le renderer SDL utilisé pour les affichages.
 * @param saveNumber : Numéro de la sauvegarde à utiliser.
 * @param room : Pointeur vers le numéro de la salle actuelle.
 * @param etage : Pointeur vers le numéro de l'étage actuel.
 * @param mapRoom : Tableau 2D représentant la salle actuelle.
 * @param tentative : Pointeur vers le nombre de tentatives effectuées par le joueur.
 * @param mapData : Tableau 2D représentant les données de la carte.
 * @param stageFilename : Nom du fichier de sauvegarde pour l'étage.
 * @param event : Pointeur vers l'événement SDL pour gérer les interactions utilisateur.
 * @param running : Pointeur vers un indicateur de l'état du programme (true si en cours, false pour quitter).
 */
void gameOver(Player *player, SDL_Renderer *renderer, int saveNumber, int *room, int *etage, 
              int mapRoom[15][21], int *tentative, int mapData[11][11], char *stageFilename, 
              SDL_Event *event, int *running);

#endif // GAME_OVER_H
