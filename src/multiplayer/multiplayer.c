#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include "./../../include/multiplayer/check_maze.h"
#include "./../../include/multiplayer/client.h"
#include "./../../include/multiplayer/server.h"
#include "./../../include/utils/sdl_utils.h"

/**
 * Structure pour les données du client
 * const char *server_ip : adresse IP du serveur
 * SDL_Renderer *renderer : renderer
 */
typedef struct {
    const char *server_ip; 
    SDL_Renderer *renderer; 
} ClientData;

/**
 * Structure pour les données du serveur
 * int grid[ROWS][COLS] : grille
 */
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

    // Centrer le texte dans le bouton
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
    int scroll_offset = 0;  
    const int max_files_on_screen = 9; 

    // Charger l'image de fond
    SDL_Surface* backgroundSurface = IMG_Load("./assets/images/fondChoixMap.png");
    if (!backgroundSurface) {
        fprintf(stderr, "Erreur de chargement de l'image de fond : %s\n", IMG_GetError());
        return;
    }
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface); 

    FILE *fp = popen("ls ./data/editor", "r");
    if (!fp) {
        fprintf(stderr, "Erreur lors de l'ouverture du dossier\n");
        SDL_DestroyTexture(backgroundTexture); 
        return;
    }

    // Lire les noms de fichiers avec un scroll (offset)
    char filename[256];
    while (fgets(filename, sizeof(filename), fp) != NULL && fileCount < 100) {
        filename[strcspn(filename, "\n")] = 0; 
        strncpy(fileNames[fileCount], filename, sizeof(fileNames[fileCount]) - 1);

        SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, filename, textColor);
        if (textSurface) {
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

            SDL_Rect textRect = {50, y_offset, textSurface->w, textSurface->h};
            fileRects[fileCount] = textRect;

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

                // Vérifier si un fichier a été cliqué, si oui on charge la grille pour checker si elle est solvable
                for (int i = 0; i < fileCount; i++) {
                    if (x > fileRects[i].x && x < fileRects[i].x + fileRects[i].w &&
                        y > fileRects[i].y && y < fileRects[i].y + fileRects[i].h) {

                        if (!load_grid(fileNames[i], grid)) {
                            printf("Erreur lors du chargement de la grille\n");
                        } else {
                            // Vérifier si le labyrinthe est solvable
                            if (is_maze_solvable(grid)) {
                                // Si oui, on lance le serveur et le client (le serveur en thread à part)
                                ServerData serverData;
                                // Copie de la grille pour le thread serveur
                                memcpy(serverData.grid, grid, sizeof(serverData.grid)); 

                                // Création du thread serveur, cela signifie que le serveur va tourner en parallèle du client.
                                // Un thread est un processus léger qui partage le même espace mémoire que le processus principal.
                                SDL_Thread *serverThread = SDL_CreateThread(server_thread, "ServerThread", &serverData);
                                if (!serverThread) {
                                    printf("Erreur lors de la création du thread serveur : %s\n", SDL_GetError());
                                    exit(EXIT_FAILURE);
                                }

                                // Ajout d'un délai de 2 secondes pour laisser le temps au serveur de démarrer
                                SDL_Delay(2000);

                                // Données du client (ip locale et renderer)
                                ClientData clientData = { 
                                    "127.0.0.1",
                                    renderer     
                                };

                                // Démarrage du client
                                start_client(clientData.server_ip, clientData.renderer); 
                                // Attente de la fin du thread serveur
                                SDL_WaitThread(serverThread, NULL);
                            } else {
                                printf("Le labyrinthe n'est pas solvable\n");
                            }
                        }
                    }
                }
            } else if (event.type == SDL_MOUSEWHEEL) {  
                if (event.wheel.y > 0) {  
                    scroll_offset = (scroll_offset > 0) ? scroll_offset - 1 : 0;
                } else if (event.wheel.y < 0) {  
                    scroll_offset = (scroll_offset < fileCount - max_files_on_screen) ? scroll_offset + 1 : scroll_offset;
                }
            }
        }

        // Effacer l'écran
        SDL_RenderClear(renderer);

        // Afficher l'image de fond
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

        // Afficher les fichiers
        y_offset = 100;
        for (int i = scroll_offset; i < scroll_offset + max_files_on_screen && i < fileCount; i++) {
            SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, fileNames[i], textColor);
            if (textSurface) {
                SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

                SDL_Rect adjustedRect = fileRects[i];
                adjustedRect.y = y_offset; 

                SDL_RenderCopy(renderer, textTexture, NULL, &adjustedRect);

                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);
            }
            y_offset += 50;
        }

        // Mettre à jour l'affichage
        SDL_RenderPresent(renderer);
    }

    // Nettoyer la texture de fond
    SDL_DestroyTexture(backgroundTexture);
}

// Fonction pour saisir l'IP de l'hôte en mode client
void client_mode_input(SDL_Renderer *renderer, TTF_Font *font) {
    char inputText[256] = "";
    int running = 1;
    // Démarrer la saisie de texte
    SDL_StartTextInput();



    int screen_width = 672;
    int screen_height = 544;

    // Rectangle pour la saisie de texte (faux input)
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

    SDL_Texture* backgroundTexture = load_texture(renderer, "./assets/images/fondVierge.png");
    if (!backgroundTexture) {
        fprintf(stderr, "Erreur de chargement de l'image de fond : %s\n", IMG_GetError());
        return;
    }

    // Boucle principale
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
                // Arrêter la saisie de texte
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                printf("IP saisie : %s\n", inputText);
                // Vérifier si l'IP est valide
                if (strlen(inputText) > 0) {
                    start_client(inputText, renderer);
                }
            } else if (event.type == SDL_TEXTINPUT) {
                // Ajouter le texte saisi à la chaîne de texte
                if (strlen(inputText) + strlen(event.text.text) < sizeof(inputText) - 1) {
                    strcat(inputText, event.text.text);
                }
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE) {
                // Supprimer le dernier caractère
                size_t len = strlen(inputText);
                // Vérifier si la chaîne n'est pas vide
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

        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

        // Afficher le message de saisie
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

        // Afficher le texte saisi si la chaîne n'est pas vide
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
    SDL_DestroyTexture(backgroundTexture);

    SDL_StopTextInput();
}

// Fonction pour dessiner le texte d'un bouton
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

// Fonction pour démarrer le mode multijoueur
void start_multiplayer_mode(SDL_Renderer *renderer) {
    SDL_Rect joinButton = {0, 0, 200, 100};  
    SDL_Rect hostButton = {0, 0, 200, 100};  
    SDL_Rect backButton = {0, 0, 100, 50};   

    TTF_Font *font = TTF_OpenFont("./assets/fonts/font.ttf", 28);
    if (!font) {
        printf("Erreur lors du chargement de la police : %s\n", TTF_GetError());
        return;
    }

    SDL_Color textColor = {255, 255, 255, 255}; 
    SDL_Color buttonColor = {205, 133, 63, 255}; 

    // Charger l'image de fond
    SDL_Surface *bgSurface = IMG_Load("./assets/images/fondMulti.png"); 
    if (!bgSurface) {
        printf("Erreur lors du chargement de l'image de fond : %s\n", SDL_GetError());
        return;
    }

    SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);
    if (!bgTexture) {
        printf("Erreur lors de la création de la texture de fond : %s\n", SDL_GetError());
        return;
    }

    int selectedButton = 0;  
    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;

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
                    selectedButton = (selectedButton + 1) % 3;
                } else if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_LEFT) {
                    selectedButton = (selectedButton + 2) % 3;
                } else if (event.key.keysym.sym == SDLK_RETURN) {
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

        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &joinButton);
        SDL_RenderFillRect(renderer, &hostButton);
        SDL_RenderFillRect(renderer, &backButton);

        draw_button_text(renderer, joinButton, "Rejoindre", font, textColor);
        draw_button_text(renderer, hostButton, "Héberger", font, textColor);
        draw_button_text(renderer, backButton, "Retour", font, textColor);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyTexture(bgTexture);
    TTF_CloseFont(font);
}
