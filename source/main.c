#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "tilemap.h"
#include "music.h" 

#define SPEED 300
#define TILE_SIZE 64
#define TILE_W_AMOUNT 60
#define TILE_H_AMOUNT 60
#define GAME_W TILE_W_AMOUNT*TILE_SIZE
#define GAME_H TILE_H_AMOUNT*TILE_SIZE
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
SDL_Window* pWindow = NULL;
SDL_Renderer* pRenderer = NULL;

int init_SDL_window(void){
    if(SDL_Init(SDL_INIT_VIDEO)!=0){
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }

    pWindow = SDL_CreateWindow(NULL,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,0);
    if(!pWindow){
        printf("Error: %s\n",SDL_GetError());
        SDL_Quit();
        return 1;
    }
    pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!pRenderer){
        printf("Error: %s\n",SDL_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;    
    }
    return 0;
}
void create_texture(SDL_Texture** texture, const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if(!surface)
    {
        printf("Error: %s\n",SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return;
    }
    *texture = SDL_CreateTextureFromSurface(pRenderer, surface);
    SDL_FreeSurface(surface);
    if(!*texture)
    {
        printf("Error: %s\n",SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return;
    }
}

SDL_Rect follow_camera(SDL_Rect* camera, SDL_Rect* target)
{
    SDL_Rect new_camera = *target;
    new_camera.x -= camera->x;
    new_camera.y -= camera->y;
    return new_camera;
}


int main(int argv, char** args){
    if(init_SDL_window() != 0){
        printf("Error: %s\n",SDL_GetError());
    }

    init_music();

    SDL_Rect shipRect;
    shipRect.x = 100;
    shipRect.y = 100;

    // TODO: Move this to a function, maybe in a separate file
    SDL_Texture* pWhite = NULL;
    SDL_Texture* pBlack = NULL;
    SDL_Texture* pTexture = NULL;
    SDL_Texture* pVingette = NULL;

    create_texture(&pBlack, "resources/black.png");
    create_texture(&pWhite, "resources/white.png");
    create_texture(&pTexture, "resources/ship.png");
    SDL_QueryTexture(pTexture, NULL, NULL, &shipRect.w, &shipRect.h);
    shipRect.w /= 4;
    shipRect.h /= 4;

    // * psudo vingette effect
    create_texture(&pVingette, "resources/vingette.png");
    SDL_Rect vingetteRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    SDL_QueryTexture(pVingette, NULL, NULL, &vingetteRect.w, &vingetteRect.h);


    TileMap tilemap;
    SDL_Rect rect = { 0, 0, TILE_SIZE, TILE_SIZE };
    tilemap_init(&tilemap, TILE_W_AMOUNT, TILE_H_AMOUNT, TILE_SIZE);
    Tile white_tile = { 0,0, pWhite, rect };
    Tile black_tile = { 1,0, pBlack, rect };
    // TODO: implement a LoadFromFile function
    for(int y = 0; y < tilemap.height; y++)
    {
        for(int x = 0; x < tilemap.width; x++)
        {
            if((x + y) % 2 == 0)
            {
                tilemap_set_tile(&tilemap, x, y, &white_tile);
            }
            else
            {
                tilemap_set_tile(&tilemap, x, y, &black_tile);
            }
        }
    }

    bool closeWindow = false;
    bool music = true;
    bool up,down,left,right, space, m;
    up = down = left = right = space = m = false;
    // camera is centered on the player
    SDL_Rect camera = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    camera.x = (shipRect.x + shipRect.w / 2) + WINDOW_WIDTH / 2;
    camera.y = (shipRect.y + shipRect.h / 2) + WINDOW_HEIGHT / 2;

    while(!closeWindow){

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    closeWindow = true;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_M:
                            m = true;
                            break;
                        case SDL_SCANCODE_SPACE:
                            space = true;
                            break;
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                            up=true;
                            break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            left=true;
                            break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                            down=true;
                            break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            right=true;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                            up=false;
                        break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            left=false;
                        break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                            down=false;
                        break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            right=false;
                        break;
                    }
                    break;
            }
        }
        if(up && !down)
        {
            shipRect.y -= SPEED / 60;
        }
        if(down && !up)
        {
            shipRect.y += SPEED / 60;
        }
        if(left && !right)
        {
            shipRect.x -= SPEED / 60;
        }
        if(right && !left)
        {
            shipRect.x += SPEED / 60;
        }
        if(space){
            play_sound_once();
            space = false;
        }

        if (m) {
            toggle_music();
            m = false;
        }

        camera.x = (shipRect.x + shipRect.w / 2) - WINDOW_WIDTH / 2;
        camera.y = (shipRect.y + shipRect.h / 2) - WINDOW_HEIGHT / 2;
        // this preserves the ship cordinates while keeping the camera centered on the ship
        SDL_Rect shipdest = follow_camera(&camera, &shipRect);
        // SDL_Rect none = { 0,0,0,0 };

        SDL_RenderClear(pRenderer);
        tilemap_draw(&tilemap, pRenderer, &camera);
        SDL_RenderCopy(pRenderer, pTexture, NULL, &shipdest);
        // * psudo vingette effect
        // SDL_RenderCopy(pRenderer, pVingette, NULL, &vingetteRect);

        SDL_RenderPresent(pRenderer);
        SDL_Delay(1000/120);//60 frames/s
    }

    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyTexture(pTexture);
    SDL_DestroyTexture(pVingette);
    tilemap_free(&tilemap);
    SDL_DestroyWindow(pWindow);

    SDL_Quit();
    return 0;
}