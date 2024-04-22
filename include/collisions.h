#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <SDL2/SDL.h>
#include "tilemap.h"

///@brief Checks for collisions between a PLAYER rectangle and specific tile types in a tile mapM
///@param nextPosition Pointer to the SDL_Rect structure representing the position to check for collisions.
///@param tilemap Pointer to the TileMap containing the tiles to check against  
///@param tiletype The type of tile to check for collisions with.
///@return Returns true if a collision is detected, false otherwise   
bool collides(SDL_Rect *nextPosition, TileMap *tilemap, TileType tiletype);


#endif 