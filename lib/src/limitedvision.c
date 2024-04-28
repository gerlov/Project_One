#include "limitedvision.h"
#include <math.h>
#define TO_RADIANS(angle) ((angle) * M_PI / 180)

void init_LimitedVision(LimitedVision *lv, SDL_Renderer *renderer, TileMap *tilemap, int w, int h, int radius)
{
    lv->w = w;
    lv->h = h;
    lv->radius = radius;
    lv->renderer = renderer;
    lv->tilemap = tilemap;
    lv->buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
}

float rayCast(LimitedVision *lv, SDL_FPoint *center, int angle)
{
    // Calculate the direction vector based on the angle
    float dirX = cos(TO_RADIANS(angle));
    float dirY = sin(TO_RADIANS(angle));
    // Start position of the ray
    float posX = center->x;
    float posY = center->y;

    // Cast the ray until it hits a wall or the maximum distance
    for (int i = 0; i < lv->radius; i++)
    {
        SDL_Point mapPos = {posX / lv->tilemap->tile_size, posY / lv->tilemap->tile_size};
        Tile *tile = &lv->tilemap->tiles[mapPos.y][mapPos.x];
        // TILE_Empty is only inbetween walls, if it was TILE_WALL the wall would not be seen.
        if (tile->type == TILE_EMPTY)
        {
            return (float)i;
        }
        // using the slow method of incrementing the position by the direction vector, this can be optimized but I couldn't get it to work
        posX += dirX;
        posY += dirY;
    }
    // Return the maximum distance if no wall was hit
    return (float)lv->radius;
}

void drawtobuffer(LimitedVision *lv, SDL_FPoint center, SDL_Vertex *v, int vertex_amount, SDL_Color centerColor, SDL_Color edgeColor)
{
    SDL_SetRenderTarget(lv->renderer, lv->buffer);
    SDL_SetRenderDrawColor(lv->renderer, edgeColor.r, edgeColor.g, edgeColor.b, edgeColor.a);
    SDL_RenderClear(lv->renderer);
    SDL_FPoint p = {center.x - lv->tilemap->camera.x, center.y - lv->tilemap->camera.y};

    SDL_Vertex v1[vertex_amount * 3];
    for (int i = 0; i < vertex_amount; i++)
    {
        int index = i * 3;
        SDL_Vertex temp = v[i];
        v1[index] = (SDL_Vertex){p, centerColor};
        v1[index + 1] = temp;
        temp = v[(i + 1) % vertex_amount];
        v1[index + 2] = temp;
    }
    SDL_RenderGeometry(lv->renderer, NULL, v1, vertex_amount * 3, NULL, 0);
    SDL_SetRenderTarget(lv->renderer, NULL);
    SDL_SetTextureBlendMode(lv->buffer, SDL_BLENDMODE_MOD);
    SDL_SetRenderDrawColor(lv->renderer, 0, 0, 0, 255);
}

void drawLimitedVision(LimitedVision *lv, SDL_FPoint center)
{
    static SDL_Color centerColor = {255, 255, 255, 255};
    static SDL_Color edgeColor = {0, 0, 0, 255};

    int numRays = 200;
    SDL_Vertex v[numRays];

    // Cast rays in all directions
    for (int i = 0; i < numRays; i++)
    {
        // Calculate the angle of the ray
        float angle = i * 360 / numRays;
        

        float distance = rayCast(lv, &center, angle);
        // Interpolate the color based on the distance so it is equally colored
        float t = distance / lv->radius;
        SDL_Color interpolatedColor = {
            (Uint8)((1 - t) * centerColor.r + t * edgeColor.r),
            (Uint8)((1 - t) * centerColor.g + t * edgeColor.g),
            (Uint8)((1 - t) * centerColor.b + t * edgeColor.b),
            255};
        // Calculate the end point of the ray
        SDL_FPoint dir = {
            center.x + cos(TO_RADIANS(angle)) * distance - lv->tilemap->camera.x,
            center.y + sin(TO_RADIANS(angle)) * distance - lv->tilemap->camera.y};
        v[i].position = dir;
        v[i].color = interpolatedColor;
    }
    // Draw the limited vision to the buffer
    drawtobuffer(lv, center, v, numRays, centerColor, edgeColor);
    // Draw the buffer to the screen
    SDL_RenderCopy(lv->renderer, lv->buffer, NULL, NULL);
}
