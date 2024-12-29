
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>

#define TILE_SIZE 32
#define ROWS 15
#define COLS 21

int loadMap(const char* filename, int data[11][11]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        return -1;
    }

    int i = 0, j = 0;
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

    printf("Données de la map chargées avec succès !\n");
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            printf("%4d ", data[i][j]);
        }
        printf("\n");
    }

    fclose(file);
    return (i * 11 + j); 
}

// Fonction pour créer une salle entière
void drawMap(int room[ROWS][COLS], SDL_Renderer *renderer) {
    SDL_Color colors[18] = {
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
        {192, 192, 192, 255}  // 18: pilier bas gauche (bg)
    };

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int tileId = room[i][j];  
            SDL_Color tileColor = colors[tileId - 1]; 

            SDL_Rect tileRect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_SetRenderDrawColor(renderer, tileColor.r, tileColor.g, tileColor.b, tileColor.a);
            SDL_RenderFillRect(renderer, &tileRect);
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

    int x, y, value;
    int i = 0, j = 0;
    
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


void saveGame(int saveNumber, int tentative, int vie, int attaque, int defense, int etage, int piece, int room, int max_vie) {
    char playerFilename[100];
    sprintf(playerFilename, "./data/game/save%d/savePlayer.txt", saveNumber);

    FILE *file = fopen(playerFilename, "w");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier %s pour la sauvegarde.\n", playerFilename);
        return;
    }

    room = (room == 100) ? 0 : room;

    fprintf(file, "%d %d %d %d %d %d %d %d", tentative, vie, attaque, defense, etage, piece, room, max_vie);
    fclose(file);

    printf("Données sauvegardées : Tentative: %d, Vie: %d, Attaque: %d, Defense: %d, Etage: %d, Piece: %d, Salle: %d\n",
           tentative, vie, attaque, defense, etage, piece, room);
}

// Fonction pour effacer les fichiers de sauvegarde de l'étage (après mort ou changement d'étage)
void clearMapDirectory(int saveNumber) {
    char dirCommand[150];
    sprintf(dirCommand, "rm -rf ./data/game/save%d/map", saveNumber);
    system(dirCommand);
}


void loadPlayerData(int saveNumber, int *tentative, int *vie, int *attaque, int *defense, int *etage, int *piece, int *room) {
    char playerFilename[100];
    sprintf(playerFilename, "./data/game/save%d/savePlayer.txt", saveNumber);

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


void loadMapAndRoom(int saveNumber, int room, int dataMap[11][11], int mapRoom[ROWS][COLS]) {
    char mapFilename[100];

    sprintf(mapFilename, "./data/game/save%d/map/map.txt", saveNumber);
    loadMap(mapFilename, dataMap);

    if (room == 0) {
        sprintf(mapFilename, "./data/game/lobbyMap.txt");
    } else {
        sprintf(mapFilename, "./data/game/save%d/map/Salle%02d/salle%02d.txt", saveNumber, room, room);
    }
    loadRoomData(mapFilename, mapRoom);
}
