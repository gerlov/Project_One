#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include "menu.h"
#include "music.h"

#define BACKGROUND_IMG_PATH "../lib/assets/menu/MenuBackground.png"

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

#define BUTTON_WIDTH WINDOW_WIDTH / 3
#define BUTTON_HEIGHT WINDOW_HEIGHT / 4
#define BUTTONS_X WINDOW_WIDTH / 2 - BUTTON_WIDTH / 2
#define BUTTONS_Y (WINDOW_HEIGHT - BUTTON_HEIGHT * 3) / 2
#define BUTTON_SPACE_BETWEEN WINDOW_WIDTH / 20
#define TOP_BUTTON_Y BUTTONS_Y - BUTTON_SPACE_BETWEEN
#define MIDDLE_BUTTON_Y BUTTONS_Y * 3
#define BOTTOM_BUTTON_Y BUTTONS_Y * 5 + BUTTON_SPACE_BETWEEN
#define OUTLINE_WIDTH WINDOW_WIDTH / 62

#define SLIDER_WIDTH BUTTON_WIDTH + OUTLINE_WIDTH
#define SLIDER_HEIGHT WINDOW_HEIGHT / 20
#define SLIDER_Y TOP_BUTTON_Y + BUTTON_HEIGHT - BUTTON_SPACE_BETWEEN + OUTLINE_WIDTH
#define SLIDER_X BUTTONS_X - OUTLINE_WIDTH / 2
#define HANDLE_WIDTH WINDOW_WIDTH / 20

#define PLAYER_TEXT_OFFSET WINDOW_WIDTH / 40
#define PLAYER_TEXT_HEIGHT WINDOW_WIDTH / 10
#define PLAYER_TEXT_INBETWEEN_SPACE WINDOW_WIDTH / 8

#define INPUT_FONT_SIZE WINDOW_HEIGHT / 20

int volSliderValue = 30; // Initial volume value
int prevVolSliderValue; // For slider to sync with mute/unmute button
bool draggingSlider = false;
bool musicMuted = false;

typedef struct MenuItem {
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    SDL_Rect position;
} MenuItem;

bool renderMenuItem(MenuItem *item)
{
    if (item->texture == NULL)
    {
        printf("Error loading image: %s\n", IMG_GetError());
        return true;
    }
    // copy the texture to the rendering context
    SDL_RenderCopy(item->renderer, item->texture, NULL, &item->position);

    return false;
}

void createMenuButton(SDL_Renderer *renderer, char *text, int r, int g, int b, int y)
{
    SDL_Color textColor = {0, 0, 0, 255};

    TTF_Font *Font = TTF_OpenFont("../lib/assets/Jacquard24-Regular.ttf", 24);
    if (Font == NULL)
    {
        return;
    }

    // Draw the outline
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // Outline color (black in this case)
    SDL_Rect buttonOutline = {(BUTTONS_X) - (OUTLINE_WIDTH) / 2, y - (OUTLINE_WIDTH) / 2, (BUTTON_WIDTH) + (OUTLINE_WIDTH), (BUTTON_HEIGHT) + (OUTLINE_WIDTH)};
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

void drawText(SDL_Renderer *renderer, SDL_Color color, TTF_Font *font, char *text, int x, int y, int w, int h)
{
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect Message_rect = {x, y, w, h};
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

void renderSlider(SDL_Renderer *renderer, float handleX)
{
    // Volume slider background
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
    SDL_Rect sliderBackground = {SLIDER_X, SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT};
    SDL_RenderFillRect(renderer, &sliderBackground);

    // Volume slider handle
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_Rect sliderHandle = {handleX, SLIDER_Y, HANDLE_WIDTH, SLIDER_HEIGHT};
    SDL_RenderFillRect(renderer, &sliderHandle);
}


void toggle_music_logic(void)
{
    toggle_music();
    musicMuted = !musicMuted;
    if (musicMuted)
    {
        if (volSliderValue != 0)
        {
            prevVolSliderValue = volSliderValue;
            volSliderValue = 0;
        }
    }
    else
    {
        volSliderValue = (prevVolSliderValue != 0) ? prevVolSliderValue : 50; // Set to default if prevVolSliderValue is zero
        set_volume(volSliderValue);
    }
}


bool optionsMenu(SDL_Renderer *renderer)
{
    bool options = true;
    bool closeWindow = false;
    int mouseX, mouseY;
    float handleX;

    // Load background texture
    MenuItem background = {IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH), renderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}};

    // Options loop
    while (options)
    {
        // Clear the screen
        SDL_RenderClear(renderer);

        // Render menu items and background
        if (renderMenuItem(&background))
            return closeWindow;
        TTF_Font *font = TTF_OpenFont("../lib/assets/Jacquard24-Regular.ttf", 24);
        drawText(renderer, (SDL_Color){255, 255, 255, 255}, font, "Volume Slider", BUTTONS_X + 25, SLIDER_Y - SLIDER_HEIGHT - BUTTON_SPACE_BETWEEN*2, BUTTON_WIDTH - 50, BUTTON_HEIGHT - 50);
        handleX = (float)SLIDER_X + (float)(SLIDER_WIDTH - HANDLE_WIDTH) * ((float)volSliderValue / 100.0);
        
        renderSlider(renderer, handleX);

        if (musicMuted)
        {
            createMenuButton(renderer, "Music OFF", 139, 0, 0, MIDDLE_BUTTON_Y);
        }
        else
        {
            createMenuButton(renderer, "Music ON", 1, 50, 32, MIDDLE_BUTTON_Y);
        }
        createMenuButton(renderer, "Exit Options", 105, 105, 105, BOTTOM_BUTTON_Y);

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
                        float newHandleX = mouseX - (HANDLE_WIDTH / 2); // Center the handle under the mouse
                        // Make sure the slider does not go "out of bounds"
                        if (newHandleX < SLIDER_X)
                        {
                            newHandleX = SLIDER_X;
                        }
                        else if (newHandleX > SLIDER_X + SLIDER_WIDTH - HANDLE_WIDTH)
                        {
                            newHandleX = SLIDER_X + SLIDER_WIDTH - HANDLE_WIDTH;
                        }
                        // Update the volume value based on the slider position
                        volSliderValue = (float)(newHandleX - (SLIDER_X)) / ((SLIDER_WIDTH) - (HANDLE_WIDTH)) * 100;
                        // Set volume to new value
                        set_volume(volSliderValue);
                    }
                    else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                             mouseY >= MIDDLE_BUTTON_Y && mouseY <= MIDDLE_BUTTON_Y + BUTTON_HEIGHT) // If the mute/unmute music button is clicked
                    {
                        toggle_music_logic();
                    }
                    else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                             mouseY >= BOTTOM_BUTTON_Y && mouseY <= BOTTOM_BUTTON_Y + BUTTON_HEIGHT) // If the exit button is clicked
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
                    float newHandleX = event.motion.x - HANDLE_WIDTH / 2; // Center the handle under the mouse
                    // Make sure the slider does not go "out of bounds"
                    if (newHandleX < SLIDER_X)
                    {
                        newHandleX = SLIDER_X;
                    }
                    else if (newHandleX > SLIDER_X + SLIDER_WIDTH - HANDLE_WIDTH)
                    {
                        newHandleX = SLIDER_X + SLIDER_WIDTH - HANDLE_WIDTH;
                    }
                    // Update the volume value based on the slider position
                    volSliderValue = (float)(newHandleX - (SLIDER_X)) / ((SLIDER_WIDTH) - (HANDLE_WIDTH)) * 100;
                    // Adjust the volume based on the new value
                    set_volume(volSliderValue);
                }
                break;
            }

            if (!volSliderValue && !musicMuted || volSliderValue && musicMuted)
                toggle_music_logic();
        }
    }

    return closeWindow;
}

bool findGameScreen(SDL_Renderer *renderer, char hostAddress[MAX_ADDRESS_LENGTH], bool *joinGame)
{
    bool findGame = true;
    bool closeWindow = false;
    bool textBoxSelected = false;

    if (TTF_Init() == -1)
    {
        fprintf(stderr, "TTF could not initialize! TTF Error: %s\n", TTF_GetError());
        return 1;
    }

    char inputText[MAX_ADDRESS_LENGTH] = "";
    int textLength = 0;
    TTF_Font *font = TTF_OpenFont("../lib/assets/Roboto-Regular.ttf", INPUT_FONT_SIZE);
    SDL_Color textColor = {0, 0, 0, 255}; // Black text

    // Load background texture
    MenuItem background = {IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH), renderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}};

    // Find game loop
    while (findGame)
    {
        // Clear the screen
        SDL_RenderClear(renderer);

        // Render background & menu button
        if (renderMenuItem(&background))
            return closeWindow;
        createMenuButton(renderer, "", 255, 255, 255, TOP_BUTTON_Y);
        createMenuButton(renderer, "Join Game", 1, 50, 32, MIDDLE_BUTTON_Y);
        createMenuButton(renderer, "Back to Menu", 139, 0, 0, BOTTOM_BUTTON_Y);

        if (textLength > 0)
        {
            // Render the input text
            SDL_Surface *textSurface = TTF_RenderText_Solid(font, inputText, textColor);
            if (textSurface == NULL)
            {
                fprintf(stderr, "Text surface could not be created! TTF Error: %s\n", TTF_GetError());
            }
            else
            {
                SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                if (textTexture == NULL)
                {
                    fprintf(stderr, "Text texture could not be created! SDL Error: %s\n", SDL_GetError());
                }
                else
                {
                    int textW, textH;
                    SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
                    SDL_Rect textRect = {BUTTONS_X + 8, TOP_BUTTON_Y + BUTTON_HEIGHT / 2 - INPUT_FONT_SIZE / 2, textW, textH};
                    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                    SDL_DestroyTexture(textTexture);
                }
                SDL_FreeSurface(textSurface);
            }
        }

        SDL_RenderPresent(renderer);

        // Event handling
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                closeWindow = true; // Closes the application
                findGame = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    findGame = false; // Returns to the menu
                    break;
                case SDL_SCANCODE_BACKSPACE:
                    if (textLength > 0)
                    {
                        inputText[textLength - 1] = '\0';
                        textLength--;
                    }
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY); // Get mouse location
                if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                    mouseY >= TOP_BUTTON_Y && mouseY <= TOP_BUTTON_Y + BUTTON_HEIGHT)
                {
                    textBoxSelected = true;
                }
                else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                         mouseY >= MIDDLE_BUTTON_Y && mouseY <= MIDDLE_BUTTON_Y + BUTTON_HEIGHT)
                {
                    // Start the game
                    hostAddress = inputText;
                    *joinGame = true;
                    return closeWindow;
                }
                else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                         mouseY >= BOTTOM_BUTTON_Y && mouseY <= BOTTOM_BUTTON_Y + BUTTON_HEIGHT)
                {
                    findGame = false;
                    return closeWindow;
                }
                else
                {
                    textBoxSelected = false;
                }
                break;
            case SDL_TEXTINPUT:
                // Handle text input
                if (textLength < sizeof(inputText) - 1 && textBoxSelected)
                { // Leave room for null terminator
                    strcat(inputText, event.text.text);
                    textLength = strlen(inputText);
                }
                break;
            }
        }
    }
    TTF_CloseFont(font);
    TTF_Quit();
    return closeWindow;
}

bool menu(SDL_Renderer *renderer, char hostAddress[MAX_ADDRESS_LENGTH], bool inGame)
{
    bool menu = true;
    bool closeWindow = false;
    bool joinGame = false;

    TTF_Init(); // Initialize TTF

    // Load all textures for main menu
    MenuItem background = {IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH), renderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}};

    // Main menu loop
    while (menu)
    {
        // Clear the screen
        SDL_RenderClear(renderer);

        // Render background and menu items
        if (renderMenuItem(&background))
            return closeWindow;
        if (inGame)
        {
            createMenuButton(renderer, "Resume Game", 1, 50, 32, TOP_BUTTON_Y);
            createMenuButton(renderer, "Leave Game", 139, 0, 0, BOTTOM_BUTTON_Y);
        }
        else
        {
            createMenuButton(renderer, "Join Game", 1, 50, 32, TOP_BUTTON_Y);
            createMenuButton(renderer, "Quit Game", 139, 0, 0, BOTTOM_BUTTON_Y);
        }
        createMenuButton(renderer, "Options", 105, 105, 105, MIDDLE_BUTTON_Y);

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
                    if (inGame)
                        menu = false; // Returns to the game
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY); // Get mouse location

                if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                    mouseY >= TOP_BUTTON_Y && mouseY <= TOP_BUTTON_Y + BUTTON_HEIGHT)
                {
                    if (inGame)
                    {
                        menu = false;
                    }
                    else
                    {
                        if (findGameScreen(renderer, hostAddress, &joinGame) == true)
                        {
                            closeWindow = true;
                            menu = false;
                        }
                        if(joinGame == true)
                        {
                            return closeWindow;
                        }
                    }
                }
                else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                         mouseY >= MIDDLE_BUTTON_Y && mouseY <= MIDDLE_BUTTON_Y + BUTTON_HEIGHT)
                {
                    if (optionsMenu(renderer) == true)
                    {
                        closeWindow = true;
                        menu = false;
                    }
                }
                else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                         mouseY >= BOTTOM_BUTTON_Y && mouseY <= BOTTOM_BUTTON_Y + BUTTON_HEIGHT)
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

void drawLobby(SDL_Renderer *renderer, int readyPlayers[MAX_PLAYERS], int players)
{
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("../lib/assets/Roboto-Regular.ttf", 24);
    SDL_Color color = {255, 255, 255, 255};
    SDL_RenderClear(renderer);

    MenuItem background = {IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH), renderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}};
    renderMenuItem(&background);
    
    for (int i = 0; i < players; i++)
    {
        char *text = "Player ";
        char playerNum[2];
        sprintf(playerNum, "%d", i+1);
        
        // Calculate total length including null terminator
        size_t totalLength = strlen(text) + strlen(playerNum) + 1;
        
        // Allocate memory for the concatenated string
        char *result = (char *)malloc(totalLength);
        
        // Check if memory allocation succeeded
        if (result != NULL) {
            // Copy the strings into the result buffer
            strcpy(result, text);
            strcat(result, playerNum);

            // Free the dynamically allocated memory
            free(result);
        } else {
            printf("Memory allocation failed!\n");
        }

        if (readyPlayers[i])
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }
        SDL_Rect rect = {PLAYER_TEXT_HEIGHT * 2 + PLAYER_TEXT_OFFSET + 5, PLAYER_TEXT_OFFSET + i * PLAYER_TEXT_INBETWEEN_SPACE, PLAYER_TEXT_HEIGHT, PLAYER_TEXT_HEIGHT};
        SDL_RenderFillRect(renderer, &rect);
        drawText(renderer, color, font, result, PLAYER_TEXT_OFFSET, PLAYER_TEXT_OFFSET + i * PLAYER_TEXT_INBETWEEN_SPACE, PLAYER_TEXT_HEIGHT * 2, PLAYER_TEXT_HEIGHT);
    }
    SDL_RenderPresent(renderer);
}