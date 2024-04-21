#include "limitedvision.h"

void initLimitedVision(LimitedVision *lv, SDL_Renderer *renderer, TileMap *tilemap, int w, int h, int radius)
{
    lv->w = w;
    lv->h = h;
    lv->radius = radius;
    lv->renderer = renderer;
    lv->tilemap = tilemap;
    lv->buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
}

/// @author javdix9
/// @link https://www.youtube.com/watch?v=NbSee-XM7WA
/// @brief Casts a ray in a direction using the DDA algorithm
float rayCast(LimitedVision *lv, SDL_FPoint *center, int angle)
{
    // Calculate the direction vector based on the angle
    float dirX = cos(angle * M_PI / 180);
    float dirY = sin(angle * M_PI / 180);
    // Start position of the ray
    float posX = center->x;
    float posY = center->y;

    for (int i = 0; i < lv->radius; i++)
    {
        SDL_Point mapPos = {posX / lv->tilemap->tile_size, posY / lv->tilemap->tile_size};
        // if (mapPos.x < 0 || mapPos.y < 0 || mapPos.x >= lv->tilemap->width || mapPos.y >= lv->tilemap->height)
        // {
        //     return (float)i;
        // }
        Tile *tile = &lv->tilemap->tiles[mapPos.y][mapPos.x];
        // printf("Tile type: %d\n", tile->type);
        if (tile->type == TILE_EMPTY)
        {
            return (float)i;
        }
        posX += dirX;
        posY += dirY;

    }
    return (float)lv->radius;

}

void drawtobuffer(LimitedVision *lv, SDL_FPoint center)
{
}

void drawLimitedVision(LimitedVision *lv, SDL_FPoint center)
{
    static SDL_Color centerColor = {255, 255, 255, 255};
    static SDL_Color edgeColor = {0, 0, 0, 255};

    // Clear the buffer
    SDL_SetRenderTarget(lv->renderer, lv->buffer);
    SDL_SetRenderDrawColor(lv->renderer, edgeColor.r, edgeColor.g, edgeColor.b, edgeColor.a);
    SDL_RenderClear(lv->renderer);
    int numRays = 200;
    SDL_Vertex v[numRays];

    // Cast rays in all directions
    for (int i = 0; i < numRays; i++)
    {
        float angle = i * 360 / numRays;
        

        float distance = rayCast(lv, &center, angle);
        float t = distance / lv->radius;
        SDL_Color interpolatedColor = {
            (Uint8)((1-t) * centerColor.r + t * edgeColor.r),
            (Uint8)((1-t) * centerColor.g + t * edgeColor.g),
            (Uint8)((1-t) * centerColor.b + t * edgeColor.b),
            255
        };
        SDL_FPoint dir = {center.x + cos(angle * M_PI / 180)*distance, center.y + sin(angle * M_PI / 180)*distance};
        v[i].position = dir;
        v[i].color = interpolatedColor;
    }
    SDL_FPoint p = {center.x-lv->tilemap->camera.x, center.y-lv->tilemap->camera.y};
    for (int i = 0; i < numRays; i++)
    {
        SDL_Vertex v1[3];
        v1[0] = (SDL_Vertex){p, centerColor};
        SDL_Vertex temp = v[i];
        temp.position.x -= lv->tilemap->camera.x;
        temp.position.y -= lv->tilemap->camera.y;
        v1[1] = temp;
        temp = v[(i + 1) % numRays];
        temp.position.x -= lv->tilemap->camera.x;
        temp.position.y -= lv->tilemap->camera.y;
        v1[2] = temp;
        SDL_RenderGeometry(lv->renderer,NULL, v1, 3, NULL, 0);
    }
    
    // SDL_Rect r = {center.x - lv->radius - lv->tilemap->camera.x, center.y - lv->radius - lv->tilemap->camera.y, lv->radius * 2, lv->radius * 2};
    // SDL_RenderDrawRect(lv->renderer, &r);
    SDL_SetRenderTarget(lv->renderer, NULL);
    SDL_SetTextureBlendMode(lv->buffer, SDL_BLENDMODE_MOD);
    SDL_SetRenderDrawColor(lv->renderer, 0,0,0,255);
    // Draw the buffer
    SDL_RenderCopy(lv->renderer, lv->buffer, NULL, NULL);
}
