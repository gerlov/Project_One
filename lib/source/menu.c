#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "menu.h"
#include "music.h"

#define BACKGROUND_IMG_PATH "../lib/resources/MenuBackground.png"
#define START_GAME_IMG_PATH "../lib/resources/StartGame.png"
#define OPTIONS_IMG_PATH "../lib/resources/Options.png"
#define QUIT_GAME_IMG_PATH "../lib/resources/QuitGame.png"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define BUTTON_WIDTH 349
#define BUTTON_HEIGHT 162
#define BUTTONS_X 425
#define OUTLINE_WIDTH 8

#define SLIDER_WIDTH 349
#define SLIDER_HEIGHT 20
#define SLIDER_Y 250

int volSliderValue = 100; // Initial volume value
int prevVolSliderValue; // For slider to sync with mute/unmute button
bool draggingSlider = false;
bool musicMuted = false;

bool renderMenuItem(MenuItem item)
{
    if (item.texture == NULL)
    {
        printf("Error loading image: %s\n", IMG_GetError());
        return true;
    }
    // copy the texture to the rendering context
    SDL_RenderCopy(item.renderer, item.texture, NULL, &item.position);

    return false;
}

void createMenuButton(SDL_Renderer *renderer, char *text, int r, int g, int b, int y)
{
    SDL_Color textColor = {0, 0, 0, 255};

    TTF_Font *Font = TTF_OpenFont("../lib/resources/Jacquard24-Regular.ttf", 24);
    if (Font == NULL)
    {
        return;
    }

    // Draw the outline
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // Outline color (black in this case)
    SDL_Rect buttonOutline = {BUTTONS_X - OUTLINE_WIDTH / 2, y - OUTLINE_WIDTH / 2, BUTTON_WIDTH + OUTLINE_WIDTH, BUTTON_HEIGHT + OUTLINE_WIDTH};
    SDL_RenderFillRect(renderer, &buttonOutline);

    // Draw the button
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_Rect buttonBackground = {BUTTONS_X, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    SDL_RenderFillRect(renderer, &buttonBackground);

    // Draw the text
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Font, text, textColor);
    SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect Message_rect = {BUTTONS_X + 25, y + 25, BUTTON_WIDTH - 50, BUTTON_HEIGHT - 50};
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

void drawText(SDL_Renderer *renderer, char *text, int y)
{
    SDL_Color textColor = {255, 255, 255, 255};

    TTF_Font *Font = TTF_OpenFont("../lib/resources/Jacquard24-Regular.ttf", 24);
    if (Font == NULL)
    {
        return;
    }

    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Font, text, textColor);
    SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect Message_rect = {BUTTONS_X + 25, y + 25, BUTTON_WIDTH - 50, BUTTON_HEIGHT - 50};
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

void renderSlider(SDL_Renderer *renderer, float handleX)
{
    // Volume slider background
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
    SDL_Rect sliderBackground = {BUTTONS_X, SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT};
    SDL_RenderFillRect(renderer, &sliderBackground);

    // Volume slider handle
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_Rect sliderHandle = {handleX, SLIDER_Y, 20, SLIDER_HEIGHT};
    SDL_RenderFillRect(renderer, &sliderHandle);
}

void toggle_music_logic(void)
{
    toggle_music();
    musicMuted = !musicMuted;
    if(musicMuted)
    {
        if(!volSliderValue)
        {
            prevVolSliderValue = 1;
        }
        else
        {
            prevVolSliderValue = volSliderValue;
        }
        volSliderValue = 0;
    }
    else
    {
        volSliderValue = prevVolSliderValue;
    }
}

bool optionsMenu(SDL_Renderer *renderer)
{
    bool options = true;
    bool closeWindow = false;
    int mouseX, mouseY;

    // Load background texture
    MenuItem background = {IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH), renderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}};

    // Options loop
    while (options)
    {
        // Clear the screen
        SDL_RenderClear(renderer);

        // Render menu items and background
        if (renderMenuItem(background)) return closeWindow;

        drawText(renderer, "Volume Slider", 100);
        float handleX = BUTTONS_X + (float)(SLIDER_WIDTH - 20) * ((float)volSliderValue / 100.0);
        renderSlider(renderer, handleX);

        if (musicMuted)
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
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                closeWindow = true; // Closes the application
                options = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    options = false; // Returns to the menu
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    SDL_GetMouseState(&mouseX, &mouseY); // Get mouse location

                    if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + SLIDER_WIDTH &&
                        mouseY >= SLIDER_Y && mouseY <= SLIDER_Y + SLIDER_HEIGHT)
                    {
                        draggingSlider = true;
                        float newHandleX = mouseX - 10; // Center the handle under the mouse
                        // Make sure the slider does not go "out of bounds"
                        if (newHandleX < BUTTONS_X)
                        {
                            newHandleX = BUTTONS_X;
                        }
                        else if (newHandleX > BUTTONS_X + SLIDER_WIDTH - 20)
                        {
                            newHandleX = BUTTONS_X + SLIDER_WIDTH - 20;
                        }
                        // Update the volume value based on the slider position
                        volSliderValue = (float)(newHandleX - BUTTONS_X) / (SLIDER_WIDTH - 20) * 100;
                        // Set volume to new value
                        set_volume(volSliderValue);
                    }
                    else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                             mouseY >= 300 && mouseY <= 300 + BUTTON_HEIGHT) // If the mute/unmute music button is clicked
                    {
                        toggle_music_logic();
                    }
                    else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                             mouseY >= 500 && mouseY <= 500 + BUTTON_HEIGHT) // If the exit button is clicked
                    {
                        options = false;
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    draggingSlider = false;
                }
                break;
            case SDL_MOUSEMOTION:
                if (draggingSlider)
                {
                    float newHandleX = event.motion.x - 10; // Center the handle under the mouse
                    // Make sure the slider does not go "out of bounds"
                    if (newHandleX < BUTTONS_X)
                    {
                        newHandleX = BUTTONS_X;
                    }
                    else if (newHandleX > BUTTONS_X + SLIDER_WIDTH - 20)
                    {
                        newHandleX = BUTTONS_X + SLIDER_WIDTH - 20;
                    }
                    // Update the volume value based on the slider position
                    volSliderValue = (float)(newHandleX - BUTTONS_X) / (SLIDER_WIDTH - 20) * 100;
                    // Adjust the volume based on the new value
                    set_volume(volSliderValue);
                }
                break;
            }
            
            if(!volSliderValue && !musicMuted || volSliderValue && musicMuted) toggle_music_logic();
        }
    }

    return closeWindow;
}

bool mainMenu(SDL_Renderer *renderer)
{
    bool menu = true;
    bool closeWindow = false;

    TTF_Init(); // Initialize TTF

    // Load all textures for main menu
    MenuItem background = {IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH), renderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}};

    // Main menu loop
    while (menu)
    {
        // Clear the screen
        SDL_RenderClear(renderer);

        // Render background and menu items
        if (renderMenuItem(background)) return closeWindow;
        createMenuButton(renderer, "Start Game", 1, 50, 32, 100);
        createMenuButton(renderer, "Options", 105, 105, 105, 300);
        createMenuButton(renderer, "Quit Game", 139, 0, 0, 500);

        SDL_RenderPresent(renderer);

        // Event handling
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                closeWindow = true; // Closes the application
                menu = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode)
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
                    if (optionsMenu(renderer) == true)
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