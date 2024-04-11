#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "menu.h"


#define BACKGROUND_IMG_PATH "resources/menu-background.png"
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

bool renderMenuItem(SDL_Renderer* renderer, const char* file, int x, int y, int width, int height)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH);
    if (texture == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return true;
    }
	// put the location where we want the texture to be drawn into a rectangle
	SDL_Rect image; image.x = x; image.y = y; image.w = width; image.h = height; 
    // clear the screen
    SDL_RenderClear(renderer);
    // copy the texture to the rendering context
    SDL_RenderCopy(renderer, texture, NULL, &image);
    // show the image that has been rendered
    SDL_RenderPresent(renderer);

    return false;
}

bool open_menu(SDL_Renderer* renderer)
{
    int w, h; // Texture width and height
    bool menu = true;
    bool closeWindow = false;

    if(renderMenuItem(renderer, BACKGROUND_IMG_PATH, 0, 0, WINDOW_HEIGHT, WINDOW_WIDTH)) return true;

	// main loop
	while (menu) {
		// event handling
		SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                menu = false;
                closeWindow = true;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    menu = false;
                    break;
                }
                break;
            }
        }
		
	}

    return closeWindow;
}