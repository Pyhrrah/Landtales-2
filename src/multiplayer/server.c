#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Le serveur sert ici de messager entre les deux clients,
// Il n'est pas prévu qu'il gère la logique des joueurs également (trop complexe pour le moment)

#define PORT 12345
#define BUFFER_SIZE 1024
#define ROWS 15
#define COLS 21
#define MAX_CLIENTS 2
#define MOVE_DELAY 10 

typedef struct {
    TCPsocket socket;
    int id;
    int last_x, last_y;    
    char direction;
    int hp; 
    Uint32 last_move_time;  
} Client;

Client clients[MAX_CLIENTS];

// Fonction pour envoyer la grille et l'identifiant au client
void send_grid(TCPsocket client_socket, int grid[ROWS][COLS], int client_id) {

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            SDLNet_TCP_Send(client_socket, &grid[i][j], sizeof(int));
        }
    }
    SDLNet_TCP_Send(client_socket, &client_id, sizeof(int));
}

// Fonction pour démarrer le jeu et prévenir les clients
void start_game(Client *clients) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        int start_game_signal = 1;
        SDLNet_TCP_Send(clients[i].socket, &start_game_signal, sizeof(int));
    }
}

// Fonction pour gérer les mouvements des joueurs et envoyer les mises à jour
// Fonction pour gérer les mouvements des joueurs et envoyer les mises à jour
void handle_player_movements(Client *clients) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].socket) {
            continue; // Passer si aucun socket n'est associé
        }

        char buffer[BUFFER_SIZE];
        int bytes_received = SDLNet_TCP_Recv(clients[i].socket, buffer, BUFFER_SIZE - 1);

        if (bytes_received > 0) {
            // Terminer correctement le buffer et supprimer les retours à la ligne
            buffer[bytes_received] = '\0';
            for (int k = 0; k < bytes_received; k++) {
                if (buffer[k] == '\n' || buffer[k] == '\r') {
                    buffer[k] = '\0';
                    break;
                }
            }

            // Debug : afficher le message brut reçu
            printf("Message brut reçu du joueur %d : '%s'\n", clients[i].id, buffer);

            int x, y;
            char direction;

            // Gestion des messages "MOVE"
            if (sscanf(buffer, "MOVE %d %d %c", &x, &y, &direction) == 3) {
                time_t current_time = time(NULL); // Utilise time(NULL) pour obtenir un timestamp Unix

                // Vérifier si les coordonnées ou la direction ont changé
                if ((x != clients[i].last_x || y != clients[i].last_y || direction != clients[i].direction) &&
                    (difftime(current_time, clients[i].last_move_time) > MOVE_DELAY)) {  // Utilise difftime pour la différence en secondes

                    // Mettre à jour les données du joueur
                    clients[i].last_x = x;
                    clients[i].last_y = y;
                    clients[i].direction = direction;
                    clients[i].last_move_time = current_time;

                    // Envoyer les mouvements à l'autre joueur
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (j != i && clients[j].socket) {
                            char move_message[BUFFER_SIZE];
                            snprintf(move_message, sizeof(move_message), "MOVE %d %d %c\n", x, y, direction);
                            SDLNet_TCP_Send(clients[j].socket, move_message, strlen(move_message) + 1);
                        }
                    }
                }

            // Gestion des messages "SWORD"
            } else if (strncmp(buffer, "SWORD", 5) == 0) {
                int sword_x, sword_y, sword_w, sword_h;
                time_t sword_timestamp;

                // Extraire les informations du message "SWORD"
                if (sscanf(buffer, "SWORD %d %d %d %d %ld", &sword_x, &sword_y, &sword_w, &sword_h, &sword_timestamp) == 5) {
                    printf("Message SWORD valide reçu du joueur %d : %d %d %d %d %ld\n",
                           clients[i].id, sword_x, sword_y, sword_w, sword_h, sword_timestamp);

                    // Envoyer le message "SWORD" à l'autre joueur
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (j != i && clients[j].socket) {
                            SDLNet_TCP_Send(clients[j].socket, buffer, strlen(buffer) + 1);
                        }
                    }

                    // Vérifier si un joueur est touché
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (j == i || !clients[j].socket) continue;

                        // Vérifier si le timestamp du joueur est proche du coup d'épée
                        if (abs((int)(sword_timestamp - clients[j].last_move_time)) <= 1) { // 1 seconde de différence
                            int player_x = clients[j].last_x;
                            int player_y = clients[j].last_y;

                            // Vérifier la collision entre le joueur et la zone de l'épée
                            if (player_x >= sword_x && player_x < sword_x + sword_w &&
                                player_y >= sword_y && player_y < sword_y + sword_h) {

                                // Déterminer qui est touché et envoyer le message
                                char damage_message[BUFFER_SIZE];
                                snprintf(damage_message, sizeof(damage_message),
                                         "DAMAGE 10 %s\n", (j == i) ? "ME" : "OTHER");

                                // Envoyer le message aux deux joueurs
                                for (int k = 0; k < MAX_CLIENTS; k++) {
                                    if (clients[k].socket) {
                                        SDLNet_TCP_Send(clients[k].socket, damage_message, strlen(damage_message) + 1);
                                    }
                                }
                            }
                        }
                    }
                } else {
                    printf("Message SWORD invalide reçu du joueur %d : '%s'\n", clients[i].id, buffer);
                }

            } else {
                printf("Message invalide reçu du joueur %d : '%s'\n", clients[i].id, buffer);
            }

        } else if (bytes_received == 0) {
            // Le client s'est déconnecté
            printf("Le joueur %d s'est déconnecté.\n", clients[i].id);
            SDLNet_TCP_Close(clients[i].socket);
            clients[i].socket = NULL;
        } else {
            // Une erreur est survenue lors de la réception
            fprintf(stderr, "Erreur lors de la réception des données du joueur %d : %s\n", clients[i].id, SDLNet_GetError());
        }
    }
}




// Fonction principale du serveur
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

    server_socket = SDLNet_TCP_Open(&ip);
    if (!server_socket) {
        printf("Erreur SDLNet_TCP_Open : %s\n", SDLNet_GetError());
        exit(1);
    }

    printf("Serveur en attente de connexion sur le port %d...\n", PORT);

    socket_set = SDLNet_AllocSocketSet(MAX_CLIENTS + 1);
    if (!socket_set) {
        printf("Erreur SDLNet_AllocSocketSet : %s\n", SDLNet_GetError());
        exit(1);
    }

    SDLNet_TCP_AddSocket(socket_set, server_socket);

    int running = 1;

    while (running) {
        int active_sockets = SDLNet_CheckSockets(socket_set, 100);

        if (active_sockets > 0) {
            if (SDLNet_SocketReady(server_socket)) {
                if (client_count < MAX_CLIENTS) {
                    client_socket[client_count] = SDLNet_TCP_Accept(server_socket);
                    if (client_socket[client_count]) {
                        clients[client_count].socket = client_socket[client_count];
                        clients[client_count].id = client_count + 1;
                        clients[client_count].last_x = -1; // Initialiser les coordonnées
                        clients[client_count].last_y = -1;
                        clients[client_count].last_move_time = SDL_GetTicks(); // Temps initial
                        send_grid(client_socket[client_count], grid, client_count + 1);
                        client_count++;
                        if (client_count >= MAX_CLIENTS) {
                            start_game(clients);
                        }
                    }
                }
            }
        }

        // Gérer les mouvements des joueurs
        if (client_count == MAX_CLIENTS) {
            handle_player_movements(clients);
        }

        // Vérifier les événements SDL
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
            }
        }

        SDL_Delay(10);
    }

    // Nettoyage des sockets
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket) {
            SDLNet_TCP_Close(clients[i].socket);
        }
    }
    SDLNet_TCP_Close(server_socket);
    SDLNet_FreeSocketSet(socket_set);
    SDLNet_Quit();
}
