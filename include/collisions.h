#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <SDL2/SDL.h>
#include "tilemap.h"
#include "character.h" 

/// @brief Checks for collision between player and world boundaries or solid tiles.
/// @param nextPosition Pointer to an SDL_Rect for next position.
/// @param tilemap Pointer to a TileMap for tile properties.
/// @param windowWidth Width of the game window.
/// @param windowHeight Height of the game window.
/// @return Returns true if collision is detected, false otherwise.
bool collides(SDL_Rect *nextPosition, TileMap *tilemap, int windowWidth, int windowHeight);


/// @brief Checks for collision between two (character) rectangles.   
/// @param rect1 Pointer to the first SDL_Rect representing the first character's position and size.
/// @param rect2 Pointer to the second SDL_Rect representing the second character's position and size.
/// @return Returns true if the two rectangles intersect (collision detected), false otherwise.
bool characters_collide(SDL_Rect *rect1, SDL_Rect *rect2); 


#endif // COLLISIONS_H
 