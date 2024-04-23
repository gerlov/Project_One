// #include "./inc/server_game.h"
#define NO_STDIO_REDIRECT
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <time.h>
#include "our_rand.h"
#include "data.h"
#include "window.h"
#include "tilemap.h"
#include "music.h"
#include "powerup.h"

/* 
     !IMPORTANT!
*    If the server doesn't start, make sure that the server is not already running in the background. AKA check the task manager to kill it.
*/



// #include "../lib/include/game.h"

typedef struct _game
{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Character *characters[MAX_PLAYERS];
    TileMap tilemap;

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

void add(IPaddress address, IPaddress *clients, int *nrOfClients)
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
void init(Game_s *game);
void setupgame(Game_s *game);
void close(Game_s *game);
void playing(Game_s *game);
void run(Game_s *game);
void start(Game_s *game);
int getPlayerIndex(Game_s *game);


int main(int argc, char *argv[])
{
    Game_s game = {0};
    init(&game);
    run(&game);
    close(&game);
    return 0;
}

void init(Game_s *game)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        exit(1);
    }

    if (SDLNet_Init() == -1)
    {
        printf("Error initializing SDL_net: %s\n", SDLNet_GetError());
        exit(1);
    }

    game->pWindow = SDL_CreateWindow("Server", 0, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!game->pWindow)
    {
        fprintf(stderr,"Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    game->pRenderer = SDL_CreateRenderer(game->pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if (!game->pRenderer)
    {
        fprintf(stderr,"Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(game->pWindow);
        SDL_Quit();
        exit(1);
    }
    if (!(game->serverSocket = SDLNet_UDP_Open(2000)))
    {
        fprintf(stderr,"Error opening socket: %s\n", SDLNet_GetError());
        SDL_DestroyRenderer(game->pRenderer);
        SDL_DestroyWindow(game->pWindow);
        SDL_Quit();
        exit(1);
    }
    if (!(game->packet = SDLNet_AllocPacket(512)))
    {
        fprintf(stderr,"Error allocating packet: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(game->serverSocket);
        SDL_DestroyRenderer(game->pRenderer);
        SDL_DestroyWindow(game->pWindow);
        SDL_Quit();
        exit(1);
    }
    tilemap_init(&game->tilemap, game->pRenderer);
    game->gameState = START;
    game->nrOfClients = 0;
    our_srand(time(NULL));
    // game->seed = our_rand();
    game->seed = 0;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        game->readyPlayes[i] = false;
    }
}


void run(Game_s *game)
{
    
    while (game->gameState != QUIT)
    {
        switch (game->gameState)
        {
        case START:
            start(game);
            break;
        case PLAYING:
            playing(game);
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
        printf("(Server) Packet received\n");
        add(game->packet->address, game->clients, &game->nrOfClients);
        bool ready;
        memcpy(&ready, game->packet->data, sizeof(bool));
        for (int i = 0; i < game->nrOfClients; i++)
        {
            if (game->clients[i].host == game->packet->address.host && game->clients[i].port == game->packet->address.port)
            {
                game->readyPlayes[i] = ready;
            }
            game->joinData.readyPlayers[i] = game->readyPlayes[i];
        }
        
        game->joinData.PLAYERS = game->nrOfClients;
        game->joinData.seed = game->seed;
        game->joinData.gameState = START;

        game->packet->len = sizeof(JoinData);
        for (int i = 0; i < game->nrOfClients; i++)
        {
            game->packet->address = game->clients[i];
            game->joinData.playerINDEX = i;
            
            memcpy(game->packet->data, &game->joinData, sizeof(JoinData));
            SDLNet_UDP_Send(game->serverSocket, -1, game->packet);
            printf("(Server) Sent to client %d\n", i);
        }
    }
    if (game->nrOfClients > 0)
    {
        int readyCount = 0;
        for (int i = 0; i < game->nrOfClients; i++)
        {
            readyCount += game->readyPlayes[i];
        }
        if (readyCount == game->nrOfClients && start_pressed)
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
    }
}

void playing(Game_s *game) {
    static bool left = false, right = false, up = false, down = false, space = false;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            game->gameState = QUIT;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.scancode == SDL_SCANCODE_D)
            {
                left = true;
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_A)
            {
                right = true;
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_W)
            {
                up = true;
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_S)
            {
                down = true;
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
            {
                space = true;
            }
        }
        else if (event.type == SDL_KEYUP) {
            if (event.key.keysym.scancode == SDL_SCANCODE_D)
            {
                left = false;
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_A)
            {
                right = false;
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_W)
            {
                up = false;
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_S)
            {
                down = false;
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
            {
                space = false;
            }
        }
    }
    int moveX = left- right;
    int moveY = up - down;

    game->tilemap.camera.x += moveX * 10;
    game->tilemap.camera.y += moveY * 10;
    //
    if (SDLNet_UDP_Recv(game->serverSocket, game->packet) == 1)
    {
        // int playerIndex = getPlayerIndex(game);
        // if (playerIndex == -1)
        // {
        //     return;
        // }
        memcpy(&game->recievedData, game->packet->data, sizeof(CharacterData));
        int playerIndex = game->recievedData.playerID;
        printf("(Server) Packet received from %d\n", playerIndex);
        game->clientsData[playerIndex] = game->recievedData;
        for (int i = 0; i < game->nrOfClients; i++)
        {   
            game->characters[i]->position = game->clientsData[i].position;
            game->characters[i]->velocity = game->clientsData[i].velocity;
            game->characters[i]->health = game->clientsData[i].health;
            game->characters[i]->visible = game->clientsData[i].visible;
            game->characters[i]->isKilled = game->clientsData[i].iskilled;
            game->characters[i]->currentFrame = game->clientsData[i].currentFrame;
            game->characters[i]->speedPowerupTime = game->clientsData[i].speedPowerupTime;
            game->characters[i]->invisiblePowerupTime = game->clientsData[i].invisiblePowerupTime;
            game->characters[i]->frameLastUpdated = game->clientsData[i].frameLastUpdated;
            update_character_rect(game->characters[i], &game->characters[i]->position);
            game->serverData.characters[i] = game->clientsData[i];
        }
        for (int i = 0; i < game->nrOfClients; i++)
        {
            game->packet->address = game->clients[i];
            memcpy(game->packet->data, &game->serverData, sizeof(ServerData));
            game->packet->len = sizeof(ServerData);
            printf("(Server) Relay to client %d\n", i);
            SDLNet_UDP_Send(game->serverSocket, -1, game->packet);
        }
        // printf("(Server) Relay to clients\n");
    }

    SDL_SetRenderDrawColor(game->pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(game->pRenderer);

    tilemap_draw(&game->tilemap);

    for (int i = 0; i < game->nrOfClients; i++)
    {
        draw_character(game->pRenderer, game->characters[i], &game->tilemap.camera);
    }
    
    SDL_RenderPresent(game->pRenderer);

}
void setupgame(Game_s *game)
{
    our_srand(game->seed);
    tilemap_load(&game->tilemap, 1, game->seed);
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

    printf("Game started\n");

}

int getPlayerIndex(Game_s *game) {
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
    SDLNet_FreePacket(game->packet);
    SDLNet_UDP_Close(game->serverSocket);
    SDL_DestroyRenderer(game->pRenderer);
    SDL_DestroyWindow(game->pWindow);
    SDLNet_Quit();
    SDL_Quit();
}