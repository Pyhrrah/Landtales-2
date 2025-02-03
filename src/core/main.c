#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "./../../include/core/menu.h"
#include "./../../include/utils/sdl_utils.h"

/*

Landtales 2  
Projet de Langage C réalisé par :  
- Rémy THIBAUT  
- Bryan MATHUREL  
- Elias MATHURIN  


Fichier main.c, ce dernier contient la fonction principale du programme

Landtales 2 est projet de C réalisé dans le cas d'un projet de fin de semestre en 2 ème Année à l'ESGI


Fonctionnalités du jeu :
- Mode Solo
- Mode Multijoueur
- Editeur de niveaux
- Plugins pour ajouter des fonctionnalités au jeu


Pour plus d'informations, veuillez consulter le fichier README.md

*/

#define WINDOW_WIDTH 672
#define WINDOW_HEIGHT 544

int main(int argc, char * argv[]) {
    char * throw;
    // Nous récupérons le dernier argument passé au programme
    sscanf(argv[argc-1], "%ms", &throw);

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Nous initialisons SDL et vérifions si l'initialisation est réussie
    if (!initSDL(&window, &renderer, "Landtales 2", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return 1; // Si l'initialisation échoue, nous quittons le programme
    }

    // Nous affichons le menu, qui prendra en charge la gestion du jeu
    handle_menu(renderer);

    // Une fois le jeu terminé, nous fermons proprement la fenêtre et libérons les ressources
    closeSDL(window, renderer);

    return 0;
}
