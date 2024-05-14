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
#include "mazeview.h"
#include "music.h"
#include "limitedvision.h"
#include "powerup.h"
#include "menu.h"

#define NO_SERVER 0

typedef struct game
{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    BackgroundMusic *bgm;
    TileMap tilemap;
    MazeView mazeview; 
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
    char hostAddress[MAX_ADDRESS_LENGTH];

    Character *characters[MAX_PLAYERS];
    Character *myCharacter;

    CharacterData data;
    ServerData serverData;
    JoinData joinData;
    ReadyData oldready;
    ReadyData ready;

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
    game->WINDOW_HEIGHT = 700;
    if (init_SDL_window(&game->pWindow, &game->pRenderer, game->WINDOW_WIDTH, game->WINDOW_HEIGHT) == 1)
    {
        return 1;
    }

    initMenu(game->pRenderer);

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

    
    if (!(game->packet = SDLNet_AllocPacket(512)))
    {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return 1;
    }
    
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
    game->oldready.ready = false;
    game->oldready.start = false;
    game->oldready.quit = false;
    game->ready.ready = false;
    game->ready.start = false;
    game->ready.quit = false;
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
            game->ready.quit = true;
        }
        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_SPACE)
            {
                game->ready.ready = !game->ready.ready;
            }
            if (event.key.keysym.sym == SDLK_RETURN)
            {
                game->ready.start = true;
            }
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                game->ready.quit = true;
            }
        }
    }
    // Send ready to server
    if (game->ready.ready != game->oldready.ready || game->ready.start != game->oldready.start || game->ready.quit != game->oldready.quit)
    {
        if (game->ready.quit)
        {
            DEBUG_PRINT("Quitting\n");
        }
        memcpy(game->packet->data, &game->ready, sizeof(ReadyData) + 1);
        game->packet->len = sizeof(ReadyData) + 1;
        SDLNet_UDP_Send(game->serverSocket, -1, game->packet);
        game->ready.start = false;
    }



    if (SDLNet_UDP_Recv(game->serverSocket, game->packet) == 1)
    {
        DEBUG_PRINT2("(Client) Packet received\n");
        memcpy(&game->joinData, game->packet->data, sizeof(JoinData));
        game->activePlayer = game->joinData.playerINDEX;
        game->seed = game->joinData.seed;
        game->PLAYERS = game->joinData.PLAYERS;
        game->gameState = game->joinData.gameState;
        game->hunterIndex = game->joinData.hunterindex;
       // printJoinData(game->joinData);
        if (game->gameState == PLAYING)
        {
            startGame(game);
            return;
        }
    }
    game->oldready = game->ready;
    SDL_SetRenderDrawColor(game->pRenderer, 0, 0, 0, 255);
    drawLobby(game->pRenderer, game->joinData.readyPlayers, game->joinData.PLAYERS);
    if (game->ready.quit)
    {
        game->gameState = QUIT;
        return;
    }
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
    init_maze_view(&game->mazeview, game->pRenderer, &game->tilemap, game->WINDOW_WIDTH, game->WINDOW_HEIGHT);

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
    int closeRequested=0;
    SDL_Event event;
    game->lastFrameTime = SDL_GetPerformanceCounter();
    while (!closeRequested)
    {
        switch (game->gameState)
        {
        case START:
            if(menu(game->pRenderer, game->hostAddress, false)) return; // This function can get the Host Address from the user in the future
            game->gameState = JOINING; // remove this line when main menu is implemented
            
            if (SDLNet_ResolveHost(&game->serverIP, game->hostAddress, SOCKET_PORT))
            {
                fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                game->gameState = QUIT;
                break;
            }
            game->packet->address.host = game->serverIP.host;
            game->packet->address.port = game->serverIP.port;
            
             // Send initial packet to server to establish connection
            memcpy(game->packet->data, &game->ready, sizeof(ReadyData) + 1);
            game->packet->len = sizeof(ReadyData) + 1;
            SDLNet_UDP_Send(game->serverSocket, -1, game->packet);
            break;
        case JOINING:
            joining(game);
            break;

        case PLAYING:
            playing(game);
            break;

        case PAUSED:
            if(menu(game->pRenderer, game->hostAddress, true))
            {
                game->gameState = QUIT;
            }
            else
            {
                game->gameState = PLAYING;
            }
            break;
        case QUIT:
            quitMenu(game->pRenderer);
            closeRequested=1;
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
            game->data.disconnect = true;
            game->data.iskilled = true;
            memcpy(game->packet->data, &game->data, sizeof(CharacterData));
            game->packet->len = sizeof(CharacterData);
            SDLNet_UDP_Send(game->serverSocket, -1, game->packet);
        }
        handleInput(game, &event);
    }

    updateFromServer(game);

    if (game->myCharacter->isHunter && game->space)
        {
            kill_command(game->myCharacter, game->characters, game->PLAYERS);
        }

    if (game->mazeview.visible) {
        set_volume(0);
        render_maze_view(&game->mazeview, game->pRenderer); 
        if (game->myCharacter && !game->myCharacter->isKilled) {
            draw_character_on_mazeview(game->myCharacter, &game->tilemap, game->WINDOW_WIDTH, game->WINDOW_HEIGHT, &game->mazeview, game->pRenderer);
        }
    } else {
        set_volume(30);

        move_character(game->myCharacter, &game->tilemap, game->deltaTime, game->characters, game->PLAYERS, &game->mazeview);

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
            if (game->characters[i] != game->myCharacter)
                draw_character(game->pRenderer, game->characters[i], i == game->activePlayer, &game->tilemap.camera);
        }
        draw_character(game->pRenderer, game->myCharacter, true, &game->tilemap.camera);
        drawLimitedVision(&game->lv, get_character_center(game->myCharacter), game->tilemap.camera);
    }

    SDL_RenderPresent(game->pRenderer);
}

void updateFromServer(Game_c *game)
{
#if !NO_SERVER
    if (SDLNet_UDP_Recv(game->serverSocket, game->packet) == 1)
    {
        DEBUG_PRINT3("(Client) Packet received\n");
        memcpy(&game->serverData, game->packet->data, sizeof(ServerData));
        game->gameState = game->serverData.gameState;
        for (int i = 0; i < game->PLAYERS; i++)
        {
            if (i != game->activePlayer)
            {
                game->characters[i]->position = game->serverData.characters[i].position;
                game->characters[i]->velocity = game->serverData.characters[i].velocity;
                update_character_rect(game->characters[i], &game->characters[i]->position);
            }
            game->characters[i]->health = game->serverData.characters[i].health;
            game->characters[i]->visible = game->serverData.characters[i].visible;
            game->characters[i]->isKilled = game->serverData.characters[i].iskilled;
            game->characters[i]->currentFrame = game->serverData.characters[i].currentFrame;
            game->characters[i]->speedPowerupTime = game->serverData.characters[i].speedPowerupTime;
            game->characters[i]->invisiblePowerupTime = game->serverData.characters[i].invisiblePowerupTime;
            game->characters[i]->frameLastUpdated = game->serverData.characters[i].frameLastUpdated;
            int receivedPowerupId = game->serverData.characters[i].lastPowerupCollected;
            for (int i = 0; i < powerUpCount; i++) {
                if (powerUps[i].powerupid == receivedPowerupId) {
                    powerUps[i].active = 0;
                    break;
                }
            }
        }
        for (int i = 0; i < game->PLAYERS; i++)
        {
            if (game->serverData.isKilled[i])
            {
                game->characters[i]->isKilled = true;
            }
        }
    }
#endif
}

void updateToServer(Game_c *game)
{
    game->data.playerID = game->activePlayer;
    game->data.iskilled = game->myCharacter->isKilled;
    game->data.isHunter = game->myCharacter->isHunter;
    game->data.visible = game->myCharacter->visible;
    game->data.health = game->myCharacter->health;
    game->data.position = game->myCharacter->position;
    game->data.velocity = game->myCharacter->velocity;
    game->data.currentFrame = game->myCharacter->currentFrame;
    game->data.speedPowerupTime = game->myCharacter->speedPowerupTime;
    game->data.invisiblePowerupTime = game->myCharacter->invisiblePowerupTime;
    game->data.frameLastUpdated = game->myCharacter->frameLastUpdated;
    game->data.gameState = game->gameState;
    game->data.lastPowerupCollected = game->myCharacter->lastPowerupCollected;
    game->data.disconnect = false;
    for (int i = 0; i < game->PLAYERS; i++)
    {
        game->data.hasKilledindex[i] = game->characters[i]->isKilled;
    }
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
        case SDLK_ESCAPE:
            game->gameState = PAUSED;
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
        case SDLK_SPACE:
            game->space = false;
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
    free_maze_view(&game->mazeview); 
    SDLNet_FreePacket(game->packet);
    SDLNet_UDP_Close(game->serverSocket);
    SDL_DestroyRenderer(game->pRenderer);
    SDL_DestroyWindow(game->pWindow);
    SDLNet_Quit();
    SDL_Quit();
}
