#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL_ttf.h>
#define MAX_ADDRESS_LENGTH 16

/// @brief Struct for the menu items
/// @param texture The texture of the image
/// @param renderer The renderer to draw the item to
/// @param position The position of the image
typedef struct MenuItem {
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    SDL_Rect position;
} MenuItem;

/// @brief Function for rendering all images displayed
/// @param item Struct containing the texture, renderer and the position of the image
/// @return returns true if there is an error loading the image
bool renderMenuItem(MenuItem *item);

/// @brief Function for creating a menu button
/// @param renderer The renderer to draw the button to
/// @param text The text to display on the button
/// @param r The red value of the button
/// @param g The green value of the button
/// @param b The blue value of the button
/// @param y The y position of the button
void createMenuButton(SDL_Renderer* renderer, char* text, int r, int g, int b, int y);

/// @brief Function for drawing text to the screen
/// @param renderer The renderer to draw the text to
/// @param color The color of the text
/// @param font The font of the text
/// @param text The text to display
/// @param x The x position of the text
/// @param y The y position of the text
/// @param w The width of the text
/// @param h The height of the text
void drawText(SDL_Renderer *renderer, SDL_Color color, TTF_Font *font, char *text, int x, int y, int w, int h);

/// @brief Function for rendering the volume slider
/// @param renderer The renderer to draw the slider to
/// @param handleX The x position of the volume sliders handle
void renderSlider(SDL_Renderer *renderer, float handleX);

/// @brief Function for the toggling of the music as well as syncing with mute/unmute button
void toggle_music_logic(void);

/// @brief Main function for the options tab
/// @param renderer The renderer to draw the options tab to
/// @return returns true if window should be closed
bool optionsMenu(SDL_Renderer* renderer);

/// @brief Find game function for connecting to a server
/// @param renderer The renderer to draw the graphics to
/// @param hostAddress The address of the server to connect to
/// @param joinGame A boolean to whether to join the game or not
/// @return returns true if window should be closed
bool findGame(SDL_Renderer *renderer, char hostAddress[MAX_ADDRESS_LENGTH], bool *joinGame);

/// @brief Main code for the pause menu
/// @param renderer The renderer to draw the menu to
/// @param hostAddress The address of the server to connect to
/// @param inGame A boolean to check if the game is in progress
/// @return returns true if window should be closed
bool pauseMenu(SDL_Renderer* renderer, char hostAddress[MAX_ADDRESS_LENGTH], bool inGame);

/// @brief Function for drawing the lobby
/// @param renderer The renderer to draw the lobby to
/// @param readyPlayers An array of the players that are ready
/// @param players The amount of players in the lobby
void drawLobby(SDL_Renderer *renderer, int readyPlayers[6], int players);

#endif // MENU_H