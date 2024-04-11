/**
 * @brief Checks for collision between player object and the world (window boundaries and solid tiles).  
 * @param nextPosition Pointer to an SDL_Rect representing the entity's next position.  
 * @param tilemap Pointer to a TileMap containing the layout of the game world's tiles.
 *                Is used to access tile properties and determine if any solid tiles are in collision.
 * @param windowWidth The width of the game window, used to check for collisions with the right boundary.
 * @param windowHeight The height of the game window, used to check for collisions with the bottom boundary.
 * 
 * @return Returns true if a collision is detected (with either the window boundaries or solid tiles),
 *         false otherwise. 
 */

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <SDL2/SDL.h>
#include "tilemap.h"  

bool collides(SDL_Rect *nextPosition, TileMap *tilemap, int windowWidth, int windowHeight); 


#endif // COLLISIONS_h  



