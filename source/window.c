#include "window.h"
#include "music.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

int init_SDL_window(SDL_Window **window, SDL_Renderer **renderer, int WINDOW_WIDTH, int WINDOW_HEIGHT) {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    *window = SDL_CreateWindow("Game Title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if(!(*window)) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if(!(*renderer)) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return 1;
    }
    init_sounds();

    return 0;
}

void cleanup_SDL(SDL_Window* window, SDL_Renderer* renderer) {
    if(renderer != NULL) SDL_DestroyRenderer(renderer);
    if(window != NULL) SDL_DestroyWindow(window);
    
    SDL_Quit();
}
