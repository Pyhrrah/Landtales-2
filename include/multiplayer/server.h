#ifndef SERVER_H
#define SERVER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SERVER_PORT 12345

/**
 * Démarre le serveur et initialise la grille de jeu.
 * 
 * Cette fonction démarre un serveur qui écoute les connexions entrantes sur un port défini.
 * Elle initialise également la grille de jeu pour les interactions avec les clients.
 * 
 * @param grid : Tableau 2D représentant la grille du jeu, utilisé pour envoyer les données du jeu 
 *               aux clients et pour la gestion de l'état du jeu.
 */
void start_server(int grid[15][21]);

#endif // SERVER_H
