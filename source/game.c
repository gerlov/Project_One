#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "tilemap.h"
#include "music.h"
#include "window.h"
#include "collisions.h"
#include "menu.h"
#include <stdlib.h> // rand(), srand() 
#include <time.h>   // for seeding srand()    
#include "character.h"  
#include "texture.h"

#define SPEED 300
#define TILE_SIZE 64
#define TILE_W_AMOUNT 19  // changed from 60 to test collisions 
#define TILE_H_AMOUNT 11  // changed from 60 to test collisions
#define GAME_W TILE_W_AMOUNT*TILE_SIZE
#define GAME_H TILE_H_AMOUNT*TILE_SIZE
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 700  // changed from 800 to adjust to my screen size

SDL_Window* pWindow = NULL;
SDL_Renderer* pRenderer = NULL;



int runGame()
{
    if(init_SDL_window(&pWindow, &pRenderer, WINDOW_WIDTH, WINDOW_HEIGHT) != 0) {
        printf("Failed to initialize window and renderer.\n");
        return 1;
    }

    char soundPathbgm[] = "resources/music/bgm1.mp3";
    BackgroundMusic *bgm =init_background_music(soundPathbgm, 20);
    play_background_music(bgm);
    free_bgm(bgm);

    // TODO: Move this to a function, maybe in a separate file
    SDL_Texture* pWhite = NULL;
    SDL_Texture* pBlack = NULL;
    SDL_Texture* pTexture = NULL;
    SDL_Texture* pVingette = NULL;

    create_texture(&pBlack, pRenderer,  "resources/black.png");
    create_texture(&pWhite, pRenderer, "resources/white.png");



    Character shipRect;
    init_character(&shipRect, pRenderer, "resources/ship.png");

    Character secondCharacter;
    init_character(&secondCharacter, pRenderer, "resources/ship.png"); // Use a different texture if desired
    secondCharacter.rect.x = 120; // Starting position
    secondCharacter.rect.y = 120; // Starting position

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
    

    bool menu = true;
    bool music = true;
    bool up, down, left, right, space, m, lower_volume, inc_volume;
    up = down = left = right = space = m = lower_volume = inc_volume = false;   

    bool w, a, s, d;
    w = a = s = d = false;  


    ////////////////////////////////////////////////////////
    // TODO: remove. these are to test collisions only.      

    SDL_Texture* bush = NULL;
    SDL_Texture* cactus = NULL;                
    SDL_Texture* stone = NULL; 
    SDL_Texture* tile2 = NULL;  
    SDL_Texture* tile5 = NULL; 

    create_texture(&bush, pRenderer, "resources/testpack/Bush.png");    
    create_texture(&cactus, pRenderer, "resources/testpack/Cactus.png"); 
    create_texture(&stone, pRenderer, "resources/testpack/Stone.png"); 
    create_texture(&tile2, pRenderer, "resources/testpack/tile2.png"); 
    create_texture(&tile5, pRenderer, "resources/testpack/tile5.png"); 

    srand(time(NULL)); 

    Tile testTiles[] = {
        {2, 0, bush, rect},       // not solid (decoration)   
        {3, 0, cactus, rect},     // not solid
        {4, 0, stone, rect},      // not solid   
        {5, 1, tile2, rect},      // solid (blocks movement) 
        {6, 1, tile5, rect}       // solid    
    };         
    

    // place 3 of each randomly 
    bool positions[TILE_W_AMOUNT][TILE_H_AMOUNT] = {false};  // Track whether a position is occupied
    for (int i = 0; i < 5; i++) {
        int count = 0;
        while (count < 3) {
            int x = rand() % TILE_W_AMOUNT;
            int y = rand() % TILE_H_AMOUNT;

            if (!positions[x][y]) {  // Check if the position is already occupied
                tilemap_set_tile(&tilemap, x, y, &testTiles[i]);
                positions[x][y] = true;  // Mark the position as occupied
                count++;
            }
        }
    }

    bool closeWindow = false;
    while(!closeWindow)
    {


        SDL_Event event;
        SDL_RenderClear(pRenderer);
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                closeWindow = true;
                break;
            // * this is the movement code from simpleSDLexample1.
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE:
                        menu = true;
                        break;
                    case SDL_SCANCODE_M:
                        m = true;
                        break;
                    case SDL_SCANCODE_SPACE:
                        space = true;
                        break;
                    case SDL_SCANCODE_W:
                        w = true;
                        break;
                    case SDL_SCANCODE_UP:
                        up = true;
                        break;
                    case SDL_SCANCODE_A:
                        a = true;
                        break;
                    case SDL_SCANCODE_LEFT:
                        left = true;
                        break;
                    case SDL_SCANCODE_S:
                        s = true;
                        break;
                    case SDL_SCANCODE_DOWN:
                        down = true;
                        break;
                    case SDL_SCANCODE_D:
                        d = true;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        right = true;
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                        w = false;
                        break;
                    case SDL_SCANCODE_UP:
                        up = false;
                        break;
                    case SDL_SCANCODE_A:
                        a = false;
                        break;
                    case SDL_SCANCODE_LEFT:
                        left = false;
                        break;
                    case SDL_SCANCODE_S:
                        s = false;
                        break;
                    case SDL_SCANCODE_DOWN:
                        down = false;
                        break;
                    case SDL_SCANCODE_D:
                        d = false;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        right = false;
                        break;
                }
                break;
            }   
        }   

        //draw all charz
        move_character(&shipRect, &tilemap, WINDOW_WIDTH, WINDOW_HEIGHT, up, down, left, right);
        move_character(&secondCharacter, &tilemap, WINDOW_WIDTH, WINDOW_HEIGHT, w, s, a, d);
        

        char soundPath[] = "resources/music/sse1.mp3";
        Single_sound *kill_sound = init_sound_effect(soundPath, 30);

        if(space)
        {
            play_sound_once(kill_sound);
            free_sse(kill_sound);
            space = false;
        }
        if(m)
        {
            toggle_music();
            m = false;
        }
        if(menu)
        {
            closeWindow = mainMenu(pRenderer);
            menu = false;
        }


        SDL_Rect none = { 0,0,0,0 };

        tilemap_draw(&tilemap, pRenderer, &none);
        draw_character(pRenderer, &shipRect);
        draw_character(pRenderer, &secondCharacter);

        SDL_RenderPresent(pRenderer);
        SDL_Delay(1000 / 120);//60 frames/s
    }

    tilemap_free(&tilemap);
    cleanup_character(&shipRect);
    cleanup_character(&secondCharacter);
    cleanup_SDL(pWindow, pRenderer);
    return 0;
}