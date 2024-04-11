#ifndef MENU_H
#define MENU_H

/// @brief Function for rendering all images displayed
/// @param renderer The renderer to draw the images to
/// @param file The file to be displayed
/// @param x x coordinate of where the file should be displayed
/// @param y y coordinate of where the file should be displayed
/// @param width width of the image
/// @param height height of the image
/// @return returns true if there is an error loading the image
bool renderMenuItem(SDL_Renderer* renderer, const char* file, int x, int y, int width, int height);

bool options(SDL_Renderer* renderer);

/// @brief Main code for the menu
/// @param renderer The renderer to draw the menu to
bool open_menu(SDL_Renderer* renderer);




#endif // MENU_H