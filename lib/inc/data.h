#ifndef DATA_H
#define DATA_H
#include <SDL2/SDL.h>
#include "tilemap.h"
#include "character.h"

#define MAX_PLAYERS 6
#define SOCKET_PORT 2000 

typedef enum {
    START,
    JOINING,
    PAUSED,
    PLAYING,
    QUIT
} GameState;

typedef struct JoinData {
    int seed;
    int playerINDEX;
    int PLAYERS;
    int hunterindex;
    int readyPlayers[MAX_PLAYERS];
    GameState gameState;
} JoinData;

typedef struct 
{
    bool ready;
    bool start;
    bool quit;
} ReadyData;

typedef struct {
    int playerID;
    bool disconnect;
    SDL_FPoint position;
    SDL_FPoint velocity;
    int health;
    int visible;
    int iskilled;
    bool isHunter;
    bool hasKilledindex[MAX_PLAYERS];
    bool isMoving;
    int currentFrame;
    Uint32 speedPowerupTime;
    Uint32 invisiblePowerupTime;
    Uint32 frameLastUpdated;
    GameState gameState;
} CharacterData;


typedef struct {
    CharacterData characters[MAX_PLAYERS];
    bool isKilled[MAX_PLAYERS]; // 1 if player at index is killed
} ServerData;

// void printJoinData(JoinData data)
// {
//     printf("Seed: %d\n", data.seed);
//     printf("Player index: %d\n", data.playerINDEX);
//     printf("Players: %d\n", data.PLAYERS);
//     printf("Game state: %d\n", data.gameState);
//     printf("Ready players: ");
//     for (int i = 0; i < data.PLAYERS; i++)
//     {
//         printf("%d ", data.readyPlayers[i]);
//     }
//     printf("\n");
// }
// void printServerData(ServerData data)
// {
    
//     printf("Game state: %d\n", data.characters[0]);
//     printf("\n");
// }

#endif // DATA_H