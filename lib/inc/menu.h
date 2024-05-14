#ifndef MENU_H
#define MENU_H

#include "data.h"

#define MAX_ADDRESS_LENGTH 16

/// @brief Initializes the menu items and texts once for the sake of saving memory
/// @param renderer The renderer to draw the menu to
void initMenu(SDL_Renderer *renderer);

/// @brief instead of having two functions for the menu, this function takes a bool that tells the function if the game just started or if it was paused this way we can have the same function for both cases, false for start and true for pause
/// @param renderer The renderer to draw the menu to
/// @param hostAddress The address of the server to connect to
/// @param inGame A boolean to check if the game is in progress
/// @return returns true if window should be closed
bool menu(SDL_Renderer* renderer, char hostAddress[MAX_ADDRESS_LENGTH], bool inGame);

void quitMenu(SDL_Renderer* renderer);

/// @brief Function for drawing the lobby
/// @param renderer The renderer to draw the lobby to
/// @param readyPlayers An array of the players that are ready
/// @param players The amount of players in the lobby
void drawLobby(SDL_Renderer *renderer, int readyPlayers[MAX_PLAYERS], int players);

#endif // MENU_H