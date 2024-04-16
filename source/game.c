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
#define TILE_W_AMOUNT 128  // changed from 60 to test collisions 
#define TILE_H_AMOUNT 128  // changed from 60 to test collisions
#define GAME_W TILE_W_AMOUNT*TILE_SIZE
#define GAME_H TILE_H_AMOUNT*TILE_SIZE
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800  // changed from 800 to adjust to my screen size

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
    SDL_Texture* pTexture = NULL;
    SDL_Texture* pVingette = NULL;




    Character testHunter;
    init_character(&testHunter, pRenderer, "resources/ship.png", 1);

    Character testHuman;
    init_character(&testHuman, pRenderer, "resources/ship.png", 0); // Use a different texture if desired
    testHuman.rect.x = 420;

    //Keeping track of all characters
    Character* characters[] = {&testHunter, &testHuman};
    int num_characters = 2;  

    //Finding the hunter
    Character* hunter = NULL;
    for (int i = 0; i < num_characters; i++) {
        if (characters[i]->isHunter == 1) {
            hunter = characters[i];
            break;
        }
    }

    TileMap tilemap;
    SDL_Rect rect = { 0, 0, TILE_SIZE, TILE_SIZE };
    tilemap_init(&tilemap, pRenderer, TILE_W_AMOUNT, TILE_H_AMOUNT, TILE_SIZE);
    tilemap_load(&tilemap, 2);
    randomize_floor(&tilemap, 0);
    orient_walls(&tilemap);
    

    bool menu = true;
    bool music = true;
    bool up, down, left, right, space, m, lower_volume, inc_volume;
    up = down = left = right = space = m = lower_volume = inc_volume = false;   

    bool w, a, s, d;
    w = a = s = d = false;  

    srand(time(NULL)); 

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
        move_character(&testHunter, &tilemap, WINDOW_WIDTH, WINDOW_HEIGHT, 
                      up, down, left, right, characters, num_characters);
        move_character(&testHuman, &tilemap, WINDOW_WIDTH, WINDOW_HEIGHT, 
                      w, s, a, d, characters, num_characters);
        



        if(space)
        {
            kill_command(hunter, characters, num_characters);
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


        

        tilemap_draw(&tilemap);
        draw_character(pRenderer, &testHunter);
        draw_character(pRenderer, &testHuman);


        SDL_RenderPresent(pRenderer);
        SDL_Delay(1000 / 120);//60 frames/s
    }

    tilemap_free(&tilemap);
    cleanup_character(&testHunter);
    cleanup_character(&testHuman);
    cleanup_SDL(pWindow, pRenderer);
    return 0;
}