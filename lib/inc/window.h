#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

/// @brief the SDL library, creates a window, and creates a renderer for the window. 
/// @param window A pointer to an SDL_Window pointer. This function will assign the created window to *window.
/// @param renderer A pointer to an SDL_Renderer pointer. This function will assign the created renderer to *renderer.
/// @param WINDOW_WIDTH The width of the window to be created.
/// @param WINDOW_HEIGHT The height of the window to be created.
/// @return Returns 0 on success, and a non-zero error code on failure. On failure, *window and *renderer will not be set.
int init_SDL_window(SDL_Window **window, SDL_Renderer **renderer, int WINDOW_WIDTH, int WINDOW_HEIGHT);


/// @brief Cleans up the SDL resources: the renderer and the window, and then quits SDL.
/// @param window A pointer to the SDL_Window to be destroyed. If NULL, no action is taken.
/// @param renderer A pointer to the SDL_Renderer to be destroyed. If NULL, no action is taken.
void cleanup_SDL(SDL_Window* window, SDL_Renderer* renderer);

#endif
