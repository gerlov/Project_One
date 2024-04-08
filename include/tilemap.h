#ifndef TILEMAP_H
#define TILEMAP_H
#include <SDL2/SDL.h>
#include <stdbool.h>

/// @brief Struct that represents a tile
typedef struct Tile
{
    int id; // tile id. Used to determine which texture and properties should be loaded for this tile !UNIPLEMENTED!
    int is_solid; // if this tile is solid or not
    SDL_Texture* texture; // texture of the tile
    SDL_Rect src_rect; // source rectangle of the texture
} Tile;
/// @brief Struct that represents a tilemap
typedef struct TileMap
{
    int width; // in tiles
    int height; // in tiles
    Tile* tiles; // 1D array of tiles
    int tile_size; // in pixels
} TileMap;

/// @brief Struct to load tilemap data from a file
typedef struct TileMapData
{
    int width; // in tiles
    int height; // in tiles
    int tile_size; // in pixels
    int* tiles; // 1D array of tile ids
} TileMapData;

/// @brief Inits a tilemap
/// @param tilemap the tilemap to init
/// @param width the width of the tilemap in tiles
/// @param height the height of the tilemap in tiles
/// @param tile_size the size of each tile in pixels (square)
void tilemap_init(TileMap* tilemap, int width, int height, int tile_size);

/// @brief Sets a tile in the tilemap
/// @param tilemap the tilemap to set the tile in
/// @param x the x position of the tile
/// @param y the y position of the tile
/// @param tile the tile that is replacing the tile at x, y
void tilemap_set_tile(TileMap* tilemap, int x, int y, Tile* tile);

/// @brief Draws the tilemap
/// @param tilemap the tilemap to draw
/// @param renderer the renderer to draw the tilemap to
/// @param camera the camera rectangle. So the tilemap can be drawn relative to the camera
void tilemap_draw(TileMap* tilemap, SDL_Renderer* renderer, SDL_Rect* camera);

/// @brief Gets a tile from the tilemap at x, y
Tile* get_tile(TileMap* tilemap, int x, int y);

/// @brief Frees the tilemap and all of its textures
void tilemap_free(TileMap* tilemap);
#endif // TILEMAP_H