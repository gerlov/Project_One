#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <SDL2/SDL.h>
#include "tilemap.h"

/// @brief Checks for collision between player and world boundaries or solid tiles.
/// @param nextPosition Pointer to an SDL_Rect for next position.
/// @param tilemap Pointer to a TileMap for tile properties.
/// @param windowWidth Width of the game window.
/// @param windowHeight Height of the game window.
/// @return Returns true if collision is detected, false otherwise.
bool collides(SDL_Rect *nextPosition, TileMap *tilemap, int windowWidth, int windowHeight);

#endif // COLLISIONS_H
