#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./../../include/editor/colors.h"

#define PORT 12345
#define ROWS 15
#define COLS 21
#define CELL_SIZE 32
#define BUFFER_SIZE 1024 


/*

Fichier client.c, contient les fonctions pour le client du mode multijoueur

Important : TCPsocket client_socket correspond à la connexion avec le serveur. 
Exemple de valeur pour client_socket : TCPsocket client_socket = SDLNet_TCP_Open(&ip); soit l'adresse IP du serveur

 */


/**
 * Structure pour représenter un joueur
 * SDL_Rect rect : rectangle pour la position du joueur (x, y, w, h)
 * char orientation : orientation du joueur (U, D, R, L)
 */
typedef struct {
    SDL_Rect rect;
    char orientation;
} Player;

// Fonctions pour vérifier les collisions avec les murs
int check_collision(int grid[ROWS][COLS], int new_x, int new_y) {
    if (new_x < 0 || new_x >= COLS || new_y < 0 || new_y >= ROWS) {
        return 0;
    }
    // Tuiles bloquées (murs)
    int blocked_tiles[] = {5, 6, 7, 8, 9, 10, 11};
    int tile_value = grid[new_y][new_x];
    for (size_t i = 0; i < sizeof(blocked_tiles) / sizeof(blocked_tiles[0]); i++) {
        if (tile_value == blocked_tiles[i]) {
            return 0; 
        }
    }
    return 1; 
}

// Fonctions pour envoyer et recevoir la grille, initialiser les joueurs et les adversaires
// TCPsocket client_socket : socket du client, correspond à la connexion avec le serveur dans SDL_net
void receive_grid(TCPsocket client_socket, int grid[ROWS][COLS], int *client_id, char *orientation) {
    // memset permet de remplir la grille avec des 0
    memset(grid, 0, sizeof(int) * ROWS * COLS);
    // Réception de la grille
    size_t grid_size = ROWS * COLS * sizeof(int);
    size_t received = 0;
    // Tant que la taille reçue est inférieure à la taille de la grille, on continue de recevoir
    while (received < grid_size) {
        // SDLNet_TCP_Recv permet de recevoir des données sur le socket
        int bytes = SDLNet_TCP_Recv(client_socket, (char *)grid + received, grid_size - received);
        if (bytes <= 0) {
            fprintf(stderr, "Erreur de réception de la grille : %s\n", SDLNet_GetError());
            exit(EXIT_FAILURE);
        }
        received += bytes;
    }
    received = 0;
    size_t id_size = sizeof(int);
    // Tant que la taille reçue est inférieure à la taille de l'identifiant, on continue de recevoir
    while (received < id_size) {
        // Réception de l'identifiant du client
        int bytes = SDLNet_TCP_Recv(client_socket, (char *)client_id + received, id_size - received);
        if (bytes <= 0) {
            fprintf(stderr, "Erreur de réception de l'identifiant du client : %s\n", SDLNet_GetError());
            exit(EXIT_FAILURE);
        }
        received += bytes;
    }

    received = 0;
    size_t orientation_size = sizeof(char);
    while (received < orientation_size) {
        // Réception de l'orientation du joueur
        int bytes = SDLNet_TCP_Recv(client_socket, (char *)orientation + received, orientation_size - received);
        if (bytes <= 0) {
            fprintf(stderr, "Erreur de réception de l'orientation : %s\n", SDLNet_GetError());
            exit(EXIT_FAILURE);
        }
        received += bytes;
    }

    printf("Grille, identifiant client et orientation reçus avec succès.\n");
}

// Fonction pour dessiner la grille
void draw_grid(SDL_Renderer *renderer, int grid[ROWS][COLS]) {
    static int textures_loaded = 0;
    if (!textures_loaded) {
        load_textures(renderer);
        textures_loaded = 1;
    }

    // Parcours de la grille pour dessiner les tuiles
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            SDL_Rect rect = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            int value = grid[i][j];

            if (value >= 0 && value < 14) {
                SDL_Texture *current_texture = textures[value];

                if (current_texture != NULL) {
                    SDL_RenderCopy(renderer, current_texture, NULL, &rect);
                } else {
                    SDL_SetRenderDrawColor(renderer, colors[value].r, colors[value].g, colors[value].b, colors[value].a);
                    SDL_RenderFillRect(renderer, &rect);
                }
            } else {
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

// Fonction pour initialiser le joueur et l'adversaire
void initialize_player_opponent(int grid[ROWS][COLS], Player *player, Player *opponent, int client_id) {
    int spawn_tile_player = (client_id == 1) ? 12 : 13;
    int spawn_tile_opponent = (client_id == 1) ? 13 : 12;

    // Initialisation des positions des joueurs
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j] == spawn_tile_player) {
                player->rect = (SDL_Rect){j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                break;
            }
        }
    }

    // Initialisation des positions des adversaires
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j] == spawn_tile_opponent) {
                opponent->rect = (SDL_Rect){j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                break;
            }
        }
    }

    // Vérification des positions de spawn
    if (player->rect.x == -1 || player->rect.y == -1) {
        fprintf(stderr, "Erreur : Position de spawn introuvable pour le joueur %d.\n", client_id);
        exit(EXIT_FAILURE);
    }

    // Vérification des positions de spawn
    if (opponent->rect.x == -1 || opponent->rect.y == -1) {
        fprintf(stderr, "Erreur : Position de spawn introuvable pour l'adversaire.\n");
        exit(EXIT_FAILURE);
    }
}

// Fonction pour dessiner le joueur
void draw_player(SDL_Renderer *renderer, Player *player, const char *image_path) {
    // Charger l'image du sprite
    SDL_Texture *sprite_texture = IMG_LoadTexture(renderer, image_path);
    if (!sprite_texture) {
        fprintf(stderr, "Erreur de chargement du sprite : %s\n", SDL_GetError());
        return;
    }

    // Déterminer la position de la sprite en fonction de l'orientation
    SDL_Rect src_rect = {0, 0, 32, 32}; // La taille de chaque sprite est de 32x32

    switch (player->orientation) {
        case 'D': // Vers le bas (y=0)
            src_rect.y = 0;
            break;
        case 'L': // Vers la gauche (y=32)
            src_rect.y = 32;
            break;
        case 'R': // Vers la droite (y=64)
            src_rect.y = 64;
            break;
        case 'U': // Vers le haut (y=96)
            src_rect.y = 96;
            break;
        default:
            fprintf(stderr, "Orientation non valide : %c\n", player->orientation);
            return;
    }

    // Positionner le rectangle de destination où on veut afficher le sprite du joueur
    SDL_Rect dest_rect = player->rect;

    // Afficher le sprite du joueur avec la bonne orientation
    SDL_RenderCopy(renderer, sprite_texture, &src_rect, &dest_rect);

    // Libérer la texture après dessin
    SDL_DestroyTexture(sprite_texture);
}

// Fonction pour envoyer la position du joueur au serveur, TCPsocket client_socket : socket du client, Player *player : joueur
void send_player_position(TCPsocket client_socket, Player *player) {
    // Création du message à envoyer, BUFFER_SIZE est la taille du buffer, le buffer est un tableau de caractères
    char message[BUFFER_SIZE];
    // snprintf permet de formater une chaîne de caractères
    snprintf(message, sizeof(message), "MOVE %d %d %c\n", player->rect.x / CELL_SIZE, player->rect.y / CELL_SIZE, player->orientation);
    // SDLNet_TCP_Send permet d'envoyer des données sur le socket
    SDLNet_TCP_Send(client_socket, message, strlen(message));
}

// Fonction pour gérer les messages du serveur, TCPsocket client_socket : socket du client, Player *opponent : adversaire
void handle_server_messages(TCPsocket client_socket, Player *opponent) {
    char buffer[BUFFER_SIZE];

    // Initialisation du SocketSet
    // SDLNet_SocketSet est une structure qui contient un ensemble de sockets
    static SDLNet_SocketSet socket_set = NULL;
    if (!socket_set) {
        // SDLNet_AllocSocketSet permet d'allouer un SocketSet
        socket_set = SDLNet_AllocSocketSet(1);
        if (!socket_set) {
            fprintf(stderr, "Erreur lors de l'allocation du SocketSet : %s\n", SDLNet_GetError());
            return;
        }
        // SDLNet_TCP_AddSocket permet d'ajouter un socket à un SocketSet
        SDLNet_TCP_AddSocket(socket_set, client_socket);
    }

    // SDLNet_CheckSockets permet de vérifier si des données sont prêtes à être lues sur les sockets
    if (SDLNet_CheckSockets(socket_set, 0) > 0 && SDLNet_SocketReady(client_socket)) {
        // SDLNet_TCP_Recv permet de recevoir des données sur le socket
        int bytes_received = SDLNet_TCP_Recv(client_socket, buffer, sizeof(buffer) - 1);

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';

            int x, y;
            char orientation;
            if (sscanf(buffer, "MOVE %d %d %c", &x, &y, &orientation) == 3) {
                opponent->rect.x = x * CELL_SIZE;
                opponent->rect.y = y * CELL_SIZE;
                opponent->orientation = orientation;
            } else {
                fprintf(stderr, "Format de message inconnu : %s\n", buffer);
            }
        } else if (bytes_received <= 0) {
            if (bytes_received == 0) {
                printf("Le serveur a fermé la connexion.\n");
            } else {
                fprintf(stderr, "Erreur lors de la réception des données : %s\n", SDLNet_GetError());
            }
        }
    }
}

// Fonction pour démarrer le client, const char *server_ip : adresse IP du serveur, SDL_Renderer *renderer : renderer
void start_client(const char *server_ip, SDL_Renderer *renderer) {
    if (SDLNet_Init() == -1) {
        fprintf(stderr, "Erreur d'initialisation de SDL_net : %s\n", SDLNet_GetError());
        return;
    }

    if (!renderer) {
        fprintf(stderr, "Erreur : Le renderer n'a pas été correctement initialisé.\n");
        return;
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, server_ip, PORT) == -1) {
        fprintf(stderr, "Erreur de résolution de l'hôte : %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return;
    }

    TCPsocket client_socket = SDLNet_TCP_Open(&ip);
    if (!client_socket) {
        fprintf(stderr, "Erreur de connexion au serveur : %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return;
    }

    printf("Connecté au serveur.\n");

    int grid[ROWS][COLS] = {0}, client_id = 0;
    char orientation;
    receive_grid(client_socket, grid, &client_id, &orientation);

    Player player = {.orientation = orientation}, opponent = {.rect = {0, 0, CELL_SIZE, CELL_SIZE}, .orientation = 'U'};
    initialize_player_opponent(grid, &player, &opponent, client_id);

    SDL_Event event;
    int running = 1;

    Uint32 last_send_time = SDL_GetTicks();
    const Uint32 send_interval = 200;

    while (running) {
        handle_server_messages(client_socket, &opponent);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                int x = player.rect.x / CELL_SIZE;
                int y = player.rect.y / CELL_SIZE;
                int new_x = x, new_y = y;

                switch (event.key.keysym.sym) {
                    case SDLK_q: new_x--; player.orientation = 'L'; break;
                    case SDLK_d: new_x++; player.orientation = 'R'; break;
                    case SDLK_z: new_y--; player.orientation = 'U'; break;
                    case SDLK_s: new_y++; player.orientation = 'D'; break;
                }

                if (check_collision(grid, new_x, new_y)) {
                    player.rect.x = new_x * CELL_SIZE;
                    player.rect.y = new_y * CELL_SIZE;
                }
            }
        }

        if (SDL_GetTicks() - last_send_time >= send_interval) {
            send_player_position(client_socket, &player);
            last_send_time = SDL_GetTicks();
        }

        SDL_RenderClear(renderer);
        draw_grid(renderer, grid);
        draw_player(renderer, &player, "assets/images/sprite/player/1.png");
        draw_player(renderer, &opponent, "assets/images/sprite/player/2.png");
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDLNet_TCP_Close(client_socket);
    SDLNet_Quit();
}
