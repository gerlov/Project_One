#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include "tilemap.h"
#include "character.h"
#include "music.h"
#include "powerup.h"
#include "limitedvision.h"



#define MAX_PLAYERS 6

// Enum for game states
typedef enum
{
    PAUSED,
    PLAYING,
    QUIT
} GameState;


typedef struct _Game
{
    int speed;
    int TILE_SIZE;
    int SPEED;
    int TILE_W_AMOUNT;
    int TILE_H_AMOUNT;
    int GAME_WIDTH;
    int GAME_HEIGHT;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
    int PLAYERS;
    int seed;
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    BackgroundMusic *bgm;
    Character *characters[MAX_PLAYERS];
    Character *hunter;
    TileMap tilemap;

    PowerUp powerUps[MAX_POWERUPS];
    int powerUpCount;

    // Do not send to server
    GameState gameState;
    LimitedVision lv;
    bool space, music, m, lower_volume, inc_volume;
    bool closeWindow;
    float deltaTime;
    Uint64 lastFrameTime;
    Uint64 currentFrameTime;

    // TODO(THEO): move this to input.h later
    union 
    {
        struct
        {
            int up : 1;
            int down : 1;
            int left : 1;
            int right : 1;
            int space : 1;
        };
        int all : 5;
    } keys;

} Game;

/// @brief Initiate the actual game
int runGame();

#endif