#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h> 
#include "./../../include/game/game.h"
#include "./../../include/game/map.h"
#include "./../../include/game/pause.h"
#include "./../../include/core/player.h"
#include "./../../include/core/ennemies.h"


#define TILE_SIZE 32
#define ROWS 15
#define COLS 21
#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)

// Fonction pour charger les données de la map de l'étage depuis un fichier
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

// Fonction pour vérifier la collision du joueur avec les murs et les ennemis
bool checkCollision(Player *player, int mapRoom[ROWS][COLS], Enemy enemies[], int enemyCount) {
    int tileX = player->rect.x / TILE_SIZE;
    int tileY = player->rect.y / TILE_SIZE;

    if (tileX >= 0 && tileX < COLS && tileY >= 0 && tileY < ROWS) {
        int tileId = mapRoom[tileY][tileX];
        if (tileId == 5 || tileId == 6 || tileId == 7 || tileId == 11 || 
            tileId == 12 || tileId == 13 || tileId == 14 || 
            tileId == 15 || tileId == 16 || tileId == 17 || tileId == 18) {
            return true;
        }
    }

    for (int i = 0; i < enemyCount; i++) {
        if (SDL_HasIntersection(&player->rect, &enemies[i].rect)) {
            player->vie -= 10; 
            printf("Collision avec un ennemi ! PV restants: %d\n", player->vie);
            return true; 
        }
    }

    printf("%d\n", player->vie);

    return false;
}

// Fonction pour gérer la transition de porte (redirection vers la bonne salle etc.)
bool handleDoorTransition(Player *player, int mapRoom[ROWS][COLS], int data[11][11], int *currentRoom, int saveNumber) {
    int tileX = player->rect.x / TILE_SIZE;
    int tileY = player->rect.y / TILE_SIZE;

    if (tileX >= 0 && tileX < COLS && tileY >= 0 && tileY < ROWS) {
        int tileId = mapRoom[tileY][tileX];

        if (tileId >= 8 && tileId <= 10) { 
            int nextRoom = -1;
            int currentRoomX = -1, currentRoomY = -1;

            for (int i = 0; i < 11; i++) {
                for (int j = 0; j < 11; j++) {
                    if (data[i][j] == *currentRoom) {
                        currentRoomX = j;
                        currentRoomY = i;
                        break;
                    }
                }
                if (currentRoomX != -1) break;
            }

            if (currentRoomX == -1 || currentRoomY == -1) {
                printf("Erreur : salle actuelle (%d) introuvable dans la carte de l'étage.\n", *currentRoom);
                return false;
            }

            if (tileId == 8) { 
                if (tileY == 0 && currentRoomY > 1) { 
                    nextRoom = data[currentRoomY - 2][currentRoomX];
                    printf("haut\n");
                } else if (tileY == ROWS - 1 && currentRoomY < 9) { 
                    nextRoom = data[currentRoomY + 2][currentRoomX];
                    printf("bas\n");
                }
            } else if (tileId == 9) { 
                if (tileX == 0 && currentRoomX > 1) { 
                    nextRoom = data[currentRoomY][currentRoomX - 2];
                    printf("gauche\n");
                }
            } else if (tileId == 10) {

                if (*currentRoom == 0) {
                    nextRoom = 1;
                    creerEtageEntier(1);
                } else if (tileX == COLS - 1 && currentRoomX < 9) { 
                    nextRoom = data[currentRoomY][currentRoomX + 2];
                    printf("droite\n");
                }
            }

            if (nextRoom > 0) { 
                *currentRoom = nextRoom;
                char roomFilename[100];
                sprintf(roomFilename, "./data/game/save%d/map/Salle%02d/salle%02d.txt", saveNumber, *currentRoom, *currentRoom);
                loadRoomData(roomFilename, mapRoom);

                if (tileId == 8) { 
                    player->rect.y = (tileY == 0) ? TILE_SIZE * (ROWS - 1) : 0;
                } else if (tileId == 9) { 
                    player->rect.x = TILE_SIZE * (COLS - 1);
                } else if (tileId == 10) {
                    player->rect.x = 0;
                }

                return true;
            } else {
                printf("Erreur : la salle suivante est invalide ou inexistante.\n");
            }
        }
    }

    return false;
}

// Fonction pour sauvegarder les données du joueur (dès qu'une porte est prise)
void saveGame(int saveNumber, int tentative, int vie, int attaque, int defense, int etage, int piece, int room) {
    char playerFilename[100];
    sprintf(playerFilename, "./data/game/save%d/savePlayer.txt", saveNumber);

    FILE *file = fopen(playerFilename, "w");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier %s pour la sauvegarde.\n", playerFilename);
        return;
    }

    fprintf(file, "%d %d %d %d %d %d %d", tentative, vie, attaque, defense, etage, piece, room);
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

// Fonction pour gérer la fin de partie (mort du joueur), temporaire visuellement
void gameOver(Player *player, SDL_Renderer *renderer, int saveNumber, int *room, int *etage,int mapRoom[ROWS][COLS]) {
    if (player->vie <= 0) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
        
        SDL_Delay(2000); 

        player->vie = 100;

        clearMapDirectory(saveNumber);
        
        *room = 0; 
        *etage = 1; 
        clearEnemies();
        char roomFilename[100] = "./data/game/lobbyMap.txt";
        loadRoomData(roomFilename, mapRoom);
    }
}

// Fonction pour gérer la partie dans son intégralité
void allGame(int saveNumber, SDL_Renderer *renderer) {
    char mapFilename[100];
    int dataMap[11][11];

    sprintf(mapFilename, "./data/game/save%d/map/map.txt", saveNumber);
    loadMap(mapFilename, dataMap);

    int tentative = 0, vie = 100, attaque = 15, defense = 15, etage = 1, piece = 0, room = 1;

    char playerFilename[100];
    sprintf(playerFilename, "./data/game/save%d/savePlayer.txt", saveNumber);

    FILE *file = fopen(playerFilename, "r");
    if (file) {
        printf("Le fichier %s existe, on va le charger.\n", playerFilename);

        fscanf(file, "%d %d %d %d %d %d %d", &tentative, &vie, &attaque, &defense, &etage, &piece, &room);
        
        printf("Données lues du fichier: Tentative: %d, Vie: %d, Attaque: %d, Defense: %d, Etage: %d, Piece: %d, Salle : %d\n",
               tentative, vie, attaque, defense, etage, piece, room);

        fclose(file); 
    } else {
        printf("Le fichier %s n'existe pas, on va le créer.\n", playerFilename);

        file = fopen(playerFilename, "w");
        if (!file) {
            printf("Erreur lors de la création du fichier %s\n", playerFilename);
            return;
        }

        fprintf(file, "%d %d %d %d %d %d %d", tentative, vie, attaque, defense, etage, piece, room);
        fclose(file); 

        printf("Données par défaut écrites dans le fichier: Tentative: %d, Vie: %d, Attaque: %d, Defense: %d, Etage: %d, Piece: %d, Salle : %d\n",
               tentative, vie, attaque, defense, etage, piece, room);
    }

    int mapRoom[ROWS][COLS];
    
    if (room == 0) {
        sprintf(mapFilename, "./data/game/lobbyMap.txt");
    } else {
        sprintf(mapFilename, "./data/game/save%d/map/Salle%02d/salle%02d.txt", saveNumber, room, room);
    }

    loadRoomData(mapFilename, mapRoom);

    Player player;
    initPlayer(&player, TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE, vie);

    char enemyFilename[100];
    sprintf(enemyFilename, "./data/game/save%d/map/Salle%02d/mobs%02d.txt", saveNumber, room, room);
    initEnemies(enemyFilename);

    SDL_Event event;
    bool running = true;
    bool gamePaused = false;

    Uint32 frameStart;
    int frameTime;

    while (running) {

        frameStart = SDL_GetTicks();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
    SDL_RenderClear(renderer); 

    if (gamePaused) {
        SDL_Event pauseEvent;
        bool pauseRunning = true;
        SDL_Rect resumeButton = {200, 200, 400, 100};
        SDL_Rect quitButton = {200, 350, 400, 100};
        
        drawPauseMenu(renderer); 

        while (pauseRunning) {
            while (SDL_PollEvent(&pauseEvent)) {
                if (pauseEvent.type == SDL_QUIT) {
                    running = false;
                    pauseRunning = false;
                }

                if (handleButtonClick(&pauseEvent, resumeButton, quitButton, &gamePaused, &running)) {
                    pauseRunning = false;
                }
            }
        }
        continue;
    }

    gameOver(&player, renderer, saveNumber, &room, &etage, mapRoom);
    drawMap(mapRoom, renderer);
    drawEnemies(renderer);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            int oldX = player.rect.x, oldY = player.rect.y;

            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    player.rect.y -= TILE_SIZE;
                    player.orientation = 'U';
                    break;
                case SDLK_DOWN:
                    player.rect.y += TILE_SIZE;
                    player.orientation = 'D';
                    break;
                case SDLK_LEFT:
                    player.rect.x -= TILE_SIZE;
                    player.orientation = 'L';
                    break;
                case SDLK_RIGHT:
                    player.rect.x += TILE_SIZE;
                    player.orientation = 'R';
                    break;
                case SDLK_a:
                    attackWithSword(renderer, &player, player.orientation, enemies, &enemyCount, enemyFilename);  
                    break;
                case SDLK_e:
                    regeneratePlayer(&player);  
                    break;
                case SDLK_SPACE:
                    useLightning(renderer, &player);  
                    break;
                case SDLK_b:
                    player.vie -= 100;
                    break;
                case SDLK_ESCAPE:
                    handlePauseMenu(&event, &gamePaused);
                    break;
            }

            if (checkCollision(&player, mapRoom, enemies, enemyCount)) {
                player.rect.x = oldX;
                player.rect.y = oldY;
            } else if (handleDoorTransition(&player, mapRoom, dataMap, &room, saveNumber)) {
                saveGame(saveNumber, tentative, vie, attaque, defense, etage, piece, room);
                clearEnemies();
                sprintf(enemyFilename, "./data/game/save%d/map/Salle%02d/mobs%02d.txt", saveNumber, room, room);
                initEnemies(enemyFilename);
            }
        }
    }

    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    SDL_RenderFillRect(renderer, &player.rect);

    SDL_RenderPresent(renderer); 

    frameTime = SDL_GetTicks() - frameStart;

        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
}
}

// Fonction pour initialiser une partie (création de la map, génération des ennemis, etc.)
void initGame(int save_number, SDL_Renderer *renderer) {
    set_save_path(save_number);

    supprimerEtage();
    creerEtageEntier(1);
    printf("Génération de l'étage %hhd : OK\n", 2);
    printf("La seed est %d\n", lireSeed());

    printf("La génération de la map entière est un succès !\n");

    allGame(save_number, renderer);
}

// Fonction pour vérifier si une sauvegarde existe
int check_save_exists(const char *save_path) {
    if (access(save_path, F_OK) != -1) {
        return 1;  
    } else {
        return 0;  
    }
}

// Fonction pour gérer les clics de souris sur les boutons
int is_mouse_in_rect(int mouse_x, int mouse_y, SDL_Rect rect) {
    return (mouse_x >= rect.x && mouse_x <= (rect.x + rect.w) &&
            mouse_y >= rect.y && mouse_y <= (rect.y + rect.h));
}

// Fonction pour dessiner du texte à l'écran
void draw_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y) {
    SDL_Color color = {255, 255, 255, 255};  

    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (surface == NULL) {
        fprintf(stderr, "Erreur de création de surface pour le texte : %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        fprintf(stderr, "Erreur de création de texture pour le texte : %s\n", SDL_GetError());
        SDL_FreeSurface(surface);  
        return;
    }

    SDL_Rect dest = {x, y, surface->w, surface->h};  
    SDL_RenderCopy(renderer, texture, NULL, &dest);  

    SDL_FreeSurface(surface);  
    SDL_DestroyTexture(texture);
}

// Fonction pour démarrer le mode de jeu
void start_game_mode(SDL_Renderer *renderer) {
    if (TTF_Init() == -1) {
        fprintf(stderr, "Échec de l'initialisation de SDL_ttf : %s\n", TTF_GetError());
        return;
    }

    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 24);
    if (!font) {
        fprintf(stderr, "Échec du chargement de la police : %s\n", TTF_GetError());
        return;
    }

    int running = 1;
    SDL_Event event;

    int save1_exists = check_save_exists("./data/game/save1");
    int save2_exists = check_save_exists("./data/game/save2");
    int save3_exists = check_save_exists("./data/game/save3");

    int screen_width = 672;
    int screen_height = 544;

    SDL_Rect nouvelle_partie_rect = {screen_width - 220, 50, 200, 30};
    SDL_Rect quitter_rect = {screen_width - 220, screen_height - 130, 200, 30};
    SDL_Rect save1_rect = {screen_width / 2 - 120, screen_height / 2 - 110, 240, 30};
    SDL_Rect save2_rect = {screen_width / 2 - 120, screen_height / 2 - 10, 240, 30};
    SDL_Rect save3_rect = {screen_width / 2 - 120, screen_height / 2 + 90, 240, 30};

    while (running) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer);

        draw_text(renderer, font, "Nouvelle Partie", nouvelle_partie_rect.x, nouvelle_partie_rect.y);
        draw_text(renderer, font, "Quitter", quitter_rect.x, quitter_rect.y);
        draw_text(renderer, font, save1_exists ? "Charger Partie 1" : "Aucune Partie 1", save1_rect.x, save1_rect.y);
        draw_text(renderer, font, save2_exists ? "Charger Partie 2" : "Aucune Partie 2", save2_rect.x, save2_rect.y);
        draw_text(renderer, font, save3_exists ? "Charger Partie 3" : "Aucune Partie 3", save3_rect.x, save3_rect.y);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                if (is_mouse_in_rect(mouse_x, mouse_y, nouvelle_partie_rect)) {
                    printf("Nouvelle Partie\n");

                    if (!save1_exists) {
                        initGame(1, renderer);
                    } else if (!save2_exists) {
                        initGame(2, renderer);
                    } else if (!save3_exists) {
                        initGame(3, renderer);
                    } else {
                        printf("Aucune sauvegarde disponible, veuillez supprimer une sauvegarde existante.\n");
                    }
                } else if (is_mouse_in_rect(mouse_x, mouse_y, quitter_rect)) {
                    printf("Quitter\n");
                    running = 0;
                } else if (is_mouse_in_rect(mouse_x, mouse_y, save1_rect)) {
                    if (save1_exists) {
                        allGame(1, renderer);
                    } else {
                        printf("Aucune Partie 1 disponible\n");
                    }
                } else if (is_mouse_in_rect(mouse_x, mouse_y, save2_rect)) {
                    if (save2_exists) {
                        allGame(2, renderer);
                    } else {
                        printf("Aucune Partie 2 disponible\n");
                    }
                } else if (is_mouse_in_rect(mouse_x, mouse_y, save3_rect)) {
                    if (save3_exists) {
                        allGame(3, renderer);
                    } else {
                        printf("Aucune Partie 3 disponible\n");
                    }
                }
            }
        }

        SDL_Delay(100);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    printf("Retour au menu principal...\n");

    TTF_CloseFont(font);
    TTF_Quit();
}
