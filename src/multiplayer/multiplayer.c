#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <string.h>
#include "./../../include/multiplayer/check_maze.h"
#include "./../../include/multiplayer/client.h"
#include "./../../include/multiplayer/server.h"

// Fonction pour dessiner un bouton avec texte
void draw_button(SDL_Renderer *renderer, SDL_Rect rect, const char *text, TTF_Font *font, SDL_Color textColor) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
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

        SDL_Surface *textSurface = TTF_RenderText_Solid(font, filename, textColor);
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

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;

                for (int i = 0; i < fileCount; i++) {
                    if (x > fileRects[i].x && x < fileRects[i].x + fileRects[i].w &&
                        y > fileRects[i].y && y < fileRects[i].y + fileRects[i].h) {

                        if (!load_grid(fileNames[i], grid)) {
                            printf("Erreur lors du chargement de la grille\n");
                        } else {
                            if (is_maze_solvable(grid)) {
                                start_server(grid);
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
            SDL_Surface *textSurface = TTF_RenderText_Solid(font, fileNames[i], textColor);
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
    bool running = true;
    SDL_StartTextInput();

    SDL_Rect inputRect = {100, 200, 400, 50};
    SDL_Color bgColor = {50, 50, 50, 255};   
    SDL_Color borderColor = {255, 255, 255, 255}; 
    SDL_Color textColor = {255, 255, 255, 255};

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
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
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Surface *messageSurface = TTF_RenderText_Solid(font, "Renseignez l'IP de l'hôte :", textColor);
        if (messageSurface) {
            SDL_Texture *messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
            SDL_Rect messageRect = {100, 100, messageSurface->w, messageSurface->h};
            SDL_RenderCopy(renderer, messageTexture, NULL, &messageRect);
            SDL_FreeSurface(messageSurface);
            SDL_DestroyTexture(messageTexture);
        }

        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(renderer, &inputRect);

        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderDrawRect(renderer, &inputRect);

        if (strlen(inputText) > 0) {
            SDL_Surface *inputSurface = TTF_RenderText_Solid(font, inputText, textColor);
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

        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput();
}

// Fonction principale pour le mode multijoueur
void start_multiplayer_mode(SDL_Renderer *renderer) {
    SDL_Rect clientButton = {100, 100, 200, 100};
    SDL_Rect serverButton = {400, 100, 200, 100};

    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 28);
    if (!font) {
        printf("Erreur lors du chargement de la police : %s\n", TTF_GetError());
        return;
    }

    SDL_Color textColor = {0, 0, 0, 255};

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;

                if (x > clientButton.x && x < clientButton.x + clientButton.w &&
                    y > clientButton.y && y < clientButton.y + clientButton.h) {
                    client_mode_input(renderer, font);
                } else if (x > serverButton.x && x < serverButton.x + serverButton.w &&
                           y > serverButton.y && y < serverButton.y + serverButton.h) {
                    display_file_list(renderer, font);                
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        draw_button(renderer, clientButton, "Client", font, textColor);
        draw_button(renderer, serverButton, "Serveur", font, textColor);

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
}
