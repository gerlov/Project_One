#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "menu.h"


#define BACKGROUND_IMG_PATH "resources/menu-background.png"
#define START_GAME_IMG_PATH "resources/StartGame.png"
#define OPTIONS_IMG_PATH "resources/Options.png"
#define QUIT_GAME_IMG_PATH "resources/QuitGame.png"
#define EXIT_OPTIONS_IMG_PATH "resources/ExitOptions.png"
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define BUTTON_WIDTH 349
#define BUTTON_HEIGHT 162
#define BUTTONS_X 425

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

bool options(SDL_Renderer* renderer)
{
    bool options = true;
    bool closeWindow = false;

    SDL_RenderClear(renderer);

    if(renderMenuItem(renderer, BACKGROUND_IMG_PATH, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)) return closeWindow;
    if(renderMenuItem(renderer, EXIT_OPTIONS_IMG_PATH, BUTTONS_X, 500, BUTTON_WIDTH, BUTTON_HEIGHT)) return closeWindow;

    // Options loop
    while (options) {
		// Event handling
		SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                closeWindow = true; // Closes the application
                options = false;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    options = false; // Returns to the menu
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY); // Get mouse location
                if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH && mouseY >= 500 && mouseY <= 500 + BUTTON_HEIGHT)
                {
                    options = false;
                }
                break;
            }
        }
	}

    return closeWindow;
}

bool open_menu(SDL_Renderer* renderer)
{
    int w, h; // Texture width and height
    bool menu = true;
    bool closeWindow = false;
    bool menuRendered = false;

	// Main menu loop
	while (menu) {
        if (!menuRendered)
        {
            // Clear the screen
            SDL_RenderClear(renderer);

            // Render background and menu items
            if(renderMenuItem(renderer, BACKGROUND_IMG_PATH, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)) return closeWindow;
            if(renderMenuItem(renderer, START_GAME_IMG_PATH, BUTTONS_X, 100, BUTTON_WIDTH, BUTTON_HEIGHT)) return closeWindow;
            if(renderMenuItem(renderer, OPTIONS_IMG_PATH, BUTTONS_X, 300, BUTTON_WIDTH, BUTTON_HEIGHT)) return closeWindow;
            if(renderMenuItem(renderer, QUIT_GAME_IMG_PATH, BUTTONS_X, 500, BUTTON_WIDTH, BUTTON_HEIGHT)) return closeWindow;

            menuRendered = true;
        }

		// Event handling
		SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                closeWindow = true; // Closes the application
                menu = false;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    menu = false; // Returns to the game
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY); // Get mouse location

                if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH && mouseY >= 100 && mouseY <= 100 + BUTTON_HEIGHT)
                {
                    menu = false;
                }
                else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH && mouseY >= 300 && mouseY <= 300 + BUTTON_HEIGHT)
                {
                    if(options(renderer) == true)
                    {
                        closeWindow = true;
                        menu = false;
                    }
                    menuRendered = false;
                }
                else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH && mouseY >= 500 && mouseY <= 500 + BUTTON_HEIGHT)
                {
                    closeWindow = true;
                    menu = false;
                }
                break;
            }
        }
	}
    return closeWindow;
}