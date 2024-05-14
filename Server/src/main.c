#define NO_STDIO_REDIRECT
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <time.h>
#include "debug.h"
#include "our_rand.h"
#include "data.h"
#include "window.h"
#include "tilemap.h"
#include "mazeview.h"
#include "music.h"
#include "powerup.h"

/*
     !IMPORTANT!
*    If the server doesn't start, make sure that the server is not already running in the background. AKA check the task manager to kill it.
*/

#define SHOW_WINDOW 0
typedef struct _game
{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Character *characters[MAX_PLAYERS];
    TileMap tilemap;
    MazeView mazeview; 

    UDPsocket serverSocket;
    UDPpacket *packet;
    int nrOfClients;
    int PLAYERS;
    int seed;
    IPaddress clients[MAX_PLAYERS];
    GameState gameState;

    CharacterData recievedData;
    CharacterData clientsData[MAX_PLAYERS];
    ServerData serverData;
    JoinData joinData;

    bool readyPlayes[MAX_PLAYERS];
} Game_s;

void Add_Client(IPaddress address, IPaddress *clients, int *nrOfClients)
{
    for (int i = 0; i < *nrOfClients; i++)
    {
        if (address.host == clients[i].host && address.port == clients[i].port)
        {
            return;
        }
    }
    clients[*nrOfClients] = address;
    (*nrOfClients)++;
}
void Remove_Client(IPaddress address, IPaddress *clients, int *nrOfClients)
{
    for (int i = 0; i < *nrOfClients; i++)
    {
        if (address.host == clients[i].host && address.port == clients[i].port)
        {
            DEBUG_PRINT2("Removing client %d\n", i);
            for (int j = i; j < *nrOfClients - 1; j++)
            {
                clients[j] = clients[j + 1];
            }
            (*nrOfClients)--;
            return;
        }
    }
    DEBUG_PRINT2("Client not found\n");
}
int init(Game_s *game);
void setupgame(Game_s *game);
void close(Game_s *game);
void playing(Game_s *game);
void run(Game_s *game);
void start(Game_s *game);
int getPlayerIndex(Game_s *game);

int main(int argc, char *argv[])
{
    Game_s game = {0};
    if (init(&game) == 1)
    {
        fprintf(stderr, "Error initializing game\n");
        exit(1);
    }
    run(&game);
    close(&game);
    return 0;
}

int init(Game_s *game)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (SDLNet_Init() == -1)
    {
        fprintf(stderr, "Error initializing SDL_net: %s\n", SDLNet_GetError());
        return 1;
    }
#if SHOW_WINDOW
    game->pWindow = SDL_CreateWindow("Server", 0, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN);
    if (!game->pWindow)
    {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    game->pRenderer = SDL_CreateRenderer(game->pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if (!game->pRenderer)
    {
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(game->pWindow);
        SDL_Quit();
        return 1;
    }
#endif
    if (!(game->serverSocket = SDLNet_UDP_Open(SOCKET_PORT)))
    {
        fprintf(stderr, "Error opening socket: %s\n", SDLNet_GetError());
        SDL_DestroyRenderer(game->pRenderer);
        SDL_DestroyWindow(game->pWindow);
        SDL_Quit();
        return 1;
    }
    if (!(game->packet = SDLNet_AllocPacket(512)))
    {
        fprintf(stderr, "Error allocating packet: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(game->serverSocket);
        SDL_DestroyRenderer(game->pRenderer);
        SDL_DestroyWindow(game->pWindow);
        SDL_Quit();
        return 1;
    }
    tilemap_init(&game->tilemap, game->pRenderer);
    game->gameState = START;
    game->nrOfClients = 0;
    our_srand(time(NULL));
    game->seed = our_rand();
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        game->readyPlayes[i] = false;
    }

    return 0;
}

void run(Game_s *game)
{
    int closeRequest=0;
    while (!closeRequest)
    {
        switch (game->gameState)
        {
        case START:
            start(game);
            break;
        case PLAYING:
            playing(game);
            break;
        case QUIT:
            printf("\n\tPreparing to close\n");
            closeRequest=1;
            break;
        default:
            break;
        }
    }
}
void start(Game_s *game)
{
    static bool start_pressed = false;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            game->gameState = QUIT;
        }
        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
            {
                start_pressed = true;
            }
        }
    }
    // receive data from clients
    if (SDLNet_UDP_Recv(game->serverSocket, game->packet) == 1)
    {
        DEBUG_PRINT3("(Server) Packet received\n");
        Add_Client(game->packet->address, game->clients, &game->nrOfClients);
        ReadyData ready;
        memcpy(&ready, game->packet->data, sizeof(ReadyData));
        DEBUG_PRINT2("quit?: %d\n", ready.quit);
        if (ready.quit)
        {
            DEBUG_PRINT("Client disconnected\n");
            Remove_Client(game->packet->address, game->clients, &game->nrOfClients);
            DEBUG_PRINT("Clients left: %d\n", game->nrOfClients);
            if (game->nrOfClients == 0)
            {
                game->gameState = QUIT;
            }
        }
        else
        {
            for (int i = 0; i < game->nrOfClients; i++)
            {
                if (game->clients[i].host == game->packet->address.host && game->clients[i].port == game->packet->address.port)
                {
                    game->readyPlayes[i] = ready.ready;
                    if (i == 0 && ready.start)
                    {
                        start_pressed = true;
                    }
                }
                game->joinData.readyPlayers[i] = game->readyPlayes[i];
            }
        }
        for (int i = 0; i < game->nrOfClients; i++)
        {
            // printIPaddress of client
            DEBUG_PRINT2("Client %d: %d.%d.%d.%d:%d\n", i, game->clients[i].host >> 24 & 0xFF, game->clients[i].host >> 16 & 0xFF, game->clients[i].host >> 8 & 0xFF, game->clients[i].host & 0xFF, game->clients[i].port);
        }
        
        game->joinData.PLAYERS = game->nrOfClients;
        DEBUG_PRINT("Nr of clients: %d\n", game->joinData.PLAYERS);
        game->joinData.seed = game->seed;
        game->joinData.gameState = JOINING;
        game->packet->len = sizeof(JoinData);
        for (int i = 0; i < game->nrOfClients; i++)
        {
            game->packet->address = game->clients[i];
            game->joinData.playerINDEX = i;

            memcpy(game->packet->data, &game->joinData, sizeof(JoinData));
            SDLNet_UDP_Send(game->serverSocket, -1, game->packet);
        }
        DEBUG_PRINT3("(Server) Sent to clients\n");
    }
    int readyCount = 0;
    for (int i = 0; i < game->nrOfClients; i++)
    {
        readyCount += game->readyPlayes[i];
    }
    if (readyCount == game->nrOfClients && start_pressed && game->nrOfClients > 0)
    {
        game->gameState = PLAYING;
        game->joinData.gameState = PLAYING;
        game->joinData.PLAYERS = game->nrOfClients;
        
        game->joinData.seed = game->seed;
        game->joinData.hunterindex = our_rand() % game->nrOfClients;

        for (int i = 0; i < game->nrOfClients; i++)
        {
            game->joinData.playerINDEX = i;

            game->packet->address = game->clients[i];
            game->packet->len = sizeof(JoinData);
            memcpy(game->packet->data, &game->joinData, sizeof(JoinData));
            SDLNet_UDP_Send(game->serverSocket, -1, game->packet);
        }
        setupgame(game);
    }
    else
    {
        start_pressed = false;
    }
}

void playing(Game_s *game)
{
    static bool left = false, right = false, up = false, down = false, space = false;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            game->gameState = QUIT;
            break;
#if SHOW_WINDOW
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
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
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
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
#endif
        }
    }
#if SHOW_WINDOW
    int moveX = left - right;
    int moveY = down - up;
    game->tilemap.camera.x += moveX * 10;
    game->tilemap.camera.y += moveY * 10;
#endif
    if (SDLNet_UDP_Recv(game->serverSocket, game->packet) == 1)
    {

        memcpy(&game->recievedData, game->packet->data, sizeof(CharacterData));
        int playerIndex = game->recievedData.playerID;
        DEBUG_PRINT3("(Server) Packet received from %d\n", playerIndex);
        game->clientsData[playerIndex] = game->recievedData;
        if (game->recievedData.disconnect)
        {
            DEBUG_PRINT("Player %d disconnected\n", playerIndex);
        }
        for (int i = 0; i < game->nrOfClients; i++)
        {
#if SHOW_WINDOW
            game->characters[i]->position = game->clientsData[i].position;
            game->characters[i]->velocity = game->clientsData[i].velocity;
            game->characters[i]->health = game->clientsData[i].health;
            game->characters[i]->visible = game->clientsData[i].visible;
            game->characters[i]->isKilled = game->clientsData[i].iskilled;
            game->characters[i]->currentFrame = game->clientsData[i].currentFrame;
            game->characters[i]->speedPowerupTime = game->clientsData[i].speedPowerupTime;
            game->characters[i]->invisiblePowerupTime = game->clientsData[i].invisiblePowerupTime;
            game->characters[i]->frameLastUpdated = game->clientsData[i].frameLastUpdated;
            game->characters[i]->isMoving = game->clientsData[i].isMoving;
            update_character_rect(game->characters[i], &game->characters[i]->position);
#endif
            game->serverData.characters[i] = game->clientsData[i];
            if (game->clientsData[i].isHunter)
            {
                memcpy(&game->serverData.isKilled, &game->clientsData[i].hasKilledindex, sizeof(bool) * MAX_PLAYERS);
            }
        }

        for (int i = 0; i < game->nrOfClients; i++)
        {
            game->packet->address = game->clients[i];
            memcpy(game->packet->data, &game->serverData, sizeof(ServerData));
            game->packet->len = sizeof(ServerData);
            SDLNet_UDP_Send(game->serverSocket, -1, game->packet);
        }
        DEBUG_PRINT3("(Server) Relay to client\n");
    }
    game->PLAYERS = game->nrOfClients;
    for (int i = 0; i < game->nrOfClients; i++)
    {
        if (game->clientsData[i].disconnect)
        {
            game->PLAYERS--;
        }
    }
    // DEBUG_PRINT2("PLAYERS: %d\n", game->PLAYERS);
    if (game->PLAYERS == 0)
    {
        game->gameState = QUIT;
    }

#if SHOW_WINDOW
    SDL_SetRenderDrawColor(game->pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(game->pRenderer);

    tilemap_draw(&game->tilemap);

    for (int i = 0; i < game->nrOfClients; i++)
    {
        draw_character(game->pRenderer, game->characters[i], true, &game->tilemap.camera);
    }

    SDL_RenderPresent(game->pRenderer);
#endif
}
void setupgame(Game_s *game)
{
    our_srand(game->seed);
#if SHOW_WINDOW
    tilemap_load(&game->tilemap, 2, game->seed);
    init_powerUps(game->pRenderer, &game->tilemap, game->tilemap.tile_size);

    const char *characterFiles[] = {
        "../lib/assets/characters/warriorTwo.png",
        "../lib/assets/characters/femaleOne.png",
        "../lib/assets/characters/maleOne.png",
        "../lib/assets/characters/warriorOne.png",
        "../lib/assets/characters/maleOne.png"};
    const char *hunterClothes[] = {
        "../lib/assets/characters/monster.png"};

    int hunterIndex = game->joinData.hunterindex;
    Character characters[MAX_PLAYERS];
    for (int i = 0; i < game->nrOfClients; i++)
    {
        SDL_Point spawn = get_spawn_point(&game->tilemap, i == hunterIndex);

        if (i == hunterIndex)
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
    DEBUG_PRINT("Game setup\n");
#endif
}

int getPlayerIndex(Game_s *game)
{
    for (int i = 0; i < game->nrOfClients; i++)
    {
        if (game->clients[i].host == game->packet->address.host && game->clients[i].port == game->packet->address.port)
        {
            return i;
        }
    }
    return -1;
}

void close(Game_s *game)
{
    DEBUG_PRINT("Cleaning up\n");
    cleanup_powerup_resources();
    tilemap_free(&game->tilemap);
    SDLNet_FreePacket(game->packet);
    SDLNet_UDP_Close(game->serverSocket);
    #if SHOW_WINDOW
    for (int i = 0; i < game->nrOfClients; i++)
    {
        cleanup_character(game->characters[i]);
    }
    SDL_DestroyWindow(game->pWindow);
    SDL_DestroyRenderer(game->pRenderer);
    #endif
    SDLNet_Quit();
    SDL_Quit();
}