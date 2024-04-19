#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "menu.h"
#include "music.h"

#define BACKGROUND_IMG_PATH "resources/MenuBackground.png"
#define START_GAME_IMG_PATH "resources/StartGame.png"
#define OPTIONS_IMG_PATH "resources/Options.png"
#define QUIT_GAME_IMG_PATH "resources/QuitGame.png"
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define BUTTON_WIDTH 349
#define BUTTON_HEIGHT 162
#define BUTTONS_X 425
#define OUTLINE_WIDTH 8
#define SLIDER_WIDTH 20
#define SLIDER_HEIGHT 200
#define SLIDER_PADDING 10
#define TRACK_WIDTH 10

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

void createMenuButton(SDL_Renderer* renderer, char* text, int r, int g, int b, int y)
{
    SDL_Color textColor = {0, 0, 0, 255};

    TTF_Font* Font = TTF_OpenFont("resources/Jacquard24-Regular.ttf", 24);
    if (Font == NULL) {
        return;
    }

    // Draw the outline
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // Outline color (black in this case)
    SDL_Rect buttonOutline = {BUTTONS_X-OUTLINE_WIDTH/2, y-OUTLINE_WIDTH/2, BUTTON_WIDTH+OUTLINE_WIDTH, BUTTON_HEIGHT+OUTLINE_WIDTH};
    SDL_RenderFillRect(renderer, &buttonOutline);

    // Draw the button
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_Rect buttonBackground = {BUTTONS_X, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    SDL_RenderFillRect(renderer, &buttonBackground);

    // Draw the text
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Font, text, textColor);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect Message_rect = {BUTTONS_X+25, y+25, BUTTON_WIDTH-50, BUTTON_HEIGHT-50};
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

bool optionsMenu(SDL_Renderer* renderer)
{
    bool options = true;
    bool closeWindow = false;
    int mouseX, mouseY;

    TTF_Init(); // Initialize TTF

    // Load background texture
    MenuItem background = {IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH), renderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}};

    // Options loop
    while (options) {
        // Clear the screen
        SDL_RenderClear(renderer);

        if(renderMenuItem(background)) return closeWindow;

        if(musicMuted)
        {
            createMenuButton(renderer, "Music OFF", 139, 0, 0, 300);
        }
        else
        {
            createMenuButton(renderer, "Music ON", 1, 50, 32, 300);
        }
        createMenuButton(renderer, "Exit Options", 105, 105, 105, 500);

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
                if (event.button.button == SDL_BUTTON_LEFT) {
                    SDL_GetMouseState(&mouseX, &mouseY); // Get mouse location

                    // if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH/2 && 
                    //     mouseY >= 100 && mouseY <= 100 + BUTTON_HEIGHT) // If the decrease music button is clicked
                    // {
                    //     decrease_volume();
                    // }
                    // else if (mouseX >= BUTTONS_X+175 && mouseX <= BUTTONS_X + BUTTON_WIDTH && 
                    //     mouseY >= 100 && mouseY <= 100 + BUTTON_HEIGHT) // If the increase music button is clicked
                    // {
                    //     increase_volume();
                    // }
                    if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH && 
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
                }
            }
        }
	}

    return closeWindow;
}

bool mainMenu(SDL_Renderer* renderer)
{
    bool menu = true;
    bool closeWindow = false;

    // Load all textures for main menu
    MenuItem background = {IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH), renderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}};
    MenuItem startGame = {IMG_LoadTexture(renderer, START_GAME_IMG_PATH), renderer, {BUTTONS_X, 100, BUTTON_WIDTH, BUTTON_HEIGHT}};
    MenuItem options = {IMG_LoadTexture(renderer, OPTIONS_IMG_PATH), renderer, {BUTTONS_X, 300, BUTTON_WIDTH, BUTTON_HEIGHT}};
    MenuItem quitGame = {IMG_LoadTexture(renderer, QUIT_GAME_IMG_PATH), renderer, {BUTTONS_X, 500, BUTTON_WIDTH, BUTTON_HEIGHT}};

	// Main menu loop
	while (menu) {
        // Clear the screen
        SDL_RenderClear(renderer);

        // Render background and menu items
        if(renderMenuItem(background)) return closeWindow;
        if(renderMenuItem(startGame)) return closeWindow;
        if(renderMenuItem(options)) return closeWindow;
        if(renderMenuItem(quitGame)) return closeWindow;

        SDL_RenderPresent(renderer);

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