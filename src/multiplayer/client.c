#include <SDL2/SDL.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define PORT 12345
#define ROWS 15
#define COLS 21
#define CELL_SIZE 32
#define BUFFER_SIZE 1024 // Taille du buffer de réception et sert à envoyer des messages

typedef struct {
    SDL_Rect rect;
} Player;

// Vérifie les collisions avec des tuiles bloquantes
bool check_collision(int grid[ROWS][COLS], int new_x, int new_y) {
    if (new_x < 0 || new_x >= COLS || new_y < 0 || new_y >= ROWS) {
        return false;
    }
    int blocked_tiles[] = {5, 6, 7, 8,9,10,11};
    int tile_value = grid[new_y][new_x];
    for (size_t i = 0; i < sizeof(blocked_tiles) / sizeof(blocked_tiles[0]); i++) {
        if (tile_value == blocked_tiles[i]) {
            return false; 
        }
    }
    return true; 
}

// Reçoit la grille et l'identifiant du joueur
void receive_grid(int client_socket, int grid[ROWS][COLS], int *client_id) {
    memset(grid, 0, sizeof(int) * ROWS * COLS);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (recv(client_socket, &grid[i][j], sizeof(int), 0) <= 0) {
                perror("Erreur de réception de données");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (recv(client_socket, client_id, sizeof(int), 0) <= 0) {
        perror("Erreur de réception de l'identifiant");
        exit(EXIT_FAILURE);
    }
}

// Dessine la grille
void draw_grid(SDL_Renderer *renderer, int grid[ROWS][COLS]) {
    SDL_Color colors[] = {
        {0, 0, 0, 255},       {70, 200, 70, 255},  {50, 150, 50, 255},
        {30, 100, 30, 255},   {10, 50, 10, 255},   {150, 75, 0, 255},
        {100, 100, 100, 255}, {100, 100, 100, 255}, {0, 0, 255, 255},
        {128, 128, 128, 255}, {169, 169, 169, 255}, {139, 69, 19, 255},
        {255, 0, 0, 255},     {0, 0, 255, 255}};
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            SDL_Rect rect = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            int value = grid[i][j];
            if (value >= 1 && value <= 13) {
                SDL_SetRenderDrawColor(renderer, colors[value].r, colors[value].g, colors[value].b, colors[value].a);
            } else {
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
            }
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

// Initialise la position du joueur
void initialize_player(int grid[ROWS][COLS], Player *player, int client_id) {
    int spawn_tile = (client_id == 1) ? 12 : 13;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j] == spawn_tile) {
                player->rect = (SDL_Rect){j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                return;
            }
        }
    }
    fprintf(stderr, "Erreur : Position de spawn introuvable pour le joueur %d.\n", client_id);
    exit(EXIT_FAILURE);
}

// Dessine un joueur
void draw_player(SDL_Renderer *renderer, Player *player, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &player->rect);
}

// Envoie la position du joueur au serveur
void send_player_position(int client_socket, Player *player) {
    char message[BUFFER_SIZE];
    snprintf(message, sizeof(message), "MOVE %d %d\n", player->rect.x / CELL_SIZE, player->rect.y / CELL_SIZE);
    send(client_socket, message, strlen(message), 0);
}

// Gère les messages du serveur
void handle_server_messages(int client_socket, Player *opponent) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        int x, y;
        if (sscanf(buffer, "MOVE %d %d", &x, &y) == 2) {
            opponent->rect.x = x * CELL_SIZE;
            opponent->rect.y = y * CELL_SIZE;
        }
    }
}

// Démarre le client
void start_client(const char *server_ip, SDL_Renderer *renderer) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Erreur de création de socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
    };
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Adresse IP invalide");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connexion au serveur échouée");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connecté au serveur.\n");

    int grid[ROWS][COLS] = {0}, client_id = 0;
    receive_grid(client_socket, grid, &client_id);

    Player player, opponent = {.rect = {0, 0, CELL_SIZE, CELL_SIZE}};
    initialize_player(grid, &player, client_id);

    SDL_Event event;
    bool running = true;

    Uint32 last_send_time = SDL_GetTicks();
    const Uint32 send_interval = 200;

    while (running) {
        handle_server_messages(client_socket, &opponent);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
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

    close(client_socket);
}
