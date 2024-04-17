#ifndef VISION_H
#define VISION_H
#include "tilemap.h"
#include <SDL2/SDL.h>

void draw_circle(SDL_Renderer *renderer, SDL_Rect *rect, int radius);

float send_ray(SDL_Renderer *renderer, TileMap *tilemap, SDL_FPoint *center, float angle, int max_distance);
void send_rays(SDL_Renderer *renderer, TileMap *tilemap, SDL_Rect *rect);

#endif // VISION_H