#include "tilemap.h"


void tilemap_init(TileMap* tilemap, int width, int height, int tile_size)
{
    tilemap->width = width;
    tilemap->height = height;
    tilemap->tiles = malloc(sizeof(Tile) * width * height);
    tilemap->tile_size = tile_size;
    for(int i = 0; i < width * height; i++)
    {
        tilemap->tiles[i].id = 0;
        tilemap->tiles[i].texture = NULL;
    }

}

void tilemap_set_tile(TileMap* tilemap, int x, int y, Tile* tile)
{
    tilemap->tiles[y * tilemap->width + x] = *tile;
}

void tilemap_draw(TileMap* tilemap, SDL_Renderer* renderer, SDL_Rect* camera)
{
    for(int y = 0; y < tilemap->height; y++)
    {
        for(int x = 0; x < tilemap->width; x++)
        {
            Tile* tile = get_tile(tilemap, x, y);
            if(tile->texture)
            {
                SDL_Rect dest = {
                    x * tilemap->tile_size - camera->x,
                    y * tilemap->tile_size - camera->y,
                    tilemap->tile_size,
                    tilemap->tile_size
                };
                SDL_RenderCopy(renderer, tile->texture, NULL, &dest);
            }
        }
    }

}

Tile* get_tile(TileMap* tilemap, int x, int y)
{
    return &tilemap->tiles[y * tilemap->width + x];
}

void tilemap_free(TileMap* tilemap)
{
    for(int i = 0; i < tilemap->width * tilemap->height; i++)
    {
        if(tilemap->tiles[i].texture)
        {
            SDL_DestroyTexture(tilemap->tiles[i].texture);
        }
    }
}
