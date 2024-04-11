#ifndef MENU_H
#define MENU_H

/// @brief Struct that represents menu buttons
typedef struct MenuItem 
{
    char* text; // The text that the button displays
    int x_start, x_end; // Start and stop x-coordinates
    int y_start, y_end; // Start and stop y-coordinates
    int selected; // If the button is selected or not
} MenuItem;

/// @brief Main code for the menu
/// @param renderer The renderer to draw the menu to
bool open_menu(SDL_Renderer* renderer);




#endif // MENU_H