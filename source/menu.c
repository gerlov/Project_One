#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "menu.h"


#define BACKGROUND_IMG_PATH "resources/menu-background.png"
#define START_GAME_IMG_PATH "resources/StartGame.png"
#define OPTIONS_IMG_PATH "resources/Options.png"
#define QUIT_GAME_IMG_PATH "resources/QuitGame.png"
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define BUTTON_WIDTH 349
#define BUTTON_HEIGHT 162

bool renderMenuItem(SDL_Renderer* renderer, const char* file, int x, int y, int width, int height)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, file);
    if (texture == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return true;
    }
	// put the location where we want the texture to be drawn into a rectangle
	SDL_Rect image; image.x = x; image.y = y; image.w = width; image.h = height; 
    // copy the texture to the rendering context
    SDL_RenderCopy(renderer, texture, NULL, &image);

    SDL_RenderPresent(renderer);

    return false;
}

bool open_menu(SDL_Renderer* renderer)
{
    int w, h; // Texture width and height
    bool menu = true;
    bool closeWindow = false;

    // clear the screen
    SDL_RenderClear(renderer);

    // Render background and menu items
    if(renderMenuItem(renderer, BACKGROUND_IMG_PATH, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)) return closeWindow;
    if(renderMenuItem(renderer, START_GAME_IMG_PATH, 425, 100, BUTTON_WIDTH, BUTTON_HEIGHT)) return closeWindow;
    if(renderMenuItem(renderer, OPTIONS_IMG_PATH, 425, 300, BUTTON_WIDTH, BUTTON_HEIGHT)) return closeWindow;
    if(renderMenuItem(renderer, QUIT_GAME_IMG_PATH, 425, 500, BUTTON_WIDTH, BUTTON_HEIGHT)) return closeWindow;

	// main menu loop
	while (1) {
		// event handling
		SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                return true; // Closes the application
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    return false; // Returns to the game
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY); // Get mouse location

                if (mouseX >= 425 && mouseX <= 425 + BUTTON_WIDTH && mouseY >= 100 && mouseY <= 100 + BUTTON_HEIGHT)
                {
                    return false; // Returns to the game
                }
                if (mouseX >= 425 && mouseX <= 425 + BUTTON_WIDTH && mouseY >= 300 && mouseY <= 300 + BUTTON_HEIGHT)
                {
                    return false; // Returns to the game for now...
                }
                if (mouseX >= 425 && mouseX <= 425 + BUTTON_WIDTH && mouseY >= 500 && mouseY <= 500 + BUTTON_HEIGHT)
                {
                    return true; // Closes the application
                }
            }
        }
	}
}