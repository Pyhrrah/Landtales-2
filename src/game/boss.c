#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <time.h>
#include "./../../include/core/ennemies.h"
#include "./../../include/core/player.h"
#include "./../../include/game/file.h"
#include "./../../include/game/map.h"
#include "./../../include/game/boss.h"
#include "./../../include/utils/video.h"
#include "./../../include/game/projectile.h"

#define ROWS 15
#define COLS 21
#define TILE_SIZE 32
#define MAX_PROJECTILES 100

extern Enemy enemies[];
extern int enemyCount;

SDL_Rect bossDoor = {0, 0, 0, 0};

time_t timestampLastSong = 0;
const char *lastSong = NULL;

void playRandomSound() {
    const char *sounds[] = {
        "assets/music/sons/Edusign.mp3",
        "assets/music/sons/Eleves.mp3",
        "assets/music/sons/Etudiants.mp3",
        "assets/music/sons/Langage.mp3",
        "assets/music/sons/Rotation.mp3"
    };
    int numSounds = sizeof(sounds) / sizeof(sounds[0]);

    // Vérifier le temps écoulé
    time_t now = time(NULL);
    if (now < timestampLastSong) {
        return; // Trop tôt, on ne joue rien
    }

    // Tirage aléatoire d'un son différent du précédent
    int index, attempts = 0;
    do {
        index = rand() % numSounds;
        attempts++;
    } while (attempts < 10 && lastSong && strcmp(lastSong, sounds[index]) == 0);

    lastSong = sounds[index];

    // Libérer la musique précédente si nécessaire
    check_and_free_music();

    // Jouer la nouvelle musique
    play_audio(sounds[index]);

    // Mettre à jour le timestamp
    timestampLastSong = now + 5;
}

// Initier les projectiles pour le boss
void initProjectilesBoss() {
    initProjectiles();  // Utiliser la fonction générique d'initialisation
}

// Spawn un projectile pour le boss
void spawnBossProjectile(SDL_Rect *bossRect, SDL_Rect *playerRect) {
    spawnProjectile(bossRect, playerRect, 5); // Utiliser la fonction générique de spawn
}

// Mettre à jour les projectiles du boss
void updateBossProjectiles(int mapRoom[ROWS][COLS]) {
    updateProjectiles(mapRoom); // Utiliser la fonction générique de mise à jour
}

// Vérifier les collisions des projectiles du boss avec le joueur
void checkBossProjectileCollisions(Player *player) {
    checkProjectileCollisions(player); // Utiliser la fonction générique de vérification de collisions
}

// Afficher les projectiles du boss à l'écran
void renderBossProjectiles(SDL_Renderer *renderer) {
    renderProjectiles(renderer); // Utiliser la fonction générique pour afficher les projectiles
}

// Vérifie si le boss est en vie
int isBossAlive(int enemyCount, int type) {
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].type == type) {
            return 1;
        }
    }
    return 0;
}

// Initialisation du boss
void initBigBoss() {
    clearEnemies();
    enemies[0].rect.x = 320 - 32;
    enemies[0].rect.y = 224 - 32;
    enemies[0].rect.w = 64;
    enemies[0].rect.h = 64;
    enemies[0].vie = 500;
    enemies[0].attaque = 20;
    enemies[0].defense = 10;
    enemies[0].type = 100;
    enemies[0].id = 0;
    enemyCount = 1;
    timestampLastSong = time(NULL) + 5;
}

// Gérer la porte du boss
void checkAndActivateBossDoor(int room, int enemyCount) {
    if (room == 2) {
        if (isBossAlive(enemyCount, 1) != 1) {
            bossDoor.x = 320;
            bossDoor.y = 160;
            bossDoor.w = TILE_SIZE;
            bossDoor.h = TILE_SIZE;
        }
    }
}
void handleBossDoorCollision(SDL_Renderer *renderer, SDL_Rect *playerRect, 
                             int *room, char *mapFilename, int mapRoom[ROWS][COLS], int saveNumber, 
                             int tentative, int *etage, Player *player, int mapData[11][11], char *stageFilename) {
    static SDL_Texture *bossDoorTexture = NULL;

    if (bossDoorTexture == NULL) {
        bossDoorTexture = IMG_LoadTexture(renderer, "./assets/images/sprite/map/spawn1.png");
        if (!bossDoorTexture) {
            printf("Erreur lors du chargement de l'image de la porte du boss : %s\n", IMG_GetError());
            return;
        }
    }

    if (bossDoor.w > 0 && bossDoor.h > 0) {
        SDL_RenderCopy(renderer, bossDoorTexture, NULL, &bossDoor);

        if (SDL_HasIntersection(playerRect, &bossDoor)) {
            printf("Joueur entre dans la porte, retour au lobby.\n");
            
            *room = 0;
            sprintf(mapFilename, "./data/game/lobbyMap.txt");
            loadRoomData(mapFilename, mapRoom);

            (*etage) += 1;

            bossDoor.w = 0;
            bossDoor.h = 0;

            clearEnemies();

            clearMapDirectory(saveNumber);

            if (*etage != 4) {
                creerEtageEntier(*etage);
                loadMap(stageFilename, mapData);
            }

            saveGame(saveNumber, tentative, player->vie, player->attaque, player->defense, *etage, player->argent, *room, player->max_vie);
        }
    }
}

// Lancer un projectile avec un cooldown
void launchProjectileWithCooldownBoss(SDL_Rect *bossRect, SDL_Rect *playerRect) {
    static Uint32 lastProjectileTime = 0;
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime > lastProjectileTime + 3000) { // 3 secondes d'intervalle
        spawnBossProjectile(bossRect, playerRect);
        lastProjectileTime = currentTime;
    }
}

// Gérer tous les projectiles du boss
void handleProjectilesBoss(SDL_Renderer *renderer, int mapRoom[ROWS][COLS], Player *player) {
    // Mettre à jour les projectiles
    updateBossProjectiles(mapRoom);

    // Vérifier les collisions entre les projectiles et le joueur
    checkBossProjectileCollisions(player);

    // Afficher les projectiles à l'écran
    renderBossProjectiles(renderer);
    playRandomSound();
}
