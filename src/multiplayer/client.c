#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./../../include/editor/colors.h"

#define PORT 12345
#define ROWS 15
#define COLS 21
#define CELL_SIZE 32
#define BUFFER_SIZE 1024 

typedef struct {
    SDL_Rect rect;
} Player;

// Vérifie les collisions avec des tuiles bloquantes
int check_collision(int grid[ROWS][COLS], int new_x, int new_y) {
    if (new_x < 0 || new_x >= COLS || new_y < 0 || new_y >= ROWS) {
        return 0;
    }
    int blocked_tiles[] = {5, 6, 7, 8, 9, 10, 11};
    int tile_value = grid[new_y][new_x];
    for (size_t i = 0; i < sizeof(blocked_tiles) / sizeof(blocked_tiles[0]); i++) {
        if (tile_value == blocked_tiles[i]) {
            return 0; 
        }
    }
    return 1; 
}

// Reçoit la grille et l'identifiant du joueur
void receive_grid(TCPsocket client_socket, int grid[ROWS][COLS], int *client_id) {
    memset(grid, 0, sizeof(int) * ROWS * COLS);

    // Réception de la grille
    size_t grid_size = ROWS * COLS * sizeof(int);
    size_t received = 0;
    while (received < grid_size) {
        int bytes = SDLNet_TCP_Recv(client_socket, (char *)grid + received, grid_size - received);
        if (bytes <= 0) {
            fprintf(stderr, "Erreur de réception de la grille : %s\n", SDLNet_GetError());
            exit(EXIT_FAILURE);
        }
        received += bytes;
    }

    // Réception de l'identifiant du client
    received = 0;
    size_t id_size = sizeof(int);
    while (received < id_size) {
        int bytes = SDLNet_TCP_Recv(client_socket, (char *)client_id + received, id_size - received);
        if (bytes <= 0) {
            fprintf(stderr, "Erreur de réception de l'identifiant du client : %s\n", SDLNet_GetError());
            exit(EXIT_FAILURE);
        }
        received += bytes;
    }

    printf("Grille et identifiant client reçus avec succès.\n");
}


// Dessine la grille
void draw_grid(SDL_Renderer *renderer, int grid[ROWS][COLS]) {
    static int textures_loaded = 0; // Charger les textures une seule fois
    if (!textures_loaded) {
        load_textures(renderer);
        textures_loaded = 1;
    }

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


// Initialise la position du joueur
void initialize_player_opponent(int grid[ROWS][COLS], Player *player, Player *opponent, int client_id) {
    int spawn_tile_player = (client_id == 1) ? 12 : 13;
    int spawn_tile_opponent = (client_id == 1) ? 13 : 12;

    // Initialiser la position du joueur
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j] == spawn_tile_player) {
                player->rect = (SDL_Rect){j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                break;
            }
        }
    }

    // Initialiser la position de l'adversaire
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j] == spawn_tile_opponent) {
                opponent->rect = (SDL_Rect){j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                break;
            }
        }
    }

    // Vérification des erreurs
    if (player->rect.x == -1 || player->rect.y == -1) {
        fprintf(stderr, "Erreur : Position de spawn introuvable pour le joueur %d.\n", client_id);
        exit(EXIT_FAILURE);
    }

    if (opponent->rect.x == -1 || opponent->rect.y == -1) {
        fprintf(stderr, "Erreur : Position de spawn introuvable pour l'adversaire.\n");
        exit(EXIT_FAILURE);
    }
}


// Dessine un joueur
void draw_player(SDL_Renderer *renderer, Player *player, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &player->rect);
}

// Envoie la position du joueur au serveur
void send_player_position(TCPsocket client_socket, Player *player) {
    char message[BUFFER_SIZE];
    snprintf(message, sizeof(message), "MOVE %d %d\n", player->rect.x / CELL_SIZE, player->rect.y / CELL_SIZE);
    SDLNet_TCP_Send(client_socket, message, strlen(message));
}

// Gère les messages du serveur
void handle_server_messages(TCPsocket client_socket, Player *opponent) {
    char buffer[BUFFER_SIZE];

    static SDLNet_SocketSet socket_set = NULL;
    if (!socket_set) {
        socket_set = SDLNet_AllocSocketSet(1);
        if (!socket_set) {
            fprintf(stderr, "Erreur lors de l'allocation du SocketSet : %s\n", SDLNet_GetError());
            return;
        }
        SDLNet_TCP_AddSocket(socket_set, client_socket);
    }

    if (SDLNet_CheckSockets(socket_set, 0) > 0 && SDLNet_SocketReady(client_socket)) {
        int bytes_received = SDLNet_TCP_Recv(client_socket, buffer, sizeof(buffer) - 1);

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';

            int x, y;
            if (sscanf(buffer, "MOVE %d %d", &x, &y) == 2) {
                opponent->rect.x = x * CELL_SIZE;
                opponent->rect.y = y * CELL_SIZE;
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



// Démarre le client
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
    receive_grid(client_socket, grid, &client_id);

    Player player, opponent = {.rect = {0, 0, CELL_SIZE, CELL_SIZE}};
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
                    case SDLK_q: new_x--; break;
                    case SDLK_d: new_x++; break;
                    case SDLK_z: new_y--; break;
                    case SDLK_s: new_y++; break;
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
        draw_player(renderer, &player, (SDL_Color){255, 0, 0, 255});
        draw_player(renderer, &opponent, (SDL_Color){0, 0, 255, 255});
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDLNet_TCP_Close(client_socket);
    SDLNet_Quit();
}
