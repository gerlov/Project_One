#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "menu.h"
#include "music.h"

#define BACKGROUND_IMG_PATH "resources/menu-background.png"
#define START_GAME_IMG_PATH "resources/StartGame.png"
#define OPTIONS_IMG_PATH "resources/Options.png"
#define QUIT_GAME_IMG_PATH "resources/QuitGame.png"
#define EXIT_OPTIONS_IMG_PATH "resources/ExitOptions.png"
#define MUTE_MUSIC_IMG_PATH "resources/MuteMusic.png"
#define UNMUTE_MUSIC_IMG_PATH "resources/UnmuteMusic.png"
#define DECREASE_MUSIC_IMG_PATH "resources/DecreaseMusic.png"
#define INCREASE_MUSIC_IMG_PATH "resources/IncreaseMusic.png"
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define BUTTON_WIDTH 349
#define BUTTON_HEIGHT 162
#define BUTTONS_X 425

bool musicMuted = false;

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
    MenuItem decreaseMusic = {IMG_LoadTexture(renderer, DECREASE_MUSIC_IMG_PATH), renderer, {BUTTONS_X, 100, BUTTON_WIDTH/2, BUTTON_HEIGHT}};
    MenuItem increaseMusic = {IMG_LoadTexture(renderer, INCREASE_MUSIC_IMG_PATH), renderer, {BUTTONS_X+175, 100, BUTTON_WIDTH/2, BUTTON_HEIGHT}};
    MenuItem muteMusic = {IMG_LoadTexture(renderer, MUTE_MUSIC_IMG_PATH), renderer, {BUTTONS_X, 300, BUTTON_WIDTH, BUTTON_HEIGHT}};
    MenuItem unmuteMusic = {IMG_LoadTexture(renderer, UNMUTE_MUSIC_IMG_PATH), renderer, {BUTTONS_X, 300, BUTTON_WIDTH, BUTTON_HEIGHT}};
    MenuItem exitOptions = {IMG_LoadTexture(renderer, EXIT_OPTIONS_IMG_PATH), renderer, {BUTTONS_X, 500, BUTTON_WIDTH, BUTTON_HEIGHT}};

    // Render all items
    if(renderMenuItem(background)) return closeWindow;
    if(renderMenuItem(decreaseMusic)) return closeWindow;
    if(renderMenuItem(increaseMusic)) return closeWindow;
    if(renderMenuItem(exitOptions)) return closeWindow;

    // Options loop
    while (options) {
        //Check whether the music is muted or not
        if(musicMuted)
        {
            if(renderMenuItem(unmuteMusic)) return closeWindow;
        }
        else
        {
            if(renderMenuItem(muteMusic)) return closeWindow;
        }

        SDL_RenderPresent(renderer);
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

                if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH/2 && 
                    mouseY >= 100 && mouseY <= 100 + BUTTON_HEIGHT) // If the decrease music button is clicked
                {
                    decrease_volume();
                }
                else if (mouseX >= BUTTONS_X+175 && mouseX <= BUTTONS_X + BUTTON_WIDTH && 
                    mouseY >= 100 && mouseY <= 100 + BUTTON_HEIGHT) // If the increase music button is clicked
                {
                    increase_volume();
                }
                else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH && 
                    mouseY >= 300 && mouseY <= 300 + BUTTON_HEIGHT) // If the mute/unmute music button is clicked
                {
                    toggle_music();
                    musicMuted = !musicMuted;
                }
                else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH && 
                    mouseY >= 500 && mouseY <= 500 + BUTTON_HEIGHT) // If the exit button is clicked
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

                if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH && 
                    mouseY >= 100 && mouseY <= 100 + BUTTON_HEIGHT)
                {
                    menu = false;
                }
                else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                         mouseY >= 300 && mouseY <= 300 + BUTTON_HEIGHT)
                {
                    if(optionsMenu(renderer) == true)
                    {
                        closeWindow = true;
                        menu = false;
                    }
                    menuRendered = false;
                }
                else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                         mouseY >= 500 && mouseY <= 500 + BUTTON_HEIGHT)
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