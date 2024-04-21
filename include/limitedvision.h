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

typedef struct rayOutput
{
    float distance;
    int tileType;
} rayOutput;


void initLimitedVision(LimitedVision *lv, SDL_Renderer *renderer, TileMap *tilemap, int w, int h, int radius);

float rayCast(LimitedVision *lv, SDL_FPoint *center, int angle);

void drawtobuffer(LimitedVision *lv, SDL_FPoint center, SDL_Vertex *v, int numRays, SDL_Color centerColor, SDL_Color edgeColor);

void drawLimitedVision(LimitedVision *lv, SDL_FPoint center);

#endif // LIMITEDVISION_H