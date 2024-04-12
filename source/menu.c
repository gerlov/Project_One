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

bool renderMenuItem(MenuItem item)
{
    if (item.texture == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return true;
    }
    // copy the texture to the rendering context
    SDL_RenderCopy(item.renderer, item.texture, NULL, &item.position);

    return false;
}

bool optionsMenu(SDL_Renderer* renderer)
{
    bool options = true;
    bool closeWindow = false;

    SDL_RenderClear(renderer);
    // Load all textures for options menu
    MenuItem background = {IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH), renderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}};
    MenuItem exitOptions = {IMG_LoadTexture(renderer, EXIT_OPTIONS_IMG_PATH), renderer, {BUTTONS_X, 500, BUTTON_WIDTH, BUTTON_HEIGHT}};
    // Render all items
    if(renderMenuItem(background)) return closeWindow;
    if(renderMenuItem(exitOptions)) return closeWindow;
    // Present render
    SDL_RenderPresent(renderer);

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

bool mainMenu(SDL_Renderer* renderer)
{
    int w, h; // Texture width and height
    bool menu = true;
    bool closeWindow = false;
    bool menuRendered = false;

    // Load all textures for main menu
    MenuItem background = {IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH), renderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}};
    MenuItem startGame = {IMG_LoadTexture(renderer, START_GAME_IMG_PATH), renderer, {BUTTONS_X, 100, BUTTON_WIDTH, BUTTON_HEIGHT}};
    MenuItem options = {IMG_LoadTexture(renderer, OPTIONS_IMG_PATH), renderer, {BUTTONS_X, 300, BUTTON_WIDTH, BUTTON_HEIGHT}};
    MenuItem quitGame = {IMG_LoadTexture(renderer, QUIT_GAME_IMG_PATH), renderer, {BUTTONS_X, 500, BUTTON_WIDTH, BUTTON_HEIGHT}};

	// Main menu loop
	while (menu) {
        if (!menuRendered)
        {
            // Clear the screen
            SDL_RenderClear(renderer);

            // Render background and menu items
            if(renderMenuItem(background)) return closeWindow;
            if(renderMenuItem(startGame)) return closeWindow;
            if(renderMenuItem(options)) return closeWindow;
            if(renderMenuItem(quitGame)) return closeWindow;

            SDL_RenderPresent(renderer);

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
                    if(optionsMenu(renderer) == true)
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