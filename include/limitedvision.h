#ifndef LIMITEDVISION_H
#define LIMITEDVISION_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "tilemap.h"
typedef struct _LimitedVision
{
    int w, h;
    int radius;
    TileMap *tilemap;
    SDL_Texture *buffer;
    SDL_Renderer *renderer;
} LimitedVision;

/// @brief Inits the LimitedVision struct
/// @param lv the LimitedVision struct to be initialized
/// @param renderer the renderer to draw to
/// @param tilemap a pointer to the tilemap to raycast against
/// @param w screen width
/// @param h screen height
/// @param radius maximum distance to raycast
void init_LimitedVision(LimitedVision *lv, SDL_Renderer *renderer, TileMap *tilemap, int w, int h, int radius);

/// @brief cast a ray from the center of the limited vision to the angle
/// @param lv the limited vision to cast the ray from
/// @param center the starting point of the ray
/// @param angle the angle of the ray
/// @return the distance until the ray hits a wall or the maximum distance (set by limitedVision.radius), whichever comes first
float rayCast(LimitedVision *lv, SDL_FPoint *center, int angle);

/// @brief draws the limited vision to a buffer
/// @param lv the limited vision to draw
/// @param center the center of the limited vision
/// @param v the vertices of the limited vision
/// @param numRays the amount vertices
/// @param centerColor the color of the center
/// @param edgeColor the color of the edges
void drawtobuffer(LimitedVision *lv, SDL_FPoint center, SDL_Vertex *v, int numRays, SDL_Color centerColor, SDL_Color edgeColor);

/// @brief draws the limited vision to the screen
/// @param lv the limited vision to draw
/// @param center the center of the limited vision
void drawLimitedVision(LimitedVision *lv, SDL_FPoint center);

/// @brief set the radius of the limited vision
/// @param lv the limited vision to set the radius of
/// @param radius the new radius
void setLimitedVisionRadius(LimitedVision *lv, int radius);

#endif // LIMITEDVISION_H