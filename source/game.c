#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "tilemap.h"
#include "music.h"
#include "window.h"
#include "collisions.h"
#include "powerup.h"
#include "menu.h"
#include <stdlib.h> // rand(), srand()
#include <time.h>   // for seeding srand()
#include "character.h"
#include "texture.h"
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
    int GAME_W;
    int GAME_H;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
    int PLAYERS;
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    BackgroundMusic *bgm;
    Character *characters[MAX_PLAYERS];
    Character *hunter;
    TileMap tilemap;
    GameState gameState;
    LimitedVision lv;

    PowerUp powerUps[MAX_POWERUPS];
    int powerUpCount;

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

void initialize_game(Game *game);
void process_input(Game *game);
void update_game(Game *game);
void cleanup_game(Game *game);

int runGame()
{
    Game game;
    initialize_game(&game);
    game.lastFrameTime = SDL_GetPerformanceCounter();

    while (!game.closeWindow)
    {
        process_input(&game);
        update_game(&game);

        game.currentFrameTime = SDL_GetPerformanceCounter();
        game.deltaTime = (game.currentFrameTime - game.lastFrameTime) / (float)SDL_GetPerformanceFrequency();
        game.lastFrameTime = game.currentFrameTime;
    }

    cleanup_game(&game);
    return 0;
}

void initialize_game(Game *game)
{
    srand(time(NULL));
    game->PLAYERS = 5;
    game->speed = 300;
    game->TILE_SIZE = 64;
    game->WINDOW_WIDTH = 1200;
    game->WINDOW_HEIGHT = 800;
    game->pWindow = NULL;
    game->pRenderer = NULL;
    if (init_SDL_window(&game->pWindow, &game->pRenderer, game->WINDOW_WIDTH, game->WINDOW_HEIGHT) != 0)
    {
        printf("Failed to initialize window and renderer.\n");
        exit;
    }

    init_player_sounds();

    char *soundPathbgm[] = {
        "resources/music/bgm1.mp3",
        "resources/music/bgm2.mp3",
        "resources/music/bgm3.mp3",
        "resources/music/bgm4.mp3",
        "resources/music/PEDRO.mp3"};
    // Random background;
    int size_of_soundPathbgm = sizeof(soundPathbgm) / sizeof(soundPathbgm[0]);
    int backgroundIndex = rand() % size_of_soundPathbgm;

    // SUPER BACKGROUD MUSIC
    game->bgm = init_background_music(soundPathbgm[backgroundIndex], 100);

    // game->bgm = init_background_music(soundPathbgm[backgroundIndex], 20);

    play_background_music(game->bgm);
    free_bgm(game->bgm);

    // Setting up tilemap;
    tilemap_init(&game->tilemap, game->pRenderer);
    tilemap_load(&game->tilemap, 1);
    randomize_floor(&game->tilemap, 0);
    orient_walls(&game->tilemap);

    init_LimitedVision(&game->lv, game->pRenderer, &game->tilemap, game->WINDOW_WIDTH, game->WINDOW_HEIGHT, 400);

    game->powerUpCount = 0;
    load_powerup_resources(game->pRenderer);
    init_powerUps(game->pRenderer, &game->tilemap, game->TILE_SIZE);

    const char *characterFiles[] = {
        "resources/characters/warriorTwo.png",
        "resources/characters/femaleOne.png",
        "resources/characters/maleOne.png",
        "resources/characters/warriorOne.png",
        "resources/characters/maleOne.png"};
    const char *hunterClothes[] = {
        "resources/characters/monster.png"};

    // Initate characters
    // Clear just in case
    memset(game->characters, 0, sizeof(game->characters)); // Clear the array first
    for (int i = 0; i < game->PLAYERS; i++)
    {
        game->characters[i] = NULL;
    }

    // ATM if you dont get hunter, you get the same outfit everytime
    int hunterIndex = rand() % game->PLAYERS;
    hunterIndex = 0;
    for (int i = 0; i < game->PLAYERS; i++)
    {
        SDL_Point spawn = get_spawn_point(&game->tilemap, i == hunterIndex);

        if (i == hunterIndex)
        {
            game->characters[i] = init_character(game->pRenderer, hunterClothes[0], 1);
            game->hunter = game->characters[i];
        }
        else
        {
            game->characters[i] = init_character(game->pRenderer, characterFiles[i], 0);
        }

        game->characters[i]->position.x = spawn.x;
        game->characters[i]->position.y = spawn.y;
        update_character_rect(game->characters[i], &game->characters[i]->position);
    }

    game->GAME_W = game->tilemap.width * game->TILE_SIZE;
    game->GAME_H = game->tilemap.height * game->TILE_SIZE;

    // Setting up states
    game->gameState = PAUSED;
    game->closeWindow = false;
    game->m = game->lower_volume = game->inc_volume = false;
    game->space = false;
    game->music = true;
    game->keys.all = 0;
}

void process_input(Game *game)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            game->closeWindow = true;
            break;
        // * this is the movement code from simpleSDLexample1.
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                game->gameState = PAUSED;
                break;
            case SDL_SCANCODE_M:
                game->m = true;
                break;
            case SDL_SCANCODE_SPACE:
                game->keys.space = 1;
                break;
            case SDL_SCANCODE_UP:
                game->keys.up = 1;
                break;
            case SDL_SCANCODE_LEFT:
                game->keys.left = 1;
                break;
            case SDL_SCANCODE_DOWN:
                game->keys.down = 1;
                break;
            case SDL_SCANCODE_RIGHT:
                game->keys.right = 1;
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode)
            {
            // ARROWS
            case SDL_SCANCODE_UP:
                game->keys.up = 0;
                break;
            case SDL_SCANCODE_LEFT:
                game->keys.left = 0;
                break;
            case SDL_SCANCODE_DOWN:
                game->keys.down = 0;
                break;
            case SDL_SCANCODE_RIGHT:
                game->keys.right = 0;
                break;
            }
            break;
        }
    }
    game->characters[0]->velocity.x = (game->keys.left - game->keys.right);
    game->characters[0]->velocity.y = (game->keys.up - game->keys.down);
}

void update_game(Game *game)
{
    switch (game->gameState)
    {
    case PAUSED:
        SDL_RenderClear(game->pRenderer);

        if (mainMenu(game->pRenderer))
            game->gameState = QUIT;
        else
            game->gameState = PLAYING;
        SDL_SetRenderDrawColor(game->pRenderer, 0, 0, 0, 255);
        break;
    case PLAYING:
        SDL_RenderClear(game->pRenderer);
        if (game->keys.space)
        {
            kill_command(game->characters[0], game->characters, game->PLAYERS);
            game->keys.space = 0;
        }
        // Move character
        move_character(game->characters[0], &game->tilemap, game->WINDOW_WIDTH, game->WINDOW_HEIGHT,
                       game->deltaTime, game->characters, game->PLAYERS);
        follow_player(&game->tilemap.camera, &game->characters[0]->rect, game->WINDOW_WIDTH, game->WINDOW_HEIGHT);
        // Draw stage
        tilemap_draw(&game->tilemap);
        draw_powerUps(game->pRenderer, &game->tilemap);
        for (int i = 0; i < game->PLAYERS; i++)
        {
            draw_character(game->pRenderer, game->characters[i], &game->tilemap.camera);
        }

        SDL_FPoint center = get_character_center(game->characters[0]);
        drawLimitedVision(&game->lv, center);
        // Render
        SDL_RenderPresent(game->pRenderer);

        break;
    case QUIT:
        game->closeWindow = true;
        break;
    default:
        break;
    }
}

void cleanup_game(Game *game)
{
    tilemap_free(&game->tilemap);
    cleanup_powerup_resources();
    cleanup_character(game->characters[0]);
    cleanup_player_sounds();
    cleanup_SDL(game->pWindow, game->pRenderer);
}