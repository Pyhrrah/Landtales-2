#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include "./../../include/game/game.h"
#include "./../../include/game/map.h"
#include "./../../include/game/pause.h"
#include "./../../include/game/hud.h"
#include "./../../include/game/game_over.h"
#include "./../../include/game/file.h"
#include "./../../include/core/player.h"
#include "./../../include/core/ennemies.h"
#include "./../../include/game/credit.h"
#include "./../../include/game/boss.h"
#include "./../../include/game/chest.h"
#include "./../../include/plugins/open_plugin_bonus.h"
#include "./../../include/utils/video.h"
#include "./../../include/utils/sdl_utils.h"

#define initialiserEtage() creerEtageEntier(1);
#define suppressionEtage() supprimerEtage();

#define TILE_SIZE 32
#define ROWS 15
#define COLS 21
#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)


#ifdef _WIN32
    #define PLUGIN_BONUS "./plugins/plugin_bonus.dll"
#else
    #define PLUGIN_BONUS "./plugins/plugin_bonus.so"
#endif

// Fonction pour vérifier la collision du joueur avec les murs et les ennemis
int checkCollision(Player *player, int mapRoom[ROWS][COLS], Enemy enemies[], int enemyCount) {
    int leftX   = player->rect.x;
    int rightX  = player->rect.x + player->rect.w - 1;
    int topY    = player->rect.y;
    int bottomY = player->rect.y + player->rect.h - 1;

    int leftTile   = leftX / TILE_SIZE;
    int rightTile  = rightX / TILE_SIZE;
    int topTile    = topY / TILE_SIZE;
    int bottomTile = bottomY / TILE_SIZE;

    for (int y = topTile; y <= bottomTile; y++) {
        for (int x = leftTile; x <= rightTile; x++) {
            if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
                int tileId = mapRoom[y][x];

                if (tileId == 5 || tileId == 6 || tileId == 7 || tileId == 11 || 
                    tileId == 12 || tileId == 13 || tileId == 14 || 
                    tileId == 15 || tileId == 16 || tileId == 17 || 
                    tileId == 18 || tileId == 19) {
                    return 1;  
                }
            }
        }
    }

    for (int i = 0; i < enemyCount; i++) {
        if (SDL_HasIntersection(&player->rect, &enemies[i].rect)) {
            player->vie -= 10; 
            printf("Collision avec un ennemi ! PV restants: %d\n", player->vie);
            return 1; 
        }
    }

    return 0;  
}

// Fonction pour gérer la transition de porte (redirection vers la bonne salle etc.)
int handleDoorTransition(Player *player, int mapRoom[ROWS][COLS], int data[11][11], int *currentRoom, int saveNumber, int *etage) {
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
                return 0;
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
                    if (*etage == 4) {
                        nextRoom = 100;
                    } else {
                        nextRoom = 1;
                    }
                } else if (tileX == COLS - 1 && currentRoomX < 9) { 
                    nextRoom = data[currentRoomY][currentRoomX + 2];
                    printf("droite\n");
                }
            }

            if (nextRoom > 0 && nextRoom < 100) { 
                *currentRoom = nextRoom;
                char roomFilename[100];
                sprintf(roomFilename, "./data/game/save%d/map/Salle%02d/salle%02d.txt", saveNumber, *currentRoom, *currentRoom);
                loadRoomData(roomFilename, mapRoom);

                if (tileId == 8) { 
                    player->rect.y = (tileY == 0) ? TILE_SIZE * (ROWS - 1) : 0;
                } else if (tileId == 9) { 
                    player->rect.x = TILE_SIZE * (COLS - 1);
                } else if (tileId == 10) {
                    player->rect.x = 32;
                }

                return 1; 
            } else  if (nextRoom == 100) {

                printf("Le boss est mort, passage au boss final.\n");                
                *currentRoom = nextRoom;
                char bigBossFilename[100] = "./data/game/bigBossMap.txt";
                loadRoomData(bigBossFilename, mapRoom);

                player->rect.x = 32;
                player->rect.y = 224;

                return 1; 

            } else{
                printf("Erreur : la salle suivante est invalide ou inexistante.\n");
                printf("Salle actuelle : %d, Salle suivante : %d\n", *currentRoom, nextRoom);
            }
        }
    }

    return 0; 
}


int checkCollisionTable(Player *player, SDL_Rect *attackTile, SDL_Rect *defenseTile, SDL_Rect *maxHealthTile) {
    if ((player->orientation == 'R' && player->rect.x == attackTile->x - 32 && player->rect.y == attackTile->y) || 
        (player->orientation == 'U' && player->rect.y == attackTile->y + 32 && player->rect.x == attackTile->x)) { 
        return 1; 
    }
    
    if (player->orientation == 'U' && player->rect.y == defenseTile->y + 32 && player->rect.x == defenseTile->x) {
        return 2; 
    }

    if ((player->orientation == 'L' && player->rect.x == maxHealthTile->x + 32 && player->rect.y == maxHealthTile->y) || 
        (player->orientation == 'U' && player->rect.y == maxHealthTile->y + 32 && player->rect.x == maxHealthTile->x)) { 
        return 3; 
    }

    return 0; 
}


void renderLobby(SDL_Renderer *renderer, Player *player, int *attackBought, int *defenseBought, int *maxHealthBought) {
    const int tableWidth = 3 * 32;
    const int tableHeight = 32;

    SDL_Rect table = {(COLS * TILE_SIZE - tableWidth) / 2, TILE_SIZE, tableWidth, tableHeight};

    const int tileSize = 32;
    const int itemWidth = tileSize;  
    const int itemHeight = tileSize;

    SDL_Rect attackTile = {table.x + 0 * tileSize, table.y, tileSize, tileSize};
    SDL_Rect defenseTile = {table.x + 1 * tileSize, table.y, tileSize, tileSize};
    SDL_Rect maxHealthTile = {table.x + 2 * tileSize, table.y, tileSize, tileSize};

    SDL_Rect attackItem = {attackTile.x + (tileSize - itemWidth) / 2, attackTile.y + (tileSize - itemHeight) / 2, itemWidth, itemHeight};
    SDL_Rect defenseItem = {defenseTile.x + (tileSize - itemWidth) / 2, defenseTile.y + (tileSize - itemHeight) / 2, itemWidth, itemHeight};
    SDL_Rect maxHealthItem = {maxHealthTile.x + (tileSize - itemWidth) / 2, maxHealthTile.y + (tileSize - itemHeight) / 2, itemWidth, itemHeight};

    const int price = 10;

    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    SDL_RenderFillRect(renderer, &table);

    SDL_Texture *attackTexture = NULL;
    SDL_Texture *defenseTexture = NULL;
    SDL_Texture *maxHealthTexture = NULL;
    
    if (*attackBought == 0) {
        attackTexture = IMG_LoadTexture(renderer, "./assets/images/sprite/shop/shop2.png");  
    }
    if (*defenseBought == 0) {
        defenseTexture = IMG_LoadTexture(renderer, "./assets/images/sprite/shop/shop3.png"); 
    }
    if (*maxHealthBought == 0) {
        maxHealthTexture = IMG_LoadTexture(renderer, "./assets/images/sprite/shop/shop1.png");  
    }

    if (attackTexture) {
        SDL_RenderCopy(renderer, attackTexture, NULL, &attackItem);
        SDL_DestroyTexture(attackTexture);
    }
    if (defenseTexture) {
        SDL_RenderCopy(renderer, defenseTexture, NULL, &defenseItem);
        SDL_DestroyTexture(defenseTexture);
    }
    if (maxHealthTexture) {
        SDL_RenderCopy(renderer, maxHealthTexture, NULL, &maxHealthItem);
        SDL_DestroyTexture(maxHealthTexture);
    }

    SDL_Color color;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect textRect;

    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 16);
    if (!font) {
        printf("Erreur chargement police: %s\n", TTF_GetError());
        return;
    }

    for (int i = 0; i < 3; i++) {
        SDL_Rect *currentItem = (i == 0) ? &attackItem : (i == 1) ? &defenseItem : &maxHealthItem;
        int bought = (i == 0) ? *attackBought : (i == 1) ? *defenseBought : *maxHealthBought;

        if (bought == 0) {
            color = (player->argent >= price) ? (SDL_Color){255, 255, 255, 255} : (SDL_Color){255, 0, 0, 255};
            char priceText[10];
            sprintf(priceText, "%d", price);

            surface = TTF_RenderUTF8_Solid(font, priceText, color);
            if (surface) {
                texture = SDL_CreateTextureFromSurface(renderer, surface);
                textRect = (SDL_Rect){currentItem->x + (itemWidth - surface->w) / 2, currentItem->y - 20, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, NULL, &textRect);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }
    }

    TTF_CloseFont(font);

}




void handleLobbyInteraction(Player *player, int *attackBought, int *defenseBought, int *maxHealthBought, int *alreadyBoughtInSession) {

    const int tableX = (COLS * TILE_SIZE - 3 * 32) / 2;  
    const int tableY = TILE_SIZE;                       

    SDL_Rect attackTile = {tableX + 0 * 32, tableY, 32, 32};
    SDL_Rect defenseTile = {tableX + 1 * 32, tableY, 32, 32};
    SDL_Rect maxHealthTile = {tableX + 2 * 32, tableY, 32, 32};
    int bought = 0;

    if (*alreadyBoughtInSession == 0 && player->argent >= 10) {
        int collision = checkCollisionTable(player, &attackTile, &defenseTile, &maxHealthTile);

        if (collision == 1 && *attackBought == 0) {
            player->argent -= 10;
            player->attaque += 10;
            *attackBought = 1;
            *alreadyBoughtInSession = 1;
            bought = 1;
            printf("attaque achetée\n");
        } else if (collision == 2 && *defenseBought == 0) {
            player->argent -= 10;
            player->defense += 10;
            *defenseBought = 1;
            *alreadyBoughtInSession = 1;
            bought = 1;
            printf("défense achetée\n");
        } else if (collision == 3 && *maxHealthBought == 0) {
            player->argent -= 10;
            player->max_vie += 10;
            *maxHealthBought = 1;
            *alreadyBoughtInSession = 1;
            bought = 1;
            printf("max vie achetée\n");
        }
    }

    if (bought) {
        playSong("./assets/music/sons/shop.mp3");
    }
}


void restoreArticles(int *attackBought, int *defenseBought, int *maxHealthBought, int *alreadyBoughtInSession, int room) {
    if (*alreadyBoughtInSession == 1 && room != 0) {
        *attackBought = 0;
        *defenseBought = 0;
        *maxHealthBought = 0;
        *alreadyBoughtInSession = 0;
    }
}


void allGame(int saveNumber, SDL_Renderer *renderer) {

    init_audio();

    loadPlugin(PLUGIN_BONUS);
    char mapFilename[100] = "";
    char stageFilename[100] = "";
    int dataMap[11][11];

    int tentative = 0 ,vie = 100, attaque = 15, defense = 15, etage = 1, piece = 0, room = 1, max_vie = 100;

    char playerFilename[100];
    sprintf(playerFilename, "./data/game/save%d/savePlayer.txt", saveNumber);

    int attackBought = 0;
    int defenseBought = 0;
    int maxHealthBought = 0;
    int alreadyBoughtInSession = 0;


    FILE *file = fopen(playerFilename, "r");
    if (file) {
        printf("Le fichier %s existe, on va le charger.\n", playerFilename);

        fscanf(file, "%d %d %d %d %d %d %d %d", &tentative, &vie, &attaque, &defense, &etage, &piece, &room, &max_vie);
        
        printf("Données lues du fichier: Tentative: %d, Vie: %d, Attaque: %d, Defense: %d, Etage: %d, Piece: %d, Salle : %d, Max Vie : %d\n",
               tentative, vie, attaque, defense, etage, piece, room, max_vie);

        fclose(file); 
    } else {
        printf("Le fichier %s n'existe pas, on va le créer.\n", playerFilename);

        file = fopen(playerFilename, "w");
        if (!file) {
            printf("Erreur lors de la création du fichier %s\n", playerFilename);
            return;
        }

        fprintf(file, "%d %d %d %d %d %d %d %d", tentative, vie, attaque, defense, etage, piece, room, max_vie);
        fclose(file); 

        printf("Données par défaut écrites dans le fichier: Tentative: %d, Vie: %d, Attaque: %d, Defense: %d, Etage: %d, Piece: %d, Salle : %d, Max vie : %d\n",
               tentative, vie, attaque, defense, etage, piece, room, max_vie);
    }


    int mapRoom[ROWS][COLS];
    
    if (room == 0) {
        sprintf(mapFilename, "./data/game/lobbyMap.txt");

        


    } else {
        sprintf(mapFilename, "./data/game/save%d/map/Salle%02d/salle%02d.txt", saveNumber, room, room);
    }


    loadRoomData(mapFilename, mapRoom);

    SDL_Texture *projectileTexture = load_texture(renderer, "./assets/images/sprite/projectile.png");
    if (!projectileTexture) {
        printf("Impossible de charger la texture du projectile\n");
    }


// Appel à initPlayer           
    Player  player;
    printf("Taille de player : %ld\n", sizeof(player));
    initPlayer(&player, TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE, vie, piece, attaque, defense, max_vie, renderer);

    sprintf(stageFilename, "./data/game/save%d/map/map.txt", saveNumber);
    loadMap(stageFilename, dataMap);

    char enemyFilename[100];
    sprintf(enemyFilename, "./data/game/save%d/map/Salle%02d/mobs%02d.txt", saveNumber, room, room);
    initEnemies(enemyFilename);
    initProjectilesBoss(); // Initialisation des projectiles pour le boss final


    if (room == 0 || room == 1){
        player.rect.x = 320;
        player.rect.y = 224;
    }


    SDL_Event event;
    int running = 1;
    int gamePaused = 0;

    Uint32 frameStart;
    int frameTime;


    while (running) {


        frameStart = SDL_GetTicks();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer); 

        if (room == 2) {
            checkAndActivateBossDoor(room, enemyCount);
            }

        if (room == 100){
            if(!isBossAlive(enemyCount, 100)){

                saveWinGame(saveNumber, tentative, player.vie, player.attaque, player.defense, etage, player.argent, room, player.max_vie);

                clearMapDirectory(saveNumber);

                initialiserEtage();
                cleanup_audio();

                displayCredits(renderer);

                running = 0;
            }
        }
        
        

        if (gamePaused) {
            SDL_Event pauseEvent;
            int pauseRunning = 1;
            SDL_Rect resumeButton = {410, 200, 200, 60};
            SDL_Rect quitButton = {410, 300, 200, 60};
            
            drawPauseMenu(renderer, dataMap, room); 

            while (pauseRunning) {
                while (SDL_PollEvent(&pauseEvent)) {
                    if (pauseEvent.type == SDL_QUIT) {
                        clearEnemies();
                        running = 0;
                        pauseRunning = 0;
                        
                    }

                    if (handleButtonClick(&pauseEvent, resumeButton, quitButton, &gamePaused, &running)) {
                        pauseRunning = 0;
                    }
                }
            }
            continue;
        }

        gameOver(&player, renderer, saveNumber, &room, &etage, mapRoom, &tentative, dataMap, stageFilename, &event, &running);
        if (!running) break;
        drawMap(mapRoom, renderer);

        if (room != 0) {
            drawEnemies(renderer);
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                int oldX = player.rect.x, oldY = player.rect.y;
                switch (event.key.keysym.sym) {
                    case SDLK_z:
                        player.rect.y -= TILE_SIZE;
                        player.orientation = 'U';
                        player.frame = (player.frame + 1) % 3;
                        break; 
                    case SDLK_s:
                        player.rect.y += TILE_SIZE;
                        player.orientation = 'D';
                        player.frame = (player.frame + 1) % 3;
                        break;
                    case SDLK_q:
                        player.rect.x -= TILE_SIZE;
                        player.orientation = 'L';
                        player.frame = (player.frame + 1) % 3;
                        break;
                    case SDLK_d:
                        player.rect.x += TILE_SIZE;
                        player.orientation = 'R';
                        player.frame = (player.frame + 1) % 3;
                        break;
                    case SDLK_e:
                        regeneratePlayer(&player);  
                        break;
                    case SDLK_SPACE:
                        useLightning(&player, &enemyCount, enemyFilename);  
                        break;
                    case SDLK_b:
                        player.vie -= 100;
                        break;
                    case SDLK_ESCAPE:
                        if (!gamePaused) {
                            gamePaused = 1;  // Passe en mode pause si le jeu n'est pas déjà en pause
                        } else {
                            gamePaused = 0;  // Reprend le jeu si déjà en pause
                        }
                        break;
                    case SDLK_a:
                        if (room == 0) { 
                            handleLobbyInteraction(&player, &attackBought, &defenseBought, &maxHealthBought, &alreadyBoughtInSession);
                        } else {
                            if (checkChestCollision(&player, mapRoom, saveNumber, room)) {
                                openChestWithPlugin(&player);
                                playSong("./assets/music/sons/coffre.mp3");
                            }
                        }
                        break;
                }

                if (checkCollision(&player, mapRoom, enemies, enemyCount)) {
                    player.rect.x = oldX;
                    player.rect.y = oldY;
                } else if (handleDoorTransition(&player, mapRoom, dataMap, &room, saveNumber, &etage)) {
                    saveGame(saveNumber, tentative, player.vie, player.attaque, player.defense, etage, player.argent, room, player.max_vie);
                    clearEnemies();

                        if (room < 100) {
                            sprintf(enemyFilename, "./data/game/save%d/map/Salle%02d/mobs%02d.txt", saveNumber, room, room);
                            initEnemies(enemyFilename);
                        } else if (room == 100) {
                            printf("Boss room\n");
                            initBigBoss();
                            play_audio("./assets/music/sons/Ici.mp3");
                        }
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
                        attackWithSword(renderer, &player, player.orientation, &enemyCount, enemyFilename);
                        break;
                    case SDL_BUTTON_RIGHT:
                        shootArrow(&player);
                        break;
                }
            }
        }        

        if (room == 0) {
            renderLobby(renderer, &player, &attackBought, &defenseBought, &maxHealthBought);
        } else {

            // Appeler UpdateEnnemies ici 
            restoreArticles(&attackBought, &defenseBought, &maxHealthBought, &alreadyBoughtInSession, room);

            if (checkPlayerBonusCollision(&player) == 1) {
                play_audio("./assets/music/sons/bonus.mp3");
            }
            renderBonuses(renderer);

            handleBossDoorCollision(renderer, &player.rect, &room, mapFilename, mapRoom, saveNumber, 
                            tentative, &etage, &player, dataMap, stageFilename);

            launchProjectileWithCooldownEnnemies(&player.rect);
            updateProjectiles(mapRoom);
            checkProjectileCollisions(&player);
            renderProjectiles(renderer, projectileTexture);

        }    

        if (room == 100){
            launchProjectileWithCooldownBoss(&enemies[0].rect, &player.rect);
            handleProjectilesBoss(renderer, mapRoom, &player, projectileTexture);
        }


        renderHUD(renderer, &player, tentative);
        updateArrows(&player, mapRoom, &enemyCount, enemyFilename);
        
        if (areArrowsActive(&player)) {
            updateArrows(&player, mapRoom, &enemyCount, enemyFilename);

            renderArrows(renderer, &player);
        }

        drawPlayerWithFrame(&player, renderer);
        updateAndRenderLightning(renderer);
        updateAndRenderRegenAnimation(renderer, &player);

        SDL_RenderPresent(renderer); 

        frameTime = SDL_GetTicks() - frameStart;

        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
    unloadPlugin();
    clearEnemies();
}

// Fonction pour initialiser une partie (création de la map, génération des ennemis, etc.)
void initGame(int save_number, SDL_Renderer *renderer) {
    set_save_path(save_number);

    suppressionEtage();
    initialiserEtage();
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

    SDL_Surface *surface = TTF_RenderUTF8_Solid(font, text, color);
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

    TTF_Font *font = TTF_OpenFont("./assets/fonts/font.ttf", 24);
    if (!font) {
        fprintf(stderr, "Échec du chargement de la police : %s\n", TTF_GetError());
        return;
    }

    // Charger l'image de fond
    SDL_Surface* backgroundSurface = IMG_Load("./assets/images/fondGame.png");
    if (!backgroundSurface) {
        fprintf(stderr, "Erreur de chargement de l'image de fond : %s\n", IMG_GetError());
        return;
    }
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);

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
        // Dessiner l'image de fond
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

        // Dessiner le texte
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

    // Nettoyer les ressources
    cleanup_audio();
    SDL_DestroyTexture(backgroundTexture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    printf("Retour au menu principal...\n");

    TTF_CloseFont(font);
    TTF_Quit();
}


/*
Quand tu as codé en une semaine le jeu de ta vie sans avoir rencontré le moindre problème (les problèmes viennent du code des autres).                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡠⠔⠂⠉⠉⠉⠉⠉⠀⠒⠠⠄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡠⠔⠊⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠒⠤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠔⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢆⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠃⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⠀⠤⠤⠒⣂⣈⣉⣁⣀⣄⠀⠀⠀⠀⠇⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠃⠀⠀⠀⢀⡠⠄⢒⣊⣉⣀⠤⠐⠒⠉⣁⡀⠤⠒⠒⠠⣄⠀⠀⠀⠀⢸⡀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡇⠠⠔⣂⡭⢅⣒⡈⠁⠠⠤⠒⠒⠈⠉⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡁⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠁⠀⠀⠀⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠖⠒⠂⠉⠉⠉⠉⠲⠀⠀⠀⠈⡇⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢨⡇⠀⠀⠀⠀⠀⣀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣇⠀⠀⠀⠀⠀⠀
No segmentation fault                         ⠀⠀⢇⠀⢀⡔⠊⠁⠀⠀⢀⣀⠱⠀⠀⢠⠀⣰⡯⢤⣶⣶⢲⠀⠀⠀⠀⠀⠀⠀⠻⠀⠀⠀⠀⠀⠀
dans le code                    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⡀⠀⠀⠀⡔⢲⣶⣶⠨⣭⡇⠀⢸⡄⠉⠃⠚⠛⠋⠉⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣇⠀⠀⠀⠑⠚⠻⠛⠒⠁⣧⠀⠈⢇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡆⠀⠀⠀⠀⠀
                                ⢀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠘⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠁⠀⠀⠀⠀⠀
                                ⠘⠦⣉⠒⠒⠲⢄⡀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⢠⠏⠀⠀⠀⠈⢣⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣞⠀⠀⠀⠀⠀⠀
                                ⠀⠀⢸⠁⠀⠀⠀⠈⠉⠑⠂⠠⠤⣀⣀⡀⢹⡀⠀⠀⠀⠀⠀⠀⠙⣄⢀⡤⠤⠤⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⢠⢲⠟⡆⠀⠀⠀⠀⠀
                                ⠀⢀⡞⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢧⡆⡀⠀⠀⠀⠀⠀⡈⢠⣤⡀⠀⡄⠀⠀⠀⠀⠀⠀⠀⢀⡖⣮⡿⣧⡘⠄⣀⠀⠀⠀
                                ⠀⠸⠦⠤⠢⢄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢳⡏⠀⠀⠀⢠⡞⢀⣾⣅⣁⣀⡁⠈⣦⡀⢠⡀⠀⢰⣟⣿⣿⢻⣃⠀⠀⠀⠈⠑⠲
                                ⠀⠀⠀⠀⠀⠀⠈⠐⢄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣦⡆⢠⣛⠀⣾⣿⣿⣿⣿⣿⣦⣀⠋⢸⣴⢴⡃⠿⣽⢯⠢⡽⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⠢⡀⠀⠀⠀⠀⠀⠀⠀⠀⠜⢷⣯⡿⣎⢿⣿⣿⣿⣿⣿⢟⡇⠀⠀⡿⠘⢙⡼⡿⠖⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠣⡀⠀⠀⠀⠀⠀⠀⠀⠼⠿⣇⡇⠈⢏⡛⠛⠛⡡⠊⠀⠀⠀⢠⣠⡯⣽⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠢⡀⠀⠀⠀⠀⠀⠀⠀⠹⣿⠆⡀⠀⠈⠁⠀⠀⠀⠀⢀⡴⡿⡁⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⡀⠀⠀⠀⠀⠀⠀⠀⠹⡷⢾⣀⡀⠀⢀⡆⣆⢀⣾⣹⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠢⣀⠀⠀⠀⠀⠀⠀⠁⠀⠉⢻⣞⠻⡇⡟⠻⡥⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠢⡀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢣⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢡⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
*/