#define NO_STDIO_REDIRECT
#include "debug.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <time.h>
#include "our_rand.h"
#include "data.h"
#include "window.h"
#include "character.h"
#include "tilemap.h"
#include "music.h"
#include "limitedvision.h"
#include "powerup.h"
#include "menu.h"

#define NO_SERVER 1

typedef struct game
{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    BackgroundMusic *bgm;
    TileMap tilemap;
    LimitedVision lv;
    int activePlayer; // get from the server initally
    int seed;         // get from the server initally
    int PLAYERS;
    int hunterIndex;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;

    UDPsocket serverSocket;
    IPaddress serverIP;
    UDPpacket *packet;

    Character *characters[MAX_PLAYERS];
    Character *myCharacter;

    CharacterData data;
    ServerData serverData;
    JoinData joinData;
    bool oldready;
    bool ready;
    PowerUp powerUps[MAX_POWERUPS];
    int powerUpCount;
    GameState gameState;
    Uint64 lastFrameTime;
    Uint64 currentFrameTime;
    float deltaTime;
    bool space, music, m, lower_volume, inc_volume;

    SDL_FPoint lastPos;
} Game_c;

int initiate(Game_c *game);
void joining(Game_c *game);
void run(Game_c *game);
void close(Game_c *game);
void handleInput(Game_c *game, SDL_Event *event);
void startGame(Game_c *game);
void playing(Game_c *game);
void updateFromServer(Game_c *game);
void updateToServer(Game_c *game);

int main(int argc, char *argv[])
{
    Game_c game = {0};
    if (initiate(&game) == 1)
    {
        return 1;
    }
    run(&game);
    close(&game);
    return 0;
}

int initiate(Game_c *game)
{
    game->WINDOW_WIDTH = 1200;
    game->WINDOW_HEIGHT = 800;
    if (init_SDL_window(&game->pWindow, &game->pRenderer, game->WINDOW_WIDTH, game->WINDOW_HEIGHT) == 1)
    {
        return 1;
    }
    if (SDLNet_Init() == -1)
    {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        return 1;
    }
    if (!(game->serverSocket = SDLNet_UDP_Open(0)))
    {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return 1;
    }

    ///! When the menu is implemented, the server IP will be taken from the user, change this if statement to a function that gets the IP from the user
    if (SDLNet_ResolveHost(&game->serverIP, "127.0.0.1", SOCKET_PORT))
    {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return 1;
    }
    if (!(game->packet = SDLNet_AllocPacket(512)))
    {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return 1;
    }
    game->packet->address.host = game->serverIP.host;
    game->packet->address.port = game->serverIP.port;
    init_player_sounds();

    char *soundPathbgm[] = {
        "../lib/assets/music/bgm1.mp3",
        "../lib/assets/music/bgm2.mp3",
        "../lib/assets/music/bgm3.mp3",
        "../lib/assets/music/bgm4.mp3",
        "../lib/assets/music/PEDRO.mp3"};
    int size_of_soundPathbgm = sizeof(soundPathbgm) / sizeof(soundPathbgm[0]);
    int backgroundIndex = our_rand() % size_of_soundPathbgm;

    game->bgm = init_background_music(soundPathbgm[backgroundIndex], 30);
    tilemap_init(&game->tilemap, game->pRenderer);

    game->powerUpCount = 0;
    load_powerup_resources(game->pRenderer);
    init_LimitedVision(&game->lv, game->pRenderer, &game->tilemap, game->WINDOW_WIDTH, game->WINDOW_HEIGHT, 400);
    game->gameState = START;
    game->ready = false;
    game->oldready = false;
#if NO_SERVER

    game->gameState = PLAYING;
    game->activePlayer = 0;
    game->seed = 0;
    game->PLAYERS = 2;
    game->hunterIndex = 0;
    startGame(game);

#endif
    return 0;
}

void joining(Game_c *game)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            game->gameState = QUIT;
        }
        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_SPACE)
            {
                game->ready = !game->ready;
            }
        }
    }
    // Send ready to server
    if (game->ready != game->oldready)
    {
        memcpy(game->packet->data, &game->ready, sizeof(bool) + 1);
        game->packet->len = sizeof(bool) + 1;
        SDLNet_UDP_Send(game->serverSocket, -1, game->packet);
    }
    // Receive server data
    if (SDLNet_UDP_Recv(game->serverSocket, game->packet) == 1)
    {
        DEBUG_PRINT2("(Client) Packet received\n");
        memcpy(&game->joinData, game->packet->data, sizeof(JoinData));
        game->activePlayer = game->joinData.playerINDEX;
        game->seed = game->joinData.seed;
        game->PLAYERS = game->joinData.PLAYERS;
        game->gameState = game->joinData.gameState;
        game->hunterIndex = game->joinData.hunterindex;
        printJoinData(game->joinData);
        if (game->gameState == PLAYING)
        {
            startGame(game);
            return;
        }
    }
    game->oldready = game->ready;
    SDL_SetRenderDrawColor(game->pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(game->pRenderer);
    for (int i = 0; i < game->PLAYERS; i++)
    {
        SDL_Rect rect = {10 + i * 60, 0, 50, 50};
        if (game->joinData.readyPlayers[i])
        {
            SDL_SetRenderDrawColor(game->pRenderer, 0, 255, 0, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(game->pRenderer, 255, 0, 0, 255);
        }
        SDL_RenderFillRect(game->pRenderer, &rect);
        SDL_SetRenderDrawColor(game->pRenderer, 0, 0, 0, 255);
    }
    SDL_RenderPresent(game->pRenderer);
}

void startGame(Game_c *game)
{
    const char *characterFiles[] = {
        "../lib/assets/characters/warriorTwo.png",
        "../lib/assets/characters/femaleOne.png",
        "../lib/assets/characters/maleOne.png",
        "../lib/assets/characters/warriorOne.png",
        "../lib/assets/characters/maleOne.png"};
    const char *hunterClothes[] = {
        "../lib/assets/characters/monster.png"};
    DEBUG_PRINT("Seed: %d\n", game->seed);
    our_srand(game->seed);
    tilemap_load(&game->tilemap, 2, game->seed);
    init_powerUps(game->pRenderer, &game->tilemap, game->tilemap.tile_size);

    play_background_music(game->bgm);
    free_bgm(game->bgm);

    for (int i = 0; i < game->PLAYERS; i++)
    {
        SDL_Point spawn = get_spawn_point(&game->tilemap, i == game->hunterIndex);
        if (i == game->hunterIndex)
        {
            game->characters[i] = init_character(game->pRenderer, hunterClothes[0], 1);
        }
        else
        {
            game->characters[i] = init_character(game->pRenderer, characterFiles[i], 0);
        }
        game->characters[i]->position.x = spawn.x;
        game->characters[i]->position.y = spawn.y;
        update_character_rect(game->characters[i], &game->characters[i]->position);
    }
    game->myCharacter = game->characters[game->activePlayer];
    game->lastPos = game->myCharacter->position;

#if !NO_SERVER
    updateToServer(game);
#endif
}
// MARK: - Game Loop
void run(Game_c *game)
{
    SDL_Event event;
    game->lastFrameTime = SDL_GetPerformanceCounter();
    while (game->gameState != QUIT)
    {
        switch (game->gameState)
        {
        case START:
            // main menu function goes here

            game->gameState = JOINING; // remove this line when main menu is implemented
            break;

        case JOINING:
            joining(game);
            break;

        case PLAYING:
            playing(game);
            break;

        case PAUSED:
            // pause menu? function goes here if we would like to have it
            break;
        case QUIT:
            break;
        }
        game->currentFrameTime = SDL_GetPerformanceCounter();
        game->deltaTime = (float)((game->currentFrameTime - game->lastFrameTime) / (float)SDL_GetPerformanceFrequency());
        game->lastFrameTime = game->currentFrameTime;
    }
}

void playing(Game_c *game)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            game->gameState = QUIT;
        }
        handleInput(game, &event);
    }

    updateFromServer(game);

    /// TODO: Implement kill command functionallity with server
    if (game->myCharacter->isHunter && game->space)
    {
        kill_command(game->myCharacter, game->characters, game->PLAYERS);
    }

    move_character(game->myCharacter, &game->tilemap,
                   game->deltaTime, game->characters, game->PLAYERS);

    follow_player(&game->tilemap.camera, &game->myCharacter->rect, game->WINDOW_WIDTH, game->WINDOW_HEIGHT);
    if (fabsf(game->myCharacter->position.x - game->lastPos.x) > 0.1f || fabsf(game->myCharacter->position.y - game->lastPos.y) > 0.1f)
    {
        updateToServer(game);
        game->lastPos = game->myCharacter->position;
    }
    // draw stage
    SDL_SetRenderDrawColor(game->pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(game->pRenderer);
    tilemap_draw(&game->tilemap);
    draw_powerUps(game->pRenderer, &game->tilemap);

    for (int i = 0; i < game->PLAYERS; i++)
    {
        draw_character(game->pRenderer, game->characters[i], &game->tilemap.camera);
    }
    // drawLimitedVision(&game->lv, get_character_center(game->myCharacter), game->tilemap.camera);

    SDL_RenderPresent(game->pRenderer);
}

void updateFromServer(Game_c *game)
{
#if !NO_SERVER
    if (SDLNet_UDP_Recv(game->serverSocket, game->packet) == 1)
    {
        DEBUG_PRINT3("(Client) Packet received\n");
        memcpy(&game->serverData, game->packet->data, sizeof(ServerData));
        for (int i = 0; i < game->PLAYERS; i++)
        {
            if (i == game->activePlayer)
            {
                continue;
            }
            game->characters[i]->position = game->serverData.characters[i].position;
            game->characters[i]->velocity = game->serverData.characters[i].velocity;
            game->characters[i]->health = game->serverData.characters[i].health;
            game->characters[i]->visible = game->serverData.characters[i].visible;
            game->characters[i]->isKilled = game->serverData.characters[i].iskilled;
            game->characters[i]->currentFrame = game->serverData.characters[i].currentFrame;
            game->characters[i]->speedPowerupTime = game->serverData.characters[i].speedPowerupTime;
            game->characters[i]->invisiblePowerupTime = game->serverData.characters[i].invisiblePowerupTime;
            game->characters[i]->frameLastUpdated = game->serverData.characters[i].frameLastUpdated;
            update_character_rect(game->characters[i], &game->characters[i]->position);
        }
    }
#endif
}

void updateToServer(Game_c *game)
{
    game->data.playerID = game->activePlayer;
    game->data.iskilled = game->myCharacter->isKilled;
    game->data.visible = game->myCharacter->visible;
    game->data.health = game->myCharacter->health;
    game->data.position = game->myCharacter->position;
    game->data.velocity = game->myCharacter->velocity;
    game->data.currentFrame = game->myCharacter->currentFrame;
    game->data.speedPowerupTime = game->myCharacter->speedPowerupTime;
    game->data.invisiblePowerupTime = game->myCharacter->invisiblePowerupTime;
    game->data.frameLastUpdated = game->myCharacter->frameLastUpdated;
    game->data.gameState = game->gameState;

#if !NO_SERVER
    memcpy(game->packet->data, &game->data, sizeof(CharacterData));
    game->packet->len = sizeof(CharacterData);
    DEBUG_PRINT3("(Client) Sending packet\n");
    SDLNet_UDP_Send(game->serverSocket, -1, game->packet);
#endif
}

void handleInput(Game_c *game, SDL_Event *event)
{
    static bool up = false, down = false, left = false, right = false;
    switch (event->type)
    {
    case SDL_KEYDOWN:
        switch (event->key.keysym.sym)
        {
        case SDLK_UP:
        case SDLK_w:
            up = true;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            down = true;
            break;
        case SDLK_LEFT:
        case SDLK_a:
            left = true;
            break;
        case SDLK_RIGHT:
        case SDLK_d:
            right = true;
            break;
        case SDLK_SPACE:
            game->space = true;
            break;
        }
        break;
    case SDL_KEYUP:
        switch (event->key.keysym.sym)
        {
        case SDLK_UP:
        case SDLK_w:
            up = false;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            down = false;
            break;
        case SDLK_LEFT:
        case SDLK_a:
            left = false;
            break;
        case SDLK_RIGHT:
        case SDLK_d:
            right = false;
            break;
        }
        break;
    }
    game->myCharacter->velocity.x = (right - left);
    game->myCharacter->velocity.y = (down - up);
}

void close(Game_c *game)
{
    cleanup_powerup_resources();
    for (int i = 0; i < game->PLAYERS; i++)
    {
        cleanup_character(game->characters[i]);
    }
    tilemap_free(&game->tilemap);
    SDLNet_FreePacket(game->packet);
    SDLNet_UDP_Close(game->serverSocket);
    SDL_DestroyRenderer(game->pRenderer);
    SDL_DestroyWindow(game->pWindow);
    SDLNet_Quit();
    SDL_Quit();
}