#include "vision.h"

#include <math.h>

SDL_Texture *pVingette = NULL;

float send_ray(SDL_Renderer *renderer, TileMap *tilemap, SDL_FPoint *center, float angle, int max_distance)
{
    float dx = cos(angle);
    float dy = sin(angle);
    float x = center->x;
    float y = center->y;
    for (int i = 0; i < max_distance; i++)
    {
        int mapX = (int)x;
        int mapY = (int)y;
        mapX /= tilemap->tile_size;
        mapY /= tilemap->tile_size;
        Tile *tile = get_tile(tilemap, mapX, mapY);
        if (tile->type == TILE_EMPTY)
        {
            return (float)i;
        }

        x += dx;
        y += dy;
    }
    return (float)max_distance;
}

void send_rays(SDL_Renderer *renderer, TileMap *tilemap, SDL_Rect *rect)
{
    
    SDL_Color endColor = {0, 0, 0, 255};
    SDL_Color centerColor = {255, 255, 255, 255};
    SDL_Rect screenRect = {0, 0, 1200, 700};
    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    // SDL_RenderFillRect(renderer, NULL);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MUL);
    int radius = 250;
    int vertexCount = 0;
    int vertexAmount = 300;
    SDL_Vertex vertices[vertexAmount];
    SDL_FPoint center = {rect->x + rect->w / 2, rect->y + rect->h / 2};

    float angle = (360.0/vertexAmount) * M_PI / 180;
    for (int i = 0; i < vertexAmount; i++)
    {
        float finalAngle = i * angle;
        float distance = send_ray(renderer, tilemap, &center, finalAngle, radius);
        float t = distance / radius;
        SDL_Color currentColor = {
            (Uint8)((1 - t) * centerColor.r + t * endColor.r),
            (Uint8)((1 - t) * centerColor.g + t * endColor.g),
            (Uint8)((1 - t) * centerColor.b + t * endColor.b),
            (Uint8)((1 - t) * centerColor.a + t * endColor.a)
        };
        SDL_FPoint rayEnd = {center.x + cos(finalAngle) * distance, center.y + sin(finalAngle) * distance};
        vertices[vertexCount++] = (SDL_Vertex){rayEnd, currentColor};
    }
    for (int i = 0; i < vertexAmount; i++)
    {
        SDL_Vertex vertices2[3] = {
            vertices[i],
            vertices[(i + 1)%vertexAmount],
            {center, centerColor},
        };
        SDL_RenderGeometry(renderer,NULL, vertices2, 3, NULL, 0);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}