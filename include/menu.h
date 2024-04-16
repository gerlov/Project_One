#ifndef MENU_H
#define MENU_H

/// @brief Enum for the game state
/// @param PAUSED The game is paused
/// @param PLAYING The game is playing
/// @param QUIT The game is quitting
typedef enum {
    PAUSED,
    PLAYING,
    QUIT
} GameState;

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
bool renderMenuItem(MenuItem item);

bool optionsMenu(SDL_Renderer* renderer);

/// @brief Main code for the menu
/// @param renderer The renderer to draw the menu to
bool mainMenu(SDL_Renderer* renderer);




#endif // MENU_H