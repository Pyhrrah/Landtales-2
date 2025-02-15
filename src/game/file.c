#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define TILE_SIZE 32
#define ROWS 15
#define COLS 21

// Déclaration du tableau des textures global
SDL_Texture* tileTextures[20];

// Fonction pour charger les textures des sprites
int loadTextures(SDL_Renderer *renderer) {
    const char* tilePaths[20] = {
        "./assets/images/sprite/map/sol1.png",   // 1 : tile sol normal
        "./assets/images/sprite/map/sol2.png",   // 2 : tile sol variation 1
        "./assets/images/sprite/map/sol3.png",   // 3 : tile sol variation 2
        "./assets/images/sprite/map/sol4.png",   // 4 : tile sol variation 3
        "./assets/images/sprite/map/mur1.png",   // 5 : mur haut et bas map
        "./assets/images/sprite/map/mur2.png",   // 6 : mur gauche
        "./assets/images/sprite/map/mur3.png",   // 7 : mur droite
        "./assets/images/sprite/map/sol1.png",  // 8 : porte mur haut et bas map
        "./assets/images/sprite/map/sol1.png",  // 9 : porte mur gauche
        "./assets/images/sprite/map/sol1.png",  // 10: porte mur droite
        "./assets/images/sprite/map/water.png",  // 11: tile bloc d'eau
        "./assets/images/sprite/map/rock.png",   // 12: rocher
        "./assets/images/sprite/map/stump.png",  // 13: souche d'arbre
        "./assets/images/sprite/map/debris.png", // 14: débris
        "./assets/images/sprite/map/pilier1.png",// 15: pilier haut gauche (hg)
        "./assets/images/sprite/map/pilier2.png",// 16: pilier haut droit (hd)
        "./assets/images/sprite/map/pilier3.png",// 17: pilier bas droit (bd)
        "./assets/images/sprite/map/pilier4.png", // 18: pilier bas gauche (bg)
        "./assets/images/sprite/map/coffre.png",   // 19 : tile sol normal
        "./assets/images/sprite/map/sol1.png",  // 20 : tile sol normal
    };

    // Charger toutes les images et créer les textures
    for (int i = 0; i < 20; i++) {
        SDL_Surface* surface = IMG_Load(tilePaths[i]);
        if (!surface) {
            printf("Erreur de chargement de l'image %s: %s\n", tilePaths[i], SDL_GetError());
            return -1;
        }

        // Créer la texture à partir de la surface
        tileTextures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface); 

        if (!tileTextures[i]) {
            printf("Erreur de création de la texture pour %s: %s\n", tilePaths[i], SDL_GetError());
            return -1;
        }

    }

    return 0;
}

// Fonction pour créer une salle entière
void drawMap(int room[ROWS][COLS], SDL_Renderer *renderer) {
    // Charger les textures au début de la fonction si ce n'est pas déjà fait
    static int texturesLoaded = 0;
    if (!texturesLoaded) {
        if (loadTextures(renderer) != 0) {
            printf("Erreur lors du chargement des textures\n");
            return;  // Arrête si les textures ne peuvent pas être chargées
        }
        texturesLoaded = 1;  // Assure-toi que les textures ne sont chargées qu'une seule fois
    }

    // Tableau des couleurs pour les tuiles sans sprites
    SDL_Color colors[20] = {
        {50, 205, 50, 255},   // 1 : tile sol normal
        {60, 179, 113, 255},  // 2 : tile sol variation 1
        {34, 139, 34, 255},   // 3 : tile sol variation 2
        {46, 139, 87, 255},   // 4 : tile sol variation 3
        {139, 69, 19, 255},   // 5 : mur haut et bas map
        {139, 69, 19, 255},   // 6 : mur gauche
        {139, 69, 19, 255},   // 7 : mur droite
        {210, 105, 30, 255},  // 8 : porte mur haut et bas map
        {210, 105, 30, 255},  // 9 : porte mur gauche
        {210, 105, 30, 255},  // 10: porte mur droite
        {30, 144, 255, 255},  // 11: tile bloc d'eau
        {169, 169, 169, 255}, // 12: rocher
        {139, 69, 19, 255},   // 13: souche d'arbre
        {128, 128, 128, 255}, // 14: débris
        {192, 192, 192, 255}, // 15: pilier haut gauche (hg)
        {192, 192, 192, 255}, // 16: pilier haut droit (hd)
        {192, 192, 192, 255}, // 17: pilier bas droit (bd)
        {192, 192, 192, 255},  // 18: pilier bas gauche (bg)
        {192, 192, 192, 255},  // 19: coffre
        {50, 205, 50, 255},  // 20: tile sol normal
    };

    // Parcourir chaque tuile de la salle et la dessiner
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int tileId = room[i][j];  

            // Si l'ID de la tuile est valide, on la dessine
            if (tileId > 0 && tileId <= 20) {
                SDL_Rect tileRect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };

                // Si une texture est disponible pour cette tuile, on l'utilise
                if (tileTextures[tileId - 1]) {
                    SDL_RenderCopy(renderer, tileTextures[tileId - 1], NULL, &tileRect); 
                } else {
                    SDL_Color tileColor = colors[tileId - 1]; 
                    SDL_SetRenderDrawColor(renderer, tileColor.r, tileColor.g, tileColor.b, tileColor.a);
                    SDL_RenderFillRect(renderer, &tileRect); 
                }
            }
        }
    }
}



// Fonction pour charger les données d'une salle depuis un fichier
void loadRoomData(const char* filename, int room[ROWS][COLS]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur d'ouverture du fichier %s\n", filename);
        return;
    }

    // Lire les données de la salle
    int x, y, value;
    int i = 0, j = 0;
    
    // Lire les données de la salle
    while (fscanf(file, "%d %d %d", &x, &y, &value) == 3) {
        if (i < ROWS && j < COLS) {
            room[i][j] = value;
            j++;
            if (j == COLS) {
                j = 0;
                i++;
            }
        }
    }

    fclose(file);
}

// Fonction pour charger la map depuis un fichier
int loadMap(const char* filename, int data[11][11]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        return -1;
    }

    // Lire les données de la map
    int i = 0, j = 0;
    // Tant qu'on peut lire un entier, on le stocke dans le tableau
    while (fscanf(file, "%d", &data[i][j]) == 1) {
        j++;
        if (j >= 11) {
            j = 0;
            i++;
        }

        if (i >= 11) {
            printf("Le fichier contient plus de données que prévu. Seules les 121 premières valeurs ont été lues.\n");
            break;
        }
    }

    fclose(file);
    return (i * 11 + j); 
}

// Fonction pour sauvegarder les données du joueur
void saveGame(int saveNumber, int tentative, int vie, int attaque, int defense, int etage, int piece, int room, int max_vie) {
    char playerFilename[100];
    sprintf(playerFilename, "./data/game/save%d/savePlayer.txt", saveNumber);

    // Sauvegarde des données du joueur dans un fichier
    FILE *file = fopen(playerFilename, "w");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier %s pour la sauvegarde.\n", playerFilename);
        return;
    }

    room = (room == 100) ? 0 : room;

    // Écriture des données dans le fichier
    fprintf(file, "%d %d %d %d %d %d %d %d", tentative, vie, attaque, defense, etage, piece, room, max_vie);
    fclose(file);

    printf("Données sauvegardées : Tentative: %d, Vie: %d, Attaque: %d, Defense: %d, Etage: %d, Piece: %d, Salle: %d\n",
           tentative, vie, attaque, defense, etage, piece, room);
}

// Fonction pour effacer les fichiers de sauvegarde de l'étage (après mort ou changement d'étage)
void clearMapDirectory(int saveNumber) {
    // Suppression du dossier de sauvegarde de l'étage
    char dirCommand[150];
    sprintf(dirCommand, "rm -rf ./data/game/save%d/map", saveNumber);
    system(dirCommand);
}

// Fonction pour charger les données du joueur depuis un fichier
void loadPlayerData(int saveNumber, int *tentative, int *vie, int *attaque, int *defense, int *etage, int *piece, int *room) {
    // Chargement des données du joueur depuis un fichier
    char playerFilename[100];
    sprintf(playerFilename, "./data/game/save%d/savePlayer.txt", saveNumber);

    // Ouverture du fichier en mode lecture
    FILE *file = fopen(playerFilename, "r");
    if (file) {
        fscanf(file, "%d %d %d %d %d %d %d", tentative, vie, attaque, defense, etage, piece, room);
        fclose(file); 
    } else {
        file = fopen(playerFilename, "w");
        fprintf(file, "%d %d %d %d %d %d %d", *tentative, *vie, *attaque, *defense, *etage, *piece, *room);
        fclose(file);
    }
}

// Fonction pour charger la map et la salle depuis un fichier
void loadMapAndRoom(int saveNumber, int room, int dataMap[11][11], int mapRoom[ROWS][COLS]) {
    char mapFilename[100];

    // Charger les données de la map
    sprintf(mapFilename, "./data/game/save%d/map/map.txt", saveNumber);
    loadMap(mapFilename, dataMap);

    // Charger les données de la salle
    if (room == 0) {
        sprintf(mapFilename, "./data/game/lobbyMap.txt");
    } else {
        sprintf(mapFilename, "./data/game/save%d/map/Salle%02d/salle%02d.txt", saveNumber, room, room);
    }
    loadRoomData(mapFilename, mapRoom);
}
