#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define ROWS 15
#define COLS 21
#define MAX_CLIENTS 2
#define MOVE_DELAY 10 

/**
 * Structure pour représenter un client : un socket est un point de connexion entre deux machines
 * TCPsocket socket : socket du client
 * int id : identifiant du client
 * int last_x, last_y : dernières coordonnées du joueur
 * Uint32 last_move_time : temps du dernier mouvement
 * char orientation : orientation du joueur (U, D, R, L)
 */
typedef struct {
    TCPsocket socket;
    int id;
    int last_x, last_y;
    Uint32 last_move_time;
    char orientation;
} Client;

Client clients[MAX_CLIENTS];

// Fonction pour envoyer la grille à un client
void send_grid(TCPsocket client_socket, int grid[ROWS][COLS], int client_id, char orientation) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            SDLNet_TCP_Send(client_socket, &grid[i][j], sizeof(int));
        }
    }
    SDLNet_TCP_Send(client_socket, &client_id, sizeof(int));
    SDLNet_TCP_Send(client_socket, &orientation, sizeof(char));
}

// Fonction pour démarrer le jeu
void start_game(Client *clients) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        int start_game_signal = 1;
        char initial_orientation = 'U';
        SDLNet_TCP_Send(clients[i].socket, &start_game_signal, sizeof(int));
        SDLNet_TCP_Send(clients[i].socket, &initial_orientation, sizeof(char));
    }
}

// Fonction pour gérer les mouvements des joueurs
void handle_player_movements(Client *clients) {

    // Pour chaque client, nous allons recevoir les données envoyées par le client et les traiter (mouvements + renvoi des données à l'autre joueur)
    for (int i = 0; i < MAX_CLIENTS; i++) {
        char buffer[BUFFER_SIZE];
        // Réception des données du client
        int bytes_received = SDLNet_TCP_Recv(clients[i].socket, buffer, BUFFER_SIZE - 1);

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';

            int x, y;
            char orientation;
            // Analyse du message reçu
            if (sscanf(buffer, "MOVE %d %d %c", &x, &y, &orientation) == 3) {
                Uint32 current_time = SDL_GetTicks();
                // Vérification du délai entre deux mouvements (si trop rapide, on ignore le mouvement, pour des raisons de performance)
                if ((x != clients[i].last_x || y != clients[i].last_y) && (current_time - clients[i].last_move_time > MOVE_DELAY)) {
                    clients[i].last_x = x;
                    clients[i].last_y = y;
                    clients[i].last_move_time = current_time;
                    clients[i].orientation = orientation;

                    // Envoi du mouvement à l'autre joueur
                    int other_player_id = (clients[i].id == 1) ? 2 : 1;
                    char move_message[BUFFER_SIZE];
                    // Création du message de mouvement
                    snprintf(move_message, sizeof(move_message), "MOVE %d %d %c", x, y, orientation);

                    // Envoi du message à l'autre joueur
                    if (clients[other_player_id - 1].socket) {
                        SDLNet_TCP_Send(clients[other_player_id - 1].socket, move_message, strlen(move_message) + 1);
                    }
                }
            } else {
                printf("Message invalide reçu du joueur %d : %s\n", clients[i].id, buffer);
            }
        }
    }
}

// Fonction pour démarrer le serveur
void start_server(int grid[ROWS][COLS]) {
    TCPsocket server_socket, client_socket[MAX_CLIENTS];
    IPaddress ip;
    SDLNet_SocketSet socket_set;
    int client_count = 0;
    SDL_Event event;

    if (SDLNet_Init() == -1) {
        printf("Erreur SDLNet_Init : %s\n", SDLNet_GetError());
        exit(1);
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL_Init : %s\n", SDL_GetError());
        exit(1);
    }

    if (SDLNet_ResolveHost(&ip, NULL, PORT) == -1) {
        printf("Erreur SDLNet_ResolveHost : %s\n", SDLNet_GetError());
        exit(1);
    }
    // Création du socket du serveur sur l'iP et le port spécifiés
    server_socket = SDLNet_TCP_Open(&ip);
    if (!server_socket) {
        printf("Erreur SDLNet_TCP_Open : %s\n", SDLNet_GetError());
        exit(1);
    }

    printf("Serveur en attente de connexion sur le port %d...\n", PORT);

    // Création du socket set pour gérer les sockets actifs
    socket_set = SDLNet_AllocSocketSet(MAX_CLIENTS + 1);
    if (!socket_set) {
        printf("Erreur SDLNet_AllocSocketSet : %s\n", SDLNet_GetError());
        exit(1);
    }

    // Ajout du socket du serveur dans le socket set
    SDLNet_TCP_AddSocket(socket_set, server_socket);

    int running = 1;

    while (running) {
        // Vérification des sockets actifs
        int active_sockets = SDLNet_CheckSockets(socket_set, 100);

        if (active_sockets > 0) {
            // Vérification du socket du serveur
            if (SDLNet_SocketReady(server_socket)) {
                if (client_count < MAX_CLIENTS) {
                    client_socket[client_count] = SDLNet_TCP_Accept(server_socket);
                    if (client_socket[client_count]) {
                        clients[client_count].socket = client_socket[client_count];
                        clients[client_count].id = client_count + 1;
                        clients[client_count].last_x = -1;
                        clients[client_count].last_y = -1;
                        clients[client_count].last_move_time = SDL_GetTicks();
                        clients[client_count].orientation = 'U';
                        send_grid(client_socket[client_count], grid, client_count + 1, 'U');
                        client_count++;
                        if (client_count >= MAX_CLIENTS) {
                            start_game(clients);
                        }
                    }
                }
            }
        }

        if (client_count == MAX_CLIENTS) {
            handle_player_movements(clients);
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
            }
        }

        SDL_Delay(10);
    }

    // Fermeture des sockets
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket) {
            SDLNet_TCP_Close(clients[i].socket);
        }
    }
    SDLNet_TCP_Close(server_socket);
    SDLNet_FreeSocketSet(socket_set);
    SDLNet_Quit();
}
