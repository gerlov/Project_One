#ifndef TILEMAP_H
#define TILEMAP_H
#include <SDL2/SDL.h>
#include <stdbool.h>

#define TILEMAP_MAP1_W 32
#define TILEMAP_MAP1_H 32
#define MAZE_SCALEUP_FACTOR 3
#define T_SIZE 32
#define T_DISPLAY_SIZE 64
typedef enum TileType {
    TILE_EMPTY,
    TILE_WALL,
    TILE_FLOOR,
    TILE_PIT
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

typedef struct SpawnArea
{
    SDL_Point *points;
    int num_points;
} SpawnArea;

/// @brief Struct that represents a tile
typedef struct Tile
{
    TileType type;     // type of the tile
    SDL_Rect src_rect; // source rectangle of the texture
    bool hunter_spawnable;   // if the tile is spawnable
    bool human_spawnable;    // if the tile is spawnable
} Tile;
/// @brief Struct that represents a tilemap
typedef struct TileMap
{
    SDL_Renderer *pRenderer; // renderer to load the textures with
    SDL_Texture *pWallTexture;   // texture of the tilemap
    SDL_Texture *pFloorTexture; // texture of the floor
    SDL_Texture *pPitTexture;
    SDL_FPoint camera;       // camera position
    int width;               // in tiles
    int height;              // in tiles
    Tile tiles[TILEMAP_MAP1_H*MAZE_SCALEUP_FACTOR][TILEMAP_MAP1_W*MAZE_SCALEUP_FACTOR];             // 1D array of tiles
    int tile_size;           // in pixels
    SpawnArea hunter_spawn;
    SpawnArea human_spawn;
} TileMap;


/// @brief Inits a tilemap
/// @param tilemap the tilemap to init
/// @param renderer the renderer to load the textures with
void tilemap_init(TileMap *tilemap, SDL_Renderer *renderer);

/// @brief Loads the selected map into the tilemap
/// @param tilemap  The tilemap to load the data into
/// @param map_id The id of the map to load
void tilemap_load(TileMap *tilemap, int map_id, int seed);

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
/// @param seed the seed to use for the random number generator
void generate_maze(TileMap *tilemap, int width, int height, int seed);


/// @brief Randomize the floor pattern
/// @param tilemap pointer to the tilemap
/// @param seed the seed for the random number generator
void randomize_floor(TileMap *tilemap);

/// @brief Orients the walls based on the surrounding tiles
/// @param tilemap the tilemap to orient the walls in
void orient_walls(TileMap *tilemap);


/// @brief Get the type of the neighbouring tiles
/// @param tilemap 
/// @param x 
/// @param y 
/// @return array of that contains the type of the neighbouring tiles in the order of up, right, down, left, top right, bottom right, bottom left, top left
TileType* get_neighbur_tiles_type(TileMap *tilemap, int x, int y);


void set_spawn_divisions(TileMap *tilemap, int *maze, int width, int height);

SDL_Point get_spawn_point(TileMap *tilemap, int ishunter);


#endif // TILEMAP_H