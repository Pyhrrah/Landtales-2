#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include "./../../include/multiplayer/check_maze.h"
#include "./../../include/multiplayer/client.h"
#include "./../../include/multiplayer/server.h"

// Structure pour les données du client
typedef struct {
    const char *server_ip; 
    SDL_Renderer *renderer; 
} ClientData;

/// Structure pour les données du serveur
typedef struct {
    int grid[ROWS][COLS]; 
} ServerData;

// Création d'un thread pour le serveur afin de lui passer les infos
int server_thread(void *data) {
    ServerData *server_data = (ServerData *)data; 
    start_server(server_data->grid);             
    return 0;                                    
}


// Fonction pour dessiner un bouton avec texte
void draw_button(SDL_Renderer *renderer, SDL_Rect rect, const char *text, TTF_Font *font, SDL_Color textColor) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, text, textColor);
    if (!textSurface) {
        printf("Erreur lors de la création de la surface de texte : %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        printf("Erreur lors de la création de la texture de texte : %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    int text_width = textSurface->w;
    int text_height = textSurface->h;
    SDL_Rect textRect = {
        rect.x + (rect.w - text_width) / 2,
        rect.y + (rect.h - text_height) / 2,
        text_width,
        text_height
    };

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

// Fonction pour afficher la liste des fichiers
void display_file_list(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Color textColor = {255, 255, 255, 255};
    char fileNames[100][256];
    SDL_Rect fileRects[100]; 
    int fileCount = 0;
    int y_offset = 100;

    FILE *fp = popen("ls ./data/editor", "r");
    if (!fp) {
        fprintf(stderr, "Erreur lors de l'ouverture du dossier\n");
        return;
    }

    char filename[256];
    while (fgets(filename, sizeof(filename), fp) != NULL && fileCount < 100) {
        filename[strcspn(filename, "\n")] = 0; 
        strncpy(fileNames[fileCount], filename, sizeof(fileNames[fileCount]) - 1);

        SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, filename, textColor);
        if (textSurface) {
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

            SDL_Rect textRect = {50, y_offset, textSurface->w, textSurface->h};
            fileRects[fileCount] = textRect;

            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }

        y_offset += 50;
        fileCount++;
    }

    int grid[ROWS][COLS] = {0};

    pclose(fp);

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;

                for (int i = 0; i < fileCount; i++) {
                    if (x > fileRects[i].x && x < fileRects[i].x + fileRects[i].w &&
                        y > fileRects[i].y && y < fileRects[i].y + fileRects[i].h) {


                            // Passage en mode thread si la map est trouvé : le serveur se lance en parallèle du client qui profite de la fenetre SDL déjà ouverte
                        if (!load_grid(fileNames[i], grid)) {
                            printf("Erreur lors du chargement de la grille\n");
                        } else {
                            if (is_maze_solvable(grid)) {
                                ServerData serverData;
                                memcpy(serverData.grid, grid, sizeof(serverData.grid)); 

                                SDL_Thread *serverThread = SDL_CreateThread(server_thread, "ServerThread", &serverData);
                                if (!serverThread) {
                                    printf("Erreur lors de la création du thread serveur : %s\n", SDL_GetError());
                                    exit(EXIT_FAILURE);
                                }

                                SDL_Delay(2000); // Délai permettant au serveur d'être lancé
                                // Sans ce délai, le client essaie de se connecter instantanément au serveur pas encore prêt. 
                                // Donc il plante et fait crasher le programme.

                                ClientData clientData = { 
                                    "127.0.0.1", // Le serveur et un client sont lancé en simultané sur la même machine, donc l'adresse IP est localhost
                                    renderer     
                                };

                                start_client(clientData.server_ip, clientData.renderer); 

                                SDL_WaitThread(serverThread, NULL);
                            } else {
                                printf("Le labyrinthe n'est pas solvable\n");
                            }
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        y_offset = 100;
        for (int i = 0; i < fileCount; i++) {
            SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, fileNames[i], textColor);
            if (textSurface) {
                SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

                SDL_RenderCopy(renderer, textTexture, NULL, &fileRects[i]);

                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);
            }
        }

        SDL_RenderPresent(renderer);
    }
}

// Fonction pour saisir l'IP de l'hôte en mode client
void client_mode_input(SDL_Renderer *renderer, TTF_Font *font) {
    char inputText[256] = "";
    int running = 1;
    SDL_StartTextInput();

    int screen_width = 672;
    int screen_height = 544;

    SDL_Rect inputRect = {0, 0, 400, 50};
    SDL_Color bgColor = {50, 50, 50, 255};   
    SDL_Color borderColor = {255, 255, 255, 255}; 
    SDL_Color textColor = {255, 255, 255, 255};

    inputRect.x = (screen_width - inputRect.w) / 2;
    inputRect.y = screen_height / 2;

    SDL_Rect backButton = {0, 0, 100, 50}; 
    backButton.x = screen_width - backButton.w - 20;  
    backButton.y = screen_height - backButton.h - 20;

    SDL_Color backButtonColor = {50, 50, 50, 255}; 

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                printf("IP saisie : %s\n", inputText);
                if (strlen(inputText) > 0) {
                    start_client(inputText, renderer);
                }
            } else if (event.type == SDL_TEXTINPUT) {
                if (strlen(inputText) + strlen(event.text.text) < sizeof(inputText) - 1) {
                    strcat(inputText, event.text.text);
                }
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE) {
                size_t len = strlen(inputText);
                if (len > 0) {
                    inputText[len - 1] = '\0';
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                if (x > backButton.x && x < backButton.x + backButton.w &&
                    y > backButton.y && y < backButton.y + backButton.h) {
                    running = 0; 
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Surface *messageSurface = TTF_RenderUTF8_Solid(font, "Renseignez l'IP de l'hôte :", textColor);
        if (messageSurface) {
            SDL_Texture *messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
            SDL_Rect messageRect = {
                (screen_width - messageSurface->w) / 2, 
                inputRect.y - 60,  
                messageSurface->w, 
                messageSurface->h
            };
            SDL_RenderCopy(renderer, messageTexture, NULL, &messageRect);
            SDL_FreeSurface(messageSurface);
            SDL_DestroyTexture(messageTexture);
        }

        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(renderer, &inputRect);

        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderDrawRect(renderer, &inputRect);

        if (strlen(inputText) > 0) {
            SDL_Surface *inputSurface = TTF_RenderUTF8_Solid(font, inputText, textColor);
            if (inputSurface) {
                SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(renderer, inputSurface);
                SDL_Rect textRect = {
                    inputRect.x + 10,
                    inputRect.y + (inputRect.h - inputSurface->h) / 2, 
                    inputSurface->w,
                    inputSurface->h
                };
                SDL_RenderCopy(renderer, inputTexture, NULL, &textRect);
                SDL_FreeSurface(inputSurface);
                SDL_DestroyTexture(inputTexture);
            } else {
                printf("Erreur lors de la création de la surface de texte : %s\n", TTF_GetError());
            }
        }

        SDL_SetRenderDrawColor(renderer, backButtonColor.r, backButtonColor.g, backButtonColor.b, backButtonColor.a);  
        SDL_RenderFillRect(renderer, &backButton);

        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderDrawRect(renderer, &backButton);

        SDL_Surface *backTextSurface = TTF_RenderUTF8_Solid(font, "Retour", textColor);
        if (backTextSurface) {
            SDL_Texture *backTextTexture = SDL_CreateTextureFromSurface(renderer, backTextSurface);
            SDL_Rect backTextRect = {
                backButton.x + (backButton.w - backTextSurface->w) / 2,
                backButton.y + (backButton.h - backTextSurface->h) / 2,
                backTextSurface->w,
                backTextSurface->h
            };
            SDL_RenderCopy(renderer, backTextTexture, NULL, &backTextRect);
            SDL_FreeSurface(backTextSurface);
            SDL_DestroyTexture(backTextTexture);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput();
}

void draw_button_text(SDL_Renderer *renderer, SDL_Rect button, const char *text, TTF_Font *font, SDL_Color color) {
    SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, text, color);
    if (textSurface) {
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            int textWidth = textSurface->w;
            int textHeight = textSurface->h;
            SDL_Rect textRect = {button.x + (button.w - textWidth) / 2, button.y + (button.h - textHeight) / 2, textWidth, textHeight};
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    }
}

void start_multiplayer_mode(SDL_Renderer *renderer) {
    SDL_Rect joinButton = {0, 0, 200, 100};  
    SDL_Rect hostButton = {0, 0, 200, 100};  
    SDL_Rect backButton = {0, 0, 100, 50};   

    TTF_Font *font = TTF_OpenFont("./assets/fonts/font.ttf", 28);
    if (!font) {
        printf("Erreur lors du chargement de la police : %s\n", TTF_GetError());
        return;
    }

    SDL_Color textColor = {255, 255, 255, 255};  // Texte blanc
    SDL_Color buttonColor = {205, 133, 63, 255};  // Marron clair

    // Charger l'image de fond
    SDL_Surface *bgSurface = IMG_Load("./assets/images/fondMulti.png"); // Assure-toi que le chemin de l'image est correct
    if (!bgSurface) {
        printf("Erreur lors du chargement de l'image de fond : %s\n", SDL_GetError());
        return;
    }

    SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);  // Libère la surface après avoir créé la texture
    if (!bgTexture) {
        printf("Erreur lors de la création de la texture de fond : %s\n", SDL_GetError());
        return;
    }

    int selectedButton = 0;  // Indique quel bouton est sélectionné (0 = rejoindre, 1 = héberger, 2 = retour)
    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;

                // Vérifier quel bouton a été cliqué
                if (x > joinButton.x && x < joinButton.x + joinButton.w &&
                    y > joinButton.y && y < joinButton.y + joinButton.h) {
                    client_mode_input(renderer, font);
                } else if (x > hostButton.x && x < hostButton.x + hostButton.w &&
                           y > hostButton.y && y < hostButton.y + hostButton.h) {
                    display_file_list(renderer, font);
                } else if (x > backButton.x && x < backButton.x + backButton.w &&
                           y > backButton.y && y < backButton.y + backButton.h) {
                    running = 0; 
                }
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_RIGHT) {
                    // Sélectionner le bouton suivant
                    selectedButton = (selectedButton + 1) % 3;
                } else if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_LEFT) {
                    // Sélectionner le bouton précédent
                    selectedButton = (selectedButton + 2) % 3;
                } else if (event.key.keysym.sym == SDLK_RETURN) {
                    // Activer le bouton sélectionné
                    if (selectedButton == 0) {
                        client_mode_input(renderer, font);
                    } else if (selectedButton == 1) {
                        display_file_list(renderer, font);
                    } else if (selectedButton == 2) {
                        running = 0;
                    }
                }
            }
        }

        int screen_width = 672; 
        int screen_height = 544; 

        joinButton.x = (screen_width - joinButton.w) / 2;
        hostButton.x = (screen_width - hostButton.w) / 2;

        int button_spacing = 20;  
        joinButton.y = (screen_height - (joinButton.h + hostButton.h + button_spacing)) / 2;
        hostButton.y = joinButton.y + joinButton.h + button_spacing;

        backButton.x = screen_width - backButton.w - 20;
        backButton.y = screen_height - backButton.h - 20;

        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        // Dessiner les fonds des boutons (marron clair)
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &joinButton);
        SDL_RenderFillRect(renderer, &hostButton);
        SDL_RenderFillRect(renderer, &backButton);

        // Dessiner le texte des boutons
        draw_button_text(renderer, joinButton, "Rejoindre", font, textColor);
        draw_button_text(renderer, hostButton, "Héberger", font, textColor);
        draw_button_text(renderer, backButton, "Retour", font, textColor);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(bgTexture);
    TTF_CloseFont(font);
}
