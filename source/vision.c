#include "vision.h"

#include <math.h>

#define VISION_RESOLUTION 360

SDL_Texture* mask_vision(SDL_Renderer *renderer, SDL_Texture *texture)
{

    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    // set the texture as the render target
    SDL_SetRenderTarget(renderer, texture);
    // copy the texture to the surface
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch);

    int colorBound = 0;
    // loop through all pixels
    for (int i = 0; i < width * height; i++)
    {
        Uint32 *pixel = (Uint32 *)surface->pixels + i;
        Uint8 r, g, b, a;
        SDL_GetRGBA(*pixel, surface->format, &r, &g, &b, &a);
        if (r > colorBound)
        {
            // make the pixels red color the alpha value and set the color to black
            *pixel = SDL_MapRGBA(surface->format, 0, 0, 0, 255-r);
        }
    }
    return SDL_CreateTextureFromSurface(renderer, surface);
}

void draw_vision(SDL_Renderer *renderer, TileMap *tilemap, SDL_Rect *rect, int radius, int windowWidth, int windowHeight)
{

    SDL_Color endColor = {0, 0, 0, 255};
    SDL_Color centerColor = {255, 255, 255, 255};
    SDL_FPoint center = {rect->x + rect->w / 2, rect->y + rect->h / 2};

    SDL_Vertex vertices[VISION_RESOLUTION];
    send_rays(tilemap, &center, radius, &centerColor, &endColor, vertices);

    // create a texture to draw the vision on
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, windowWidth, windowHeight);
    SDL_SetRenderTarget(renderer, texture);
    
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);

    SDL_SetRenderDrawColor(renderer, endColor.r, endColor.g, endColor.b, endColor.a);
    SDL_RenderClear(renderer);

    // draw the non occluded vision
    for (int i = 0; i < VISION_RESOLUTION; i++)
    {
        SDL_Vertex vertices2[3] = {
            vertices[i],
            vertices[(i + 1) % VISION_RESOLUTION],
            {center, centerColor},
        };
        SDL_RenderGeometry(renderer, NULL, vertices2, 3, NULL, 0);
    }

    texture = mask_vision(renderer, texture);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}
float send_ray(TileMap *tilemap, SDL_FPoint *center, float angle, int radius)
{
    // TODO: this function could be optimized
    float dx = cos(angle);
    float dy = sin(angle);
    float x = center->x;
    float y = center->y;
    for (int i = 0; i < radius; i++)
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
    return (float)radius;
}

void send_rays(TileMap *tilemap, SDL_FPoint *center, int radius, SDL_Color *centerColor, SDL_Color *endColor, SDL_Vertex *vertices)
{
    SDL_Rect screenRect = {0, 0, 1200, 700};
    int vertexCount = 0;
    float angle = (360.0 / VISION_RESOLUTION) * M_PI / 180;
    for (int i = 0; i < VISION_RESOLUTION; i++)
    {
        float finalAngle = i * angle;
        float distance = send_ray(tilemap, center, finalAngle, radius);
        float t = distance / radius;
        // interpolate the color
        SDL_Color currentColor = {
            (Uint8)((1 - t) * centerColor->r + t * endColor->r),
            (Uint8)((1 - t) * centerColor->g + t * endColor->g),
            (Uint8)((1 - t) * centerColor->b + t * endColor->b),
            (Uint8)((1 - t) * centerColor->a + t * endColor->a)};
        // create a vertex
        SDL_FPoint rayEnd = {center->x + cos(finalAngle) * distance, center->y + sin(finalAngle) * distance};
        vertices[vertexCount++] = (SDL_Vertex){rayEnd, currentColor};
    }
}