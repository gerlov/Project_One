#ifndef VISION_H
#define VISION_H
#include "tilemap.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/// @brief Make the rest of the vision black. Sets the red color to the alpha value
/// @param renderer 
/// @param texture the texture to mask
/// @return A new texture with the vision masked
SDL_Texture* mask_vision(SDL_Renderer *renderer, SDL_Texture *texture);

/// @brief Draws the vision of the player
/// @param renderer 
/// @param tilemap 
/// @param rect rectangle of the player
/// @param radius the maximum distance of the vision
/// @param windowWidth the width of the screen
/// @param windowHeight the height of the screen
void draw_vision(SDL_Renderer *renderer, TileMap *tilemap, SDL_Rect *rect, int radius, int windowWidth, int windowHeight);

/// @brief Sends a singular ray to the tilemap to check for occlusions
/// @param tilemap tilemap to check for occlusions
/// @param center the starting point of the ray
/// @param angle angle to send the ray (degrees)
/// @param radius maximum distance of the ray
/// @return Distance to the occlusion
float send_ray(TileMap *tilemap, SDL_FPoint *center, float angle, int radius);

/// @brief Sends rays to the tilemap to check for occlusions and returns the vertices of the rays
/// @param tilemap 
/// @param center the center of the vision
/// @param radius the maximum distance of the rays
/// @param centerColor the center color of the vision
/// @param endColor the end color of the vision
/// @param vertices the vertices of the rays (OUT)
void send_rays(TileMap *tilemap, SDL_FPoint *center, int radius, SDL_Color *centerColor, SDL_Color *endColor, SDL_Vertex *vertices);


#endif // VISION_H