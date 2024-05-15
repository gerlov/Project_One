#ifndef ESCAPE_PORTAL_H
#define ESCAPE_PORTAL_H

#include "tilemap.h"
#include "our_rand.h"
#include "mazeview.h"
#include "character.h"
#include "texture.h"
#include "music.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


///@brief Initialize the portal inside the tile map
///@param renderer Pointer to the SDL_Renderer used for rendering.
///@param tilemap Parameter for the game map
void initPortal(SDL_Renderer* renderer, TileMap* tilemap);

///@brief Initialize the portal inside the tile map
///@param renderer Ponter to renderer
///@param tilemap Parameter to check positioning inside tilemap
void drawPortal(SDL_Renderer* renderer, TileMap* tilemap);

///@brief Cleans the image resource
void cleanUpPortalResources();

#endif // ESCAPE_PORTAL_H
