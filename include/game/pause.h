#ifndef PAUSE_H
#define PAUSE_H

#include <SDL2/SDL.h>

/**
 * Affiche le menu de pause du jeu.
 * 
 * Cette fonction est responsable du dessin de l'interface du menu de pause, 
 * incluant les options de reprise ou de quitter le jeu.
 * 
 * @param renderer : Le renderer SDL utilisé pour dessiner le menu de pause.
 */
void drawPauseMenu(SDL_Renderer *renderer);

/**
 * Gère les événements liés au menu de pause.
 * 
 * Cette fonction vérifie les événements utilisateur pour permettre de reprendre le jeu ou de le quitter.
 * Elle modifie l'état de la pause en fonction des interactions.
 * 
 * @param event : Pointeur vers l'événement SDL pour gérer les interactions utilisateur.
 * @param gamePaused : Pointeur vers un indicateur de l'état du jeu (true si le jeu est en pause, false sinon).
 * 
 * @return true si l'état du jeu a été modifié, sinon false.
 */
int handlePauseMenu(SDL_Event *event, int *gamePaused);

/**
 * Gère le clic sur les boutons du menu de pause.
 * 
 * Cette fonction vérifie si l'utilisateur a cliqué sur les boutons "Reprendre" ou "Quitter" du menu de pause.
 * Elle met à jour les états de pause et de jeu en fonction des actions de l'utilisateur.
 * 
 * @param event : Pointeur vers l'événement SDL pour gérer les clics de l'utilisateur.
 * @param resumeButton : Rectangle représentant la zone du bouton "Reprendre".
 * @param quitButton : Rectangle représentant la zone du bouton "Quitter".
 * @param gamePaused : Pointeur vers un indicateur de l'état du jeu (true si en pause, false sinon).
 * @param running : Pointeur vers un indicateur de l'état du programme (true si en cours, false pour quitter).
 * 
 * @return true si un clic valide a été effectué, sinon false.
 */
int handleButtonClick(SDL_Event *event, SDL_Rect resumeButton, SDL_Rect quitButton, int *gamePaused, int *running);

#endif // PAUSE_H
