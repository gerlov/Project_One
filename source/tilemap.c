#include "tilemap.h"
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <time.h>

#define MAZE_SCALEUP_FACTOR 3
#define TILEMAP_MAP1_W 32
#define TILEMAP_MAP1_H 32
#define T_SIZE 32

static Tile tiles_types[] = {
    //  TYPE,   {SRC_X, SRC_Y, SRC_W, SRC_H}
    {TILE_EMPTY, {0 * T_SIZE, 4 * T_SIZE, T_SIZE, T_SIZE}}, // TILE_EMPTY
    {TILE_WALL,  {4 * T_SIZE, 0 * T_SIZE, T_SIZE, T_SIZE}},   // TILE_WALL
    {TILE_FLOOR, {1 * T_SIZE, 1 * T_SIZE, T_SIZE, T_SIZE}}, // TILE_FLOOR
};

/// @brief the walls clip rect. Used to determine which sprite to use for a wall tile
static SDL_Rect walls[] = {
    // Top means a floor tile is above the tile etc
    {0 * T_SIZE, 3 * T_SIZE, T_SIZE, T_SIZE}, // NONE
    {1 * T_SIZE, 2 * T_SIZE, T_SIZE, T_SIZE}, // top 1
    {0 * T_SIZE, 1 * T_SIZE, T_SIZE, T_SIZE}, // right 2
    {4 * T_SIZE, 1 * T_SIZE, T_SIZE, T_SIZE}, // top right 3
    {1 * T_SIZE, 0 * T_SIZE, T_SIZE, T_SIZE}, // down 4
    {0, 0, 0, 0},                             // top down 5 NOT USED
    {4 * T_SIZE, 2 * T_SIZE, T_SIZE, T_SIZE}, // down right 6
    {0, 0, 0, 0},                             // top down right 7 NOT USED
    {2 * T_SIZE, 1 * T_SIZE, T_SIZE, T_SIZE}, // left 8
    {3 * T_SIZE, 1 * T_SIZE, T_SIZE, T_SIZE}, // left top 9
    {0, 0, 0, 0},                             // left right 10 NOT USED
    {0, 0, 0, 0},                             // left right top 11 NOT USED
    {3 * T_SIZE, 2 * T_SIZE, T_SIZE, T_SIZE}, // left down 12
    {0, 0, 0, 0},                             // left right down top 13 NOT USED
    {0, 0, 0, 0},                             // left right down 14 NOT USED
    {0, 0, 0, 0},                             // left right down top 15 NOT USED
    {0 * T_SIZE, 2 * T_SIZE, T_SIZE, T_SIZE}, // corner top right 17
    {0 * T_SIZE, 0 * T_SIZE, T_SIZE, T_SIZE}, // corner down right 16
    {2 * T_SIZE, 0 * T_SIZE, T_SIZE, T_SIZE}, // corner down left 18
    {2 * T_SIZE, 2 * T_SIZE, T_SIZE, T_SIZE}  // corner top left 19
};



void apply_maze(TileMap *tilemap, int maze[], int width, int height)
{
    tilemap->width = (width+2) * MAZE_SCALEUP_FACTOR;
    tilemap->height = (height+2) * MAZE_SCALEUP_FACTOR;

    // Set all tiles to wall
    for (int y = 0; y < TILEMAP_MAP1_H; y++)
    {
        for (int x = 0; x < TILEMAP_MAP1_W; x++)
        {
            Tile tile = tiles_types[TILE_WALL];
            for (int i = 0; i < MAZE_SCALEUP_FACTOR; i++)
            {
                for (int j = 0; j < MAZE_SCALEUP_FACTOR; j++)
                {
                    tilemap_set_tile(tilemap, x * MAZE_SCALEUP_FACTOR + i, y * MAZE_SCALEUP_FACTOR + j, &tile);
                }
            }
        }
    }

    // set floor tiles according to the maze
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (maze[get_index(x, y, width)] == 0)
            {
                Tile tile = tiles_types[TILE_FLOOR];

                // scale up the floor tiles
                for (int i = 0; i < MAZE_SCALEUP_FACTOR; i++)
                {
                    for (int j = 0; j < MAZE_SCALEUP_FACTOR; j++)
                    {
                        tilemap_set_tile(tilemap, (x + 1) * MAZE_SCALEUP_FACTOR + i, y * MAZE_SCALEUP_FACTOR + j, &tile);
                    }
                }
            }
        }
    }
}

int get_index(int x, int y, int width)
{
    return y * width + x;
}


void generate_maze(TileMap *tilemap, int width, int height, int seed)
{
    int lesswidth = width - 2;
    int lessheight = height - 2;
    int *maze = malloc(sizeof(int) * (lesswidth) * (lessheight));
    int *visited = malloc(sizeof(int) * (lesswidth) * (lessheight));
    for (int i = 0; i < (lesswidth) * (lessheight); i++)
    {
        // Initialize all cells as walls
        maze[i] = 1;
        // Initialize all cells as not visited

        // Initialize all cells as not visited
        visited[i] = 0;
    }

    srand(seed);
    int x = rand() % (lesswidth);
    int y = rand() % (lessheight);
    recursive_backtrack(maze, visited, lesswidth, lessheight, x, y);

    
    apply_maze(tilemap, maze, lesswidth, lessheight);
}
/// @brief used to validate posistion in the maze generation
static bool is_valid(int maze[], int width, int height, int x, int y)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return false;
    }
    return true;
}
void recursive_backtrack(int maze[], int visited[], int width, int height, int current_x, int current_y)
{
    visited[current_y * width + current_x] = 1;

    int directions[4][2] = {
        {0, -1},
        {0, 1},
        {-1, 0},
        {1, 0},
    };
    // Shuffle the directions array
    for (int i = 3; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp[2] = {directions[i][0], directions[i][1]};
        directions[i][0] = directions[j][0];
        directions[i][1] = directions[j][1];
        directions[j][0] = temp[0];
        directions[j][1] = temp[1];
    }
    // Loop through the directions array
    for (int i = 0; i < 4; i++)
    {
        // Check if the next cell is within the bounds of the maze
        int new_x = current_x + directions[i][0] * 2;
        int new_y = current_y + directions[i][1] * 2;
        if (is_valid(maze, width, height, new_x, new_y) && !visited[new_y * width + new_x])
        {

            // neighbor is not visited
            maze[new_y * width + new_x] = 0;
            // Remove the wall between the current cell and the new cell
            maze[(current_y + directions[i][1]) * width + (current_x + directions[i][0])] = 0;
            // Recursively call the function with the new cell
            recursive_backtrack(maze, visited, width, height, new_x, new_y);
        }
    }
}
void tilemap_load(TileMap *tilemap, int map_id)
{
    if (map_id == 1)
    {
        generate_maze(tilemap, TILEMAP_MAP1_W, TILEMAP_MAP1_H, 0);
    }
    else if (map_id == 2)
    {
        generate_maze(tilemap, TILEMAP_MAP1_W, TILEMAP_MAP1_H, time(NULL));
    }
    else
    {
        printf("Error: Map not found\n");
    }
}

void tilemap_load_file(TileMap *tilemap, const char *path)
{
    printf("Error: Not implemented\n");
    return;
}
void tilemap_save_file(TileMap *tilemap, const char *path)
{
    printf("Error: Not implemented\n");
    return;
}

void tilemap_init(TileMap *tilemap, SDL_Renderer *renderer, int width, int height, int tile_size)
{
    tilemap->width = width;
    tilemap->height = height;
    tilemap->tiles = malloc(sizeof(Tile) * width * height);
    tilemap->tile_size = tile_size;
    tilemap->pRenderer = renderer;
    tilemap->x = 0;
    tilemap->y = 0;
    SDL_Surface *surface = IMG_Load("resources/tiles/Yellow_Dungeon_Tileset.png"); // Temporary tilemap image

    if (!surface)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return;
    }
    tilemap->pTexture = SDL_CreateTextureFromSurface(renderer, surface);
    surface = IMG_Load("resources/tiles/Yellow_Brick_Floor.png");
    if (!surface)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return;
    }
    tilemap->pFloorTexture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);
}

void tilemap_set_tile(TileMap *tilemap, int x, int y, Tile *tile)
{
    tilemap->tiles[y * tilemap->width + x] = *tile;
}

void tilemap_draw(TileMap *tilemap)
{
    SDL_Rect possition;
    possition.x = 0;
    possition.y = 0;
    possition.w = tilemap->tile_size;
    possition.h = tilemap->tile_size;
    for (int y = 0; y < tilemap->height; y++)
    {
        for (int x = 0; x < tilemap->width; x++)
        {
            Tile *tile = get_tile(tilemap, x, y);
            if (tile != NULL)
            {
                possition.x = x * tilemap->tile_size - tilemap->x;
                possition.y = y * tilemap->tile_size - tilemap->y;
                if (tile->type == TILE_FLOOR)
                {
                    SDL_RenderCopyEx(tilemap->pRenderer, tilemap->pFloorTexture, &tile->src_rect, &possition, 0, NULL, SDL_FLIP_NONE);
                }
                else
                {
                    SDL_RenderCopyEx(tilemap->pRenderer, tilemap->pTexture, &tile->src_rect, &possition, 0, NULL, SDL_FLIP_NONE);
                }
            }
        }
    }
}

Tile *get_tile(TileMap *tilemap, int x, int y)
{
    if (x < 0 || x >= tilemap->width || y < 0 || y >= tilemap->height)
    {
        return NULL;
    }
    return &tilemap->tiles[y * tilemap->width + x];
}

Tile *get_world_tile(TileMap *tilemap, int x, int y)
{
    x /= tilemap->tile_size;
    y /= tilemap->tile_size;
    if (x < 0 || x >= tilemap->width || y < 0 || y >= tilemap->height)
    {
        return NULL;
    }
    return &tilemap->tiles[y * tilemap->width + x];
}

void tilemap_free(TileMap *tilemap)
{
    free(tilemap->tiles);
    tilemap->tiles = NULL;
    SDL_DestroyTexture(tilemap->pTexture);
    tilemap->pTexture = NULL;
    tilemap->pRenderer = NULL;
}

void randomize_floor(TileMap *tilemap, int seed)
{
    srand(seed);
    for (int y = 0; y < tilemap->height; y++)
    {
        for (int x = 0; x < tilemap->width; x++)
        {
            Tile *tile = get_tile(tilemap, x, y);
            if (tile->type == TILE_FLOOR)
            {
                int y_rect = rand() % 6;
                int x_rect = rand() % 4;
                tile->src_rect = (SDL_Rect){x_rect * T_SIZE, y_rect * T_SIZE, T_SIZE, T_SIZE};
            }
        }
    }
}

TileType *get_neighbur_tiles_type(TileMap *tilemap, int x, int y)
{
    TileType *neighbours = malloc(sizeof(int) * 8);
    Tile *tile = NULL;
    tile = get_tile(tilemap, x, y - 1); // Top
    neighbours[0] = (tile != NULL) ? tile->type : TILE_EMPTY;
    tile = get_tile(tilemap, x + 1, y); // Right
    neighbours[1] = (tile != NULL) ? tile->type : TILE_EMPTY;
    tile = get_tile(tilemap, x, y + 1); // Bottom
    neighbours[2] = (tile != NULL) ? tile->type : TILE_EMPTY;
    tile = get_tile(tilemap, x - 1, y); // Left
    neighbours[3] = (tile != NULL) ? tile->type : TILE_EMPTY;
    tile = get_tile(tilemap, x + 1, y - 1); // Top Right
    neighbours[4] = (tile != NULL) ? tile->type : TILE_EMPTY;
    tile = get_tile(tilemap, x + 1, y + 1); // Bottom Right
    neighbours[5] = (tile != NULL) ? tile->type : TILE_EMPTY;
    tile = get_tile(tilemap, x - 1, y + 1); // Bottom Left
    neighbours[6] = (tile != NULL) ? tile->type : TILE_EMPTY;
    tile = get_tile(tilemap, x - 1, y - 1); // Top Left
    neighbours[7] = (tile != NULL) ? tile->type : TILE_EMPTY;
    return neighbours;
}
/// @brief Orient the walls based on the surrounding tiles
/// @param tilemap 
void orient_walls(TileMap *tilemap)
{
    for (int y = 0; y < tilemap->height; y++)
    {
        for (int x = 0; x < tilemap->width; x++)
        {
            Tile *tile = get_tile(tilemap, x, y);
            if (tile->type != TILE_WALL)
                continue;
            TileType *neighbours = get_neighbur_tiles_type(tilemap, x, y);

            // this makes a 4 bit number that represents the walls around the tile
            int dir = NONE;
            if (neighbours[0] == TILE_FLOOR)
            {
                dir = dir | UP;
            }
            if (neighbours[1] == TILE_FLOOR)
            {
                dir = dir | RIGHT;
            }
            if (neighbours[2] == TILE_FLOOR)
            {
                dir = dir | DOWN;
            }
            if (neighbours[3] == TILE_FLOOR)
            {
                dir = dir | LEFT;
            }
            if (dir == NONE)
            {
                // special case for corners
                if (neighbours[4] == TILE_FLOOR)
                {
                    dir = dir | TOP_RIGHT;
                }
                else if (neighbours[5] == TILE_FLOOR)
                {
                    dir = dir | BOTTOM_RIGHT;
                }
                else if (neighbours[6] == TILE_FLOOR)
                {
                    dir = dir | BOTTOM_LEFT;
                }
                else if (neighbours[7] == TILE_FLOOR)
                {
                    dir = dir | TOP_LEFT;
                }
            }
            // walls is a array of SDL_Rect that is ordered in a way that it behaves a kind of hash map
            tile->src_rect = walls[dir];
        }
    }
}


void tilemap_update_position(TileMap *tilemap, SDL_Rect *character_rect, int window_width, int window_height) {

    tilemap->x = character_rect->x - window_width / 2;
    tilemap->y = character_rect->y - window_height / 2;
}