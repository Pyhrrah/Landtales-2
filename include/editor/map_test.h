#ifndef MAP_TEST_H
#define MAP_TEST_H

#include <SDL2/SDL.h>

// Définitions des dimensions de la grille
#define GRID_WIDTH 21  // Largeur de la grille en nombre de cases
#define GRID_HEIGHT 15 // Hauteur de la grille en nombre de cases

/**
 * Vérifie si un déplacement vers une position donnée est valide.
 * 
 * @param x : Coordonnée en X de la position à vérifier.
 * @param y : Coordonnée en Y de la position à vérifier.
 * @param grid : La grille actuelle (tableau 2D) représentant la carte.
 * @return 1 si le déplacement est valide, 0 sinon.
 */
int is_valid_move(int x, int y, int grid[GRID_WIDTH][GRID_HEIGHT]);

/**
 * Teste une carte donnée en affichant ou en validant son contenu.
 * 
 * @param renderer : Le renderer SDL utilisé pour afficher la carte.
 * @param grid : La grille représentant la carte à tester.
 * @return Un code de retour pouvant indiquer un état spécifique (par exemple, succès ou échec du test).
 */
int test_map(SDL_Renderer *renderer, int grid[GRID_WIDTH][GRID_HEIGHT]);

#endif // MAP_TEST_H
