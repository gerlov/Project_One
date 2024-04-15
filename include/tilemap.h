#ifndef TILEMAP_H
#define TILEMAP_H
#include <SDL2/SDL.h>
#include <stdbool.h>

typedef enum TileType {
    TILE_EMPTY,
    TILE_WALL,
    TILE_FLOOR,
    TILE_DECOR,
    TILE_WIN
} TileType;

/// @brief Enum that represents a direction. Used for the orientation of the walls as a hash
typedef enum Direction
{
    NONE = 0,
    UP = 1,
    RIGHT = 2,
    DOWN = 4,
    LEFT = 8,
    TOP_RIGHT = 16,
    BOTTOM_RIGHT = 17,
    BOTTOM_LEFT = 18,
    TOP_LEFT = 19
} Direction;

/// @brief Struct that represents a tile
typedef struct Tile
{
    int id;            // tile id. Used to determine which texture and properties should be loaded for this tile !UNIPLEMENTED!
    int is_solid;      // if this tile is solid or not
    TileType type;     // type of the tile
    SDL_Rect src_rect; // source rectangle of the texture
} Tile;
/// @brief Struct that represents a tilemap
typedef struct TileMap
{
    SDL_Renderer *pRenderer; // renderer to load the textures with
    SDL_Texture *pTexture;   // texture of the tilemap
    SDL_Texture *pFloorTexture; // texture of the floor
    int x;                   // x position of the tilemap
    int y;                   // y position of the tilemap
    int width;               // in tiles
    int height;              // in tiles
    Tile *tiles;             // 1D array of tiles
    int tile_size;           // in pixels
} TileMap;


/// @brief Inits a tilemap
/// @param tilemap the tilemap to init
/// @param renderer the renderer to load the textures with
/// @param width the width of the tilemap in tiles
/// @param height the height of the tilemap in tiles
/// @param tile_size the size of each tile in pixels (square)
void tilemap_init(TileMap *tilemap, SDL_Renderer *renderer, int width, int height, int tile_size);

/// @brief Loads the selected map into the tilemap
/// @param tilemap  The tilemap to load the data into
/// @param map_id The id of the map to load
void tilemap_load(TileMap *tilemap, int map_id);

/// @brief Loads a tilemap from a file !UNIMPLEMENTED!
/// @param tilemap output tilemap
/// @param file_path path to the file to load
void tilemap_load_file(TileMap *tilemap, const char *file_path);

/// @brief Saves a tilemap to a file !UNIMPLEMENTED!
/// @param tilemap the tilemap to save
/// @param file_path the path to save the tilemap to
void tilemap_save_file(TileMap *tilemap, const char *file_path);

/// @brief Sets a tile in the tilemap. Tiles set after the Orient_walls or randomize_floor functions will not be oriented or randomized
/// @param tilemap the tilemap to set the tile in
/// @param x the x position of the tile
/// @param y the y position of the tile
/// @param tile the tile that is replacing the tile at x, y
void tilemap_set_tile(TileMap *tilemap, int x, int y, Tile *tile);

/// @brief Draws the tilemap
/// @param tilemap the tilemap to draw
void tilemap_draw(TileMap *tilemap);

/// @brief Gets a tile from the tilemap at x, y
/// @param tilemap the tilemap to get the tile from
/// @param x the x position of the tile
/// @param y the y position of the tile
/// @return Pointer to the tile at the grid position
Tile *get_tile(TileMap *tilemap, int x, int y);

/// @brief Gets a tile from world coordinates
/// @param tilemap the tilemap to get the tile from
/// @param x world x position of the tile
/// @param y world y position of the tile
/// @return Pointer to the tile at the world position
Tile *get_world_tile(TileMap *tilemap, int x, int y);
/// @brief Frees the tilemap and all of its textures
void tilemap_free(TileMap *tilemap);

/// @brief Generates a maze using the recursive backtracking algorithm. It also scales the maze up by the factor defined in MAZE_SCALEUP_FACTOR
/// @param tilemap the tilemap to generate the maze in
/// @param width the width of the maze
/// @param height the height of the maze
/// @param seed the seed to use for the random number generator
void generate_maze(TileMap *tilemap, int width, int height, int seed);

/// @brief A recursive backtracking algorithm to generate a maze
/// @param maze maze array
/// @param visited visited array
/// @param width the width of the maze
/// @param height height of the maze
/// @param current_x current x position (chosen at random when started)
/// @param current_y current y position (chosen at random when started)
void recursive_backtrack(int maze[], int visited[], int width, int height, int current_x, int current_y);

/// @brief Randomize the floor pattern
/// @param tilemap pointer to the tilemap
/// @param seed the seed for the random number generator
void randomize_floor(TileMap *tilemap, int seed);

/// @brief Orients the walls based on the surrounding tiles
/// @param tilemap the tilemap to orient the walls in
void orient_walls(TileMap *tilemap);

#endif // TILEMAP_H