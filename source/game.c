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
typedef enum {
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
    bool up, down, left, right;
    bool w, a, s, d;
    bool closeWindow;

} Game;

void initialize_game(Game *game);
void process_input(Game *game);
void update_game(Game *game);
void cleanup_game(Game *game);

int runGame()
{
    Game game;
    initialize_game(&game);

    while (!game.closeWindow)
    {
        process_input(&game);
        update_game(&game);
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
        "resources/music/PEDRO.mp3"
    };
    //Random background;
    int size_of_soundPathbgm = sizeof(soundPathbgm) / sizeof(soundPathbgm[0]);
    int backgroundIndex = rand() % size_of_soundPathbgm;

    //SUPER BACKGROUD MUSIC
    game->bgm = init_background_music(soundPathbgm[backgroundIndex], 100);


    // game->bgm = init_background_music(soundPathbgm[backgroundIndex], 20);
  
    play_background_music(game->bgm);
    free_bgm(game->bgm);

    // Setting up tilemap;
    tilemap_init(&game->tilemap, game->pRenderer);
    tilemap_load(&game->tilemap, 2);        // 2 = random map
    randomize_floor(&game->tilemap, 0);
    orient_walls(&game->tilemap);


    init_LimitedVision(&game->lv, game->pRenderer, &game->tilemap, game->WINDOW_WIDTH, game->WINDOW_HEIGHT,400);

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
        "resources/characters/monster.png"
    };
        
    // Initate characters
    //Clear just in case
    memset(game->characters, 0, sizeof(game->characters)); // Clear the array first
    for(int i = 0; i < game->PLAYERS; i++){
        game->characters[i] = NULL; 
    }


    //ATM if you dont get hunter, you get the same outfit everytime
    int hunterIndex = rand() % game->PLAYERS;
    hunterIndex = 0; 
    for (int i = 0; i < game->PLAYERS; i++)
    {
        if(i == hunterIndex){
            game->characters[i] = init_character(game->pRenderer, hunterClothes[0], 1);
            game->hunter = game->characters[i];
        }else{
            game->characters[i] = init_character(game->pRenderer, characterFiles[i], 0);
        }
        
        SDL_Point spawn = get_spawn_point(&game->tilemap, game->characters[i]->isHunter);
        game->characters[i]->rect.x = spawn.x;
        game->characters[i]->rect.y = spawn.y;
    }

        
    game->GAME_W = game->tilemap.width * game->TILE_SIZE;
    game->GAME_H = game->tilemap.height * game->TILE_SIZE;

    // Setting up states
    game->gameState = PAUSED;
    game->closeWindow = false;
    game->m = game->lower_volume = game->inc_volume = false;
    game->w = game->a = game->s = game->d = game->up = game->down = game->left = game->right = false;
    game->space = false;
    game->music = true;
    
}

void process_input(Game *game)
{
    SDL_Event event;
    SDL_RenderClear(game->pRenderer);
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
                game->space = true;
                break;
            case SDL_SCANCODE_UP:
                game->up = true;
                set_direction(game->characters[0], 'u');
                break;
            case SDL_SCANCODE_LEFT:
                game->left = true;
                set_direction(game->characters[0], 'l');
                break;
            case SDL_SCANCODE_DOWN:
                game->down = true;
                set_direction(game->characters[0], 'd');
                break;
            case SDL_SCANCODE_RIGHT:
                game->right = true;
                set_direction(game->characters[0], 'r');
                break;

                // WASD
                //  case SDL_SCANCODE_W:
                //      game->w = true;
                //      set_direction(game->characters[0], 'u');
                //      break;
                //  case SDL_SCANCODE_A:
                //      game->a = true;
                //      set_direction(game->characters[0], 'l');
                //      break;
                //  case SDL_SCANCODE_S:
                //      game->s = true;
                //      set_direction(game->characters[0], 'd');
                //      break;
                //  case SDL_SCANCODE_D:
                //      game->d = true;
                //      set_direction(game->characters[0], 'r');
                //      break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode)
            {
            // ARROWS
            case SDL_SCANCODE_UP:
                game->up = false;
                stop_moving(game->characters[0]);
                break;
            case SDL_SCANCODE_LEFT:
                game->left = false;
                stop_moving(game->characters[0]);
                break;
            case SDL_SCANCODE_DOWN:
                game->down = false;
                stop_moving(game->characters[0]);
                break;
            case SDL_SCANCODE_RIGHT:
                game->right = false;
                stop_moving(game->characters[0]);
                break;

                // WASD
                //  case SDL_SCANCODE_W:
                //      game->w = false;
                //      break;
                //  case SDL_SCANCODE_A:
                //      game->a = false;
                //      break;
                //  case SDL_SCANCODE_S:
                //      game->s = false;
                //      break;
                //  case SDL_SCANCODE_D:
                //      game->d = false;
                //      break;
            }
            break;
        }
    }
}

void update_game(Game *game)
{
    switch (game->gameState)
    {
    case PAUSED:
        SDL_RenderClear(game->pRenderer);
        game->down = false;
        game->up = false;
        game->left = false;
        game->right = false;

        if (mainMenu(game->pRenderer))
            game->gameState = QUIT;
        else
            game->gameState = PLAYING;
            SDL_SetRenderDrawColor(game->pRenderer, 0, 0, 0, 255);
        break;
    case PLAYING:
        if (game->space)
        {
            kill_command(game->characters[0], game->characters, game->PLAYERS);
            game->space = false;
        }
        // Move character
        move_character(game->characters[0], &game->tilemap,
                       game->up, game->down, game->left, game->right, 
                       game->characters, game->PLAYERS);
        follow_player(&game->tilemap.camera, &game->characters[0]->rect, game->WINDOW_WIDTH, game->WINDOW_HEIGHT);
        // Draw stage
        tilemap_draw(&game->tilemap);
        draw_powerUps(game->pRenderer, &game->tilemap);
        for (int i = 0; i < game->PLAYERS; i++)
        {
            draw_character(game->pRenderer, game->characters[i], &game->tilemap.camera);
        }
        

        SDL_FPoint center = {game->characters[0]->rect.x + game->characters[0]->rect.w / 2 , game->characters[0]->rect.y + game->characters[0]->rect.h / 2 };
        drawLimitedVision(&game->lv, center);
        // Render
        SDL_RenderPresent(game->pRenderer);
        SDL_Delay(1000 / 120); // 60 frames/s
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