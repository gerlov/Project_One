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
    int escapers;
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
    int pwrID; 
} PowerupCollected;

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
    int lastPowerupCollected;
    int escapers;
} CharacterData;


typedef struct {
    CharacterData characters[MAX_PLAYERS];
    bool isKilled[MAX_PLAYERS]; // 1 if player at index is killed
    GameState gameState;
    int escapers;
} ServerData;

#endif // DATA_H