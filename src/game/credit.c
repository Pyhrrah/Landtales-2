#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./../../include/game/map.h"
#include "./../../include/utils/video.h"

/*

Fichier credit.c, ce dernier contient la fonction pour afficher les crédits du jeu

*/

// Fonction pour afficher les crédits
void displayCredits(SDL_Renderer *renderer) {

    // Initialisation de SDL_ttf
    if (TTF_Init() < 0) {
        SDL_Log("Impossible d'initialiser SDL_ttf: %s", TTF_GetError());
        return;
    }
    init_audio();
    playSong("./assets/music/generique.mp3");

    // Chargement de la police
    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 24);
    if (!font) {
        SDL_Log("Impossible de charger la police: %s", TTF_GetError());
        TTF_Quit();
        return;
    }

    // Texte du générique
    const char *credits[] = {
        "Merci d'avoir jou\u00e9 \u00e0 Landtales 2 !",
        "Ce jeu a \u00e9t\u00e9 cr\u00e9\u00e9 par l'\u00e9quipe de Landtales.",
        "Nous esp\u00e9rons que vous avez appr\u00e9ci\u00e9 votre aventure.",
        "A bient\u00f4t !",
        "",
        "L' \u00e9quipe de Landtales 2 :",
        "",
        "R\u00e9alisateur :",
        "Bryan",
        "R\u00e9my",
        "Elias",
        "D\u00e9veloppeurs :",
        "R\u00e9my",
        "Bryan",
        "Designers :",
        "R\u00e9my",
        "Ing\u00e9nieurs SDL2 :",
        "R\u00e9my",
        "Ing\u00e9nieurs Génération :",
        "Bryan",
        "Doubleurs :",
        "Killian BIDAUX",
        "Supports : ",
        "PDF de 1166 pages de Monsieur Trancho",
        "Documentation SDL2",
        "Documentation FFMPEG",
        "Obscurantisme au laboratoire CodinGame",
        " ",
        "CODE SECRET : ",
        "trancho",
        "Merci et à bientôt pour de nouvelles aventures !",
        NULL
    };

    // Couleurs
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    int screen_width = 672;
    int screen_height = 544;

    int scroll_speed = 2;

    SDL_Surface *textSurface = NULL;
    SDL_Texture *textTexture = NULL;

    int y_position = screen_height;

    int running = 1;
    SDL_Event event;

    // Boucle d'affichage des crédits 
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Nettoyage de l'écran
        SDL_SetRenderDrawColor(renderer, black.r, black.g, black.b, black.a);
        SDL_RenderClear(renderer);


        // Affichage des crédits
        int current_y = y_position;
        for (int i = 0; credits[i] != NULL; i++) {
            textSurface = TTF_RenderUTF8_Blended(font, credits[i], white);
            if (!textSurface) continue;

            // Création de la texture
            textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_FreeSurface(textSurface);

            if (!textTexture) continue;

            // Récupération de la taille du texte
            int text_width, text_height;
            SDL_QueryTexture(textTexture, NULL, NULL, &text_width, &text_height);

            SDL_Rect textRect = { (screen_width - text_width) / 2, current_y, text_width, text_height };
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

            // Nettoyage de la texture
            SDL_DestroyTexture(textTexture);
            current_y += text_height + 10;
        }

        SDL_RenderPresent(renderer);

        // Défilement des crédits
        y_position -= scroll_speed;

        if (current_y < 0) {
            running = 0;
        }

        SDL_Delay(16);
    }

    // Nettoyage
    check_and_free_music();
    cleanup_audio();
    TTF_CloseFont(font);
    TTF_Quit();
}

// Fonction pour sauvegarder les données après une victoire
void saveWinGame(int saveNumber, int tentative, int vie, int attaque, int defense, int etage, int piece, int room, int max_vie) {
    char playerFilename[100];
    sprintf(playerFilename, "./data/game/save%d/savePlayer.txt", saveNumber);

    // Ouverture du fichier
    FILE *file = fopen(playerFilename, "w");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier %s pour la sauvegarde.\n", playerFilename);
        return;
    }

    // Sauvegarde des données
    tentative += 1;
    vie = max_vie;
    etage = 1;
    piece = 0;
    room = 0;

    // Écriture des données
    fprintf(file, "%d %d %d %d %d %d %d %d", tentative, vie, attaque, defense, etage, piece, room, max_vie);
    fclose(file);

    printf("Données sauvegardées après victoire : Tentative: %d, Vie: %d, Attaque: %d, Defense: %d, Etage: %d, Piece: %d, Salle: %d\n",
           tentative, vie, attaque, defense, etage, piece, room);

}
