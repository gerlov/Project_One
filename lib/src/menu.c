#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include "menu.h"
#include "SDL2/SDL_render.h"
#include "music.h"
#include <time.h>

#define BACKGROUND_IMG_PATH "../lib/assets/menu/MenuBackground.png"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 700

#define BUTTON_WIDTH WINDOW_WIDTH / (float)3
#define BUTTON_HEIGHT WINDOW_HEIGHT / (float)4
#define BUTTONS_X WINDOW_WIDTH / (float)2 - BUTTON_WIDTH / (float)2
#define BUTTONS_Y (WINDOW_HEIGHT - BUTTON_HEIGHT * 3) / (float)2
#define BUTTON_SPACE_BETWEEN WINDOW_WIDTH / (float)20
#define TOP_BUTTON_Y BUTTONS_Y - BUTTON_SPACE_BETWEEN
#define MIDDLE_BUTTON_Y BUTTONS_Y * 3
#define BOTTOM_BUTTON_Y BUTTONS_Y * 5 + BUTTON_SPACE_BETWEEN
#define OUTLINE_WIDTH WINDOW_WIDTH / (float)62

#define SLIDER_WIDTH BUTTON_WIDTH + OUTLINE_WIDTH
#define SLIDER_HEIGHT WINDOW_HEIGHT / (float)20
#define SLIDER_Y TOP_BUTTON_Y + BUTTON_HEIGHT - BUTTON_SPACE_BETWEEN + OUTLINE_WIDTH
#define SLIDER_X BUTTONS_X - OUTLINE_WIDTH / (float)2
#define HANDLE_WIDTH WINDOW_WIDTH / (float)20

#define PLAYER_TEXT_OFFSET WINDOW_WIDTH / (float)40
#define PLAYER_TEXT_HEIGHT WINDOW_WIDTH / (float)10
#define PLAYER_TEXT_INBETWEEN_SPACE WINDOW_WIDTH / (float)8

#define INPUT_FONT_SIZE WINDOW_HEIGHT / (float)20

#define HUNTER 1
#define CHARACTERS 10



int volSliderValue = 30; // Initial volume value
int prevVolSliderValue; // For slider to sync with mute/unmute button
bool draggingSlider = false;
bool musicMuted = false;



typedef struct MenuItem {
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    SDL_Rect backgroundPosition;
    SDL_Rect outlinePosition;
    SDL_Rect textPosition;
    int r, g, b; // Color of button
} MenuItem;

typedef struct textItem {
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    SDL_Rect textPosition;
} textItem;



MenuItem startGameButton, resumeGameButton, leaveGameButton, optionsButton, quitGameButton, 
         musicOnButton, musicOffButton, exitOptionsButton, ipInputBox, joinLobbyButton, backToMenuButton;

textItem volumeSlider, hunterWinsText, characterWinsText;
textItem playersText[MAX_PLAYERS];
SDL_Texture *background;

TTF_Font *Jacquard, *Roboto;


MenuItem createMenuItem(SDL_Renderer *renderer, TTF_Font *Font, char *text, int r, int g, int b, int y)
{   
    MenuItem menuItem;
    SDL_Color textColor = {0, 0, 0, SDL_ALPHA_OPAQUE};

    menuItem.renderer = renderer;
    menuItem.r = r;
    menuItem.g = g;
    menuItem.b = b;
    menuItem.backgroundPosition = (SDL_Rect){BUTTONS_X, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    menuItem.outlinePosition = (SDL_Rect){BUTTONS_X - OUTLINE_WIDTH / 2, y - OUTLINE_WIDTH / 2,
                                          BUTTON_WIDTH + OUTLINE_WIDTH, BUTTON_HEIGHT + OUTLINE_WIDTH};

    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Font, text, textColor);
    menuItem.texture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    menuItem.textPosition = (SDL_Rect){BUTTONS_X + 25, y + 25, BUTTON_WIDTH - 50, BUTTON_HEIGHT - 50};
    SDL_FreeSurface(surfaceMessage);
    return menuItem;
}
textItem createTextItem(SDL_Renderer *renderer, TTF_Font *Font, char *text, int r, int g, int b, int x, int y, int w, int h)
{
    textItem textItem;
    SDL_Color textColor = {r, g, b, SDL_ALPHA_OPAQUE};

    textItem.renderer = renderer;

    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Font, text, textColor);
    textItem.texture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    textItem.textPosition = (SDL_Rect){x, y, w, h};
    SDL_FreeSurface(surfaceMessage);

    return textItem;
}



void initMenu(SDL_Renderer *renderer)
{
    if(TTF_Init() == -1) {
        fprintf(stderr, "TTF could not initialize! TTF Error: %s\n", TTF_GetError());
        return;
    }
    Jacquard = TTF_OpenFont("../lib/assets/Jacquard24-Regular.ttf", 24);
    Roboto = TTF_OpenFont("../lib/assets/Roboto-Regular.ttf", 45);

    startGameButton = createMenuItem(renderer, Jacquard, "Join/Start Game", 1, 50, 32, TOP_BUTTON_Y);
    resumeGameButton = createMenuItem(renderer, Jacquard, "Resume Game", 1, 50, 32, TOP_BUTTON_Y);
    leaveGameButton = createMenuItem(renderer, Jacquard, "Leave Game", 139, 0, 0, BOTTOM_BUTTON_Y);
    optionsButton = createMenuItem(renderer, Jacquard, "Options", 105, 105, 105, MIDDLE_BUTTON_Y);
    quitGameButton = createMenuItem(renderer, Jacquard, "Quit Game", 139, 0, 0, BOTTOM_BUTTON_Y);
    musicOnButton = createMenuItem(renderer, Jacquard, "Music ON", 1, 50, 32, MIDDLE_BUTTON_Y);
    musicOffButton = createMenuItem(renderer, Jacquard, "Music OFF", 139, 0, 0, MIDDLE_BUTTON_Y);
    exitOptionsButton = createMenuItem(renderer, Jacquard, "Exit Options", 105, 105, 105, BOTTOM_BUTTON_Y);
    ipInputBox = createMenuItem(renderer, Jacquard, "", 255, 255, 255, TOP_BUTTON_Y);
    joinLobbyButton = createMenuItem(renderer, Jacquard, "Join Lobby", 1, 50, 32, MIDDLE_BUTTON_Y);
    backToMenuButton = createMenuItem(renderer, Jacquard, "Back to Menu", 139, 0, 0, BOTTOM_BUTTON_Y);
    
    volumeSlider = createTextItem(renderer, Jacquard, "Volume Slider", 255, 255, 255, BUTTONS_X + 25, SLIDER_Y - SLIDER_HEIGHT - BUTTON_SPACE_BETWEEN*2, BUTTON_WIDTH - 50, BUTTON_HEIGHT - 50);
    

    hunterWinsText = createTextItem(renderer, font, "The Hunter Wins", 255, 255, 255, WINDOW_WIDTH / 2 - 300, WINDOW_HEIGHT / 2 - 150, 600, 300);
    characterWinsText = createTextItem(renderer, font, "The characater Wins", 255, 255, 255, WINDOW_WIDTH / 2 - 300, WINDOW_HEIGHT / 2 - 150, 600, 300);

    background = IMG_LoadTexture(renderer, BACKGROUND_IMG_PATH);

 
    for (int i = 0; i < MAX_PLAYERS; i++)
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

            playersText[i] = createTextItem(renderer, Jacquard, result, 255, 255, 255, PLAYER_TEXT_OFFSET, PLAYER_TEXT_OFFSET + PLAYER_TEXT_INBETWEEN_SPACE * i, PLAYER_TEXT_HEIGHT * 2, PLAYER_TEXT_HEIGHT);

            // Free the dynamically allocated memory
            free(result);
        } else {
            fprintf(stderr ,"Memory allocation failed!\n");
        }

    }

}

void cleanupMenu()
{
    SDL_DestroyTexture(startGameButton.texture);
    SDL_DestroyTexture(resumeGameButton.texture);
    SDL_DestroyTexture(leaveGameButton.texture);
    SDL_DestroyTexture(optionsButton.texture);
    SDL_DestroyTexture(quitGameButton.texture);
    SDL_DestroyTexture(musicOnButton.texture);
    SDL_DestroyTexture(musicOffButton.texture);
    SDL_DestroyTexture(exitOptionsButton.texture);
    SDL_DestroyTexture(ipInputBox.texture);
    SDL_DestroyTexture(joinLobbyButton.texture);
    SDL_DestroyTexture(backToMenuButton.texture);
    SDL_DestroyTexture(volumeSlider.texture);
    SDL_DestroyTexture(gameOverText.texture);

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        SDL_DestroyTexture(playersText[i].texture);
    }

    SDL_DestroyTexture(background);
    TTF_CloseFont(Jacquard);
    TTF_CloseFont(Roboto);
}



void renderMenuItem(MenuItem *item)
{
    SDL_SetRenderDrawColor(item->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // Outline color
    SDL_RenderFillRect(item->renderer, &item->outlinePosition);

    SDL_SetRenderDrawColor(item->renderer, item->r, item->g, item->b, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(item->renderer, &item->backgroundPosition);

    SDL_RenderCopy(item->renderer, item->texture, NULL, &item->textPosition);
}
void renderTextItem(textItem *item)
{
    SDL_RenderCopy(item->renderer, item->texture, NULL, &item->textPosition);
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
        if (prevVolSliderValue == 0)
        {
            volSliderValue = 1;
        }
        else
        {
            volSliderValue = prevVolSliderValue;
        }
        // volSliderValue = (prevVolSliderValue != 0) ? prevVolSliderValue : 1; // Set to 1 if prevVolSliderValue is zero
        set_volume(volSliderValue);
    }
}



bool optionsMenu(SDL_Renderer *renderer)
{
    bool options = true;
    bool closeWindow = false;
    int mouseX, mouseY;
    float handleX;


    // Options loop
    while (options)
    {
        // Clear the screen
        SDL_RenderClear(renderer);

        // Render menu items and background
        SDL_RenderCopy(renderer, background, NULL, NULL);
        renderTextItem(&volumeSlider);
        handleX = (float)SLIDER_X + (float)(SLIDER_WIDTH - HANDLE_WIDTH) * ((float)volSliderValue / 100.0);

        if (volSliderValue != 0)
        {
            prevVolSliderValue = volSliderValue;
        }
        
        renderSlider(renderer, handleX);

        if (musicMuted)
        {
            renderMenuItem(&musicOffButton);
        }
        else
        {
            renderMenuItem(&musicOnButton);
        }
        renderMenuItem(&exitOptionsButton);

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
                default:
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
            default:
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
    SDL_Color textColor = {0, 0, 0, 255}; // Black text

    // Find game loop
    while (findGame)
    {
        // Clear the screen
        SDL_RenderClear(renderer);

        // Render background & menu button
        SDL_RenderCopy(renderer, background, NULL, NULL);
        renderMenuItem(&ipInputBox);
        renderMenuItem(&joinLobbyButton);
        renderMenuItem(&backToMenuButton);
        if (textBoxSelected)
        {
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, SDL_ALPHA_OPAQUE); // Outline color
            SDL_Rect outline = {BUTTONS_X - OUTLINE_WIDTH / 2, TOP_BUTTON_Y - OUTLINE_WIDTH / 2,
                                BUTTON_WIDTH + OUTLINE_WIDTH, BUTTON_HEIGHT + OUTLINE_WIDTH};
            int distance = 2;
            outline.x -= distance;
            outline.y -= distance;
            outline.w += 2*distance;
            outline.h += 2*distance;
            SDL_RenderDrawRect(renderer, &outline);
        }

        if (textLength > 0)
        {
            // Render the input text
            SDL_Surface *textSurface = TTF_RenderText_Solid(Roboto, inputText, textColor);
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

                    SDL_Rect textRect = {BUTTONS_X + BUTTON_WIDTH/2 - textW/2, TOP_BUTTON_Y + BUTTON_HEIGHT / 2 - INPUT_FONT_SIZE / 2, textW, textH};
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
                    if (textBoxSelected)
                        textBoxSelected = false; // Deselects the text box
                    else
                        findGame = false; // Returns to the menu
                    break;
                case SDL_SCANCODE_BACKSPACE:
                    if (textLength > 0)
                    {
                        inputText[textLength - 1] = '\0';
                        textLength--;
                    }
                    break;
                case SDL_SCANCODE_RETURN:
                    if (textLength > 0)
                    {
                        *joinGame = true;
                    }
                default:
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
                    *joinGame = true;
                }
                else if (mouseX >= BUTTONS_X && mouseX <= BUTTONS_X + BUTTON_WIDTH &&
                         mouseY >= BOTTOM_BUTTON_Y && mouseY <= BOTTOM_BUTTON_Y + BUTTON_HEIGHT)
                {
                    findGame = false;
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
        if (*joinGame == true)
        {
            strcpy(hostAddress, inputText);
            findGame = false;
        }
    }
    return closeWindow;
}

bool menu(SDL_Renderer *renderer, char hostAddress[MAX_ADDRESS_LENGTH], bool inGame)
{
    bool menu = true;
    bool closeWindow = false;
    bool joinGame = false;


    // Main menu loop
    while (menu)
    {
        // Clear the screen
        SDL_RenderClear(renderer);

        // Render background
        SDL_RenderCopy(renderer, background, NULL, NULL);
        if (inGame)
        {
            renderMenuItem(&resumeGameButton);
            renderMenuItem(&leaveGameButton);
        }
        else
        {
            renderMenuItem(&startGameButton);
            renderMenuItem(&quitGameButton);
        }
        renderMenuItem(&optionsButton);

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
                default:
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

void quitMenu(SDL_Renderer *renderer, int winner) {
    bool closeWindow = false;
    bool reachedTwoSeconds = false;
    clock_t initTimer = clock();


    while (!closeWindow && !reachedTwoSeconds) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);
        
        if (HUNTER == winner) {
            renderTextItem(&hunterWinsText);
        } else if (CHARACTERS == winner) {
            renderTextItem(&characterWinsText);
        }
        SDL_RenderPresent(renderer);

        clock_t quitMenuTimer = clock();
        double seconds = (double)(quitMenuTimer - initTimer) / CLOCKS_PER_SEC;

        if (seconds >= 2.0) {
            reachedTwoSeconds = true;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    closeWindow = true;
                    break;
            }   
        }
}

}

void drawLobby(SDL_Renderer *renderer, int readyPlayers[MAX_PLAYERS], int players)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, background, NULL, NULL);
    
    for (int i = 0; i < players; i++)
    {
        renderTextItem(&playersText[i]);
        
        SDL_Rect rect = {PLAYER_TEXT_HEIGHT * 2 + PLAYER_TEXT_OFFSET + 20, PLAYER_TEXT_OFFSET + i * PLAYER_TEXT_INBETWEEN_SPACE,
                         PLAYER_TEXT_HEIGHT, PLAYER_TEXT_HEIGHT};
        if (i == 0)
        {

            SDL_Rect outline = rect;
            int distance = 5;
            outline.x -= distance;
            outline.y -= distance;
            outline.w += 2*distance;
            outline.h += 2*distance;
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
            SDL_RenderDrawRect(renderer, &outline);
        }
        if (readyPlayers[i])
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_RenderPresent(renderer);
}
