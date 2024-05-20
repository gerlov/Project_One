#include "tilemap.h"
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include "our_rand.h"
#include "maze_gen.h"
#include "debug.h"

static Tile tiles_types[] = {
    //  TYPE,   {SRC_X, SRC_Y, SRC_W, SRC_H}
    {TILE_EMPTY, {0 * T_SIZE, 4 * T_SIZE, T_SIZE, T_SIZE}}, // TILE_EMPTY
    {TILE_WALL, {4 * T_SIZE, 0 * T_SIZE, T_SIZE, T_SIZE}},  // TILE_WALL
    {TILE_FLOOR, {1 * T_SIZE, 1 * T_SIZE, T_SIZE, T_SIZE}}, // TILE_FLOOR
    {TILE_PIT, {4 * T_SIZE, 0 * T_SIZE, T_SIZE, T_SIZE}},   // TILE_PIT
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

static SDL_Rect pits[] = {
    {5 * T_SIZE, 1 * T_SIZE, T_SIZE, T_SIZE}, // PIT EMPTY
    {4 * T_SIZE, 0 * T_SIZE, T_SIZE, T_SIZE}, // PIT
};

void set_spawn_divisions(TileMap *tilemap, int maze[], int width, int height)
{
    int previous_division = -1;
    // amount of divisions in the maze. Actuall amount of spawn devisions is divisions^2
    int divisions = 12;

    for (int k = 0; k < 3; k++)
    {
        int selected = k;
        if (k == 1)
        {
            // selected = our_rand() % (divisions * divisions);
        } else if (k == 2) {
            bool is_valid_division = false;
            int division_width = width / divisions;
            int division_height = height / divisions;

            while (!is_valid_division) {
                selected = our_rand() % (divisions * divisions);

                int start_x = (selected % divisions) * division_width;
                int start_y = (selected / divisions) * division_height;

                // just in case
                if (start_x + division_width <= width && start_y + division_height <= height)
                {
                    is_valid_division = true;
                }
            }
        }

        previous_division = selected;

        int division_width = width / divisions;
        int division_height = height / divisions;

        int start_x = (selected % divisions) * division_width;
        int start_y = (selected / divisions) * division_height;

        for (int i = 0; i < division_height; i++)
        {
            for (int j = 0; j < division_width; j++)
            {
                int x = start_x + j;
                int y = start_y + i;
                if (maze[get_index(x, y, width)] == 0)
                {
                    if (k == 0)
                    {
                        maze[get_index(x, y, width)] = MAZE_HUMAN_SPAWNABLE;
                        tilemap->human_spawn.num_points++;
                    }
                    else if (k == 1)
                    {
                        maze[get_index(x, y, width)] = MAZE_HUNTER_SPAWNABLE;
                        tilemap->hunter_spawn.num_points++;
                    }
                    else if (k== 2) {
                        maze[get_index(x, y, width)] = MAZE_PORTAL_SPAWNABLE;
                        tilemap->portal_spawn.num_points++;
                    }
                }
            }
        }
    }

    tilemap->human_spawn.points = malloc(sizeof(SDL_Point) * tilemap->human_spawn.num_points * MAZE_SCALEUP_FACTOR * MAZE_SCALEUP_FACTOR);
    tilemap->hunter_spawn.points = malloc(sizeof(SDL_Point) * tilemap->hunter_spawn.num_points * MAZE_SCALEUP_FACTOR * MAZE_SCALEUP_FACTOR);
    tilemap->portal_spawn.points = malloc(sizeof(SDL_Point) * tilemap->portal_spawn.num_points * MAZE_SCALEUP_FACTOR * MAZE_SCALEUP_FACTOR);
    tilemap->human_spawn.num_points *= MAZE_SCALEUP_FACTOR * MAZE_SCALEUP_FACTOR;
    tilemap->hunter_spawn.num_points *= MAZE_SCALEUP_FACTOR * MAZE_SCALEUP_FACTOR;
    tilemap->portal_spawn.num_points *= MAZE_SCALEUP_FACTOR * MAZE_SCALEUP_FACTOR;
}

SDL_Point get_spawn_point(TileMap *tilemap, int ishunter)
{

    SDL_Point spawn = (SDL_Point){-1, -1};
    if (ishunter)
    {
        if (tilemap->hunter_spawn.num_points > 0)
        {
            int index = 0;
            do
            {
                index = our_rand() % tilemap->hunter_spawn.num_points;
                spawn = tilemap->hunter_spawn.points[index];
            } while (spawn.x == -1);
            tilemap->hunter_spawn.points[index] = (SDL_Point){-1, -1};
            return spawn;
        }
    }
    else
    {
        if (tilemap->human_spawn.num_points > 0)
        {
            int index = 0;
            do
            {
                index = our_rand() % tilemap->human_spawn.num_points;
                spawn = tilemap->human_spawn.points[index];
            } while (spawn.x == -1);
            tilemap->human_spawn.points[index] = (SDL_Point){-1, -1};
            return spawn;
        }
    }
    printf("Error: No spawn point found\n");
    return (SDL_Point){-1, -1};
}

void apply_maze(TileMap *tilemap, int maze[], int width, int height)
{
    tilemap->width = (width + 2) * MAZE_SCALEUP_FACTOR;
    tilemap->height = (height + 2) * MAZE_SCALEUP_FACTOR;

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
    int human_spawn_count = 0;
    int hunter_spawn_count = 0;
    int portal_spawn_count = 0;
    // set floor tiles according to the maze
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int type = maze[get_index(x, y, width)];
            if (type == MAZE_PIT)
            {
                Tile tile = tiles_types[TILE_PIT];
                for (int i = 0; i < MAZE_SCALEUP_FACTOR; i++)
                {
                    for (int j = 0; j < MAZE_SCALEUP_FACTOR; j++)
                    {
                        tilemap_set_tile(tilemap, (x + 1) * MAZE_SCALEUP_FACTOR + i, (y + 1) * MAZE_SCALEUP_FACTOR + j, &tile);
                    }
                }
            }
            if (type != MAZE_WALL)
            {

                Tile tile = tiles_types[TILE_FLOOR];
                if (type == MAZE_HUMAN_SPAWNABLE)
                {
                    tile.human_spawnable = 1;
                }
                else if (type == MAZE_HUNTER_SPAWNABLE)
                {
                    tile.hunter_spawnable = 1;
                } 
                else if(type == MAZE_PORTAL_SPAWNABLE) {
                    tile.portal_spawnable = 1;
                }
                // scale up the floor tiles
                for (int i = 0; i < MAZE_SCALEUP_FACTOR; i++)
                {
                    for (int j = 0; j < MAZE_SCALEUP_FACTOR; j++)
                    {
                        int new_x = (x + 2) * MAZE_SCALEUP_FACTOR + i;
                        int new_y = (y + 1) * MAZE_SCALEUP_FACTOR + j;
                        tilemap_set_tile(tilemap, new_x, new_y, &tile);
                        if (tile.human_spawnable)
                        {
                            tilemap->human_spawn.points[human_spawn_count++] = (SDL_Point){new_x * tilemap->tile_size, new_y * tilemap->tile_size};
                        }
                        if (tile.hunter_spawnable)
                        {
                            tilemap->hunter_spawn.points[hunter_spawn_count++] = (SDL_Point){new_x * tilemap->tile_size, new_y * tilemap->tile_size};
                        }
                        if (tile.portal_spawnable)
                        {
                            tilemap->portal_spawn.points[portal_spawn_count++] = (SDL_Point){new_x * tilemap->tile_size, new_y * tilemap->tile_size};
                        }
                    }
                }
            }
        }
    }
}

void generate_maze(TileMap *tilemap, int width, int height, int seed)
{
    our_srand(seed);
    int lesswidth = width - 2;
    int lessheight = height - 2;
    int *maze = malloc(sizeof(int) * lesswidth * lessheight);
    int *visited = malloc(sizeof(int) * lesswidth * lessheight);
    for (int i = 0; i < lesswidth * lessheight; i++)
    {
        maze[i] = MAZE_WALL;
        visited[i] = 0;
    }
    int x = our_rand() % (lesswidth);
    int y = our_rand() % (lessheight);

    recursive_backtrack(maze, visited, lesswidth, lessheight, x, y);
    tilemap_new_paths(maze, lesswidth, lessheight);
    set_spawn_divisions(tilemap, maze, lesswidth, lessheight);
    apply_maze(tilemap, maze, lesswidth, lessheight);
    free(maze);
    free(visited);
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

void tilemap_load(TileMap *tilemap, int map_id, int seed)
{
    if (map_id == 1)
    {
        generate_maze(tilemap, TILEMAP_MAP1_W, TILEMAP_MAP1_H, 0);
    }
    else if (map_id == 2)
    {
        generate_maze(tilemap, TILEMAP_MAP1_W, TILEMAP_MAP1_H, seed);
    }
    else
    {
        printf("Error: Map not found\n");
    }
    orient_walls(tilemap);
    randomize_floor(tilemap);
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

void tilemap_init(TileMap *tilemap, SDL_Renderer *renderer)
{
    tilemap->width = TILEMAP_MAP1_H;
    tilemap->height = TILEMAP_MAP1_W;
    tilemap->tile_size = T_DISPLAY_SIZE;
    tilemap->pRenderer = renderer;

    tilemap->human_spawn = (SpawnArea){NULL, 0};
    tilemap->hunter_spawn = (SpawnArea){NULL, 0};
    tilemap->camera = (SDL_FPoint){0, 0};

    tilemap->pWallTexture = IMG_LoadTexture(renderer, "../lib/assets/tiles/Yellow_Dungeon_Tileset.png");
    tilemap->pFloorTexture = IMG_LoadTexture(renderer, "../lib/assets/tiles/Yellow_Brick_Floor.png");
    tilemap->pPitTexture = IMG_LoadTexture(renderer, "../lib/assets/tiles/Yellow_Brick_Pits.png");
}

void tilemap_set_tile(TileMap *tilemap, int x, int y, Tile *tile)
{
    tilemap->tiles[y][x] = *tile;
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
            if (tile == NULL)
                continue;
            if (tile->type == TILE_EMPTY)
                continue;

            possition.x = x * tilemap->tile_size - tilemap->camera.x;
            possition.y = y * tilemap->tile_size - tilemap->camera.y;
            SDL_Texture *used_texture = tilemap->pWallTexture;

            if (tile->type == TILE_FLOOR)
            {
                //? This is a temporary solution to show the spawnable tiles
                used_texture = tilemap->pFloorTexture;
                if (tile->human_spawnable)
                {
                    SDL_SetTextureColorMod(used_texture, 0, 255, 0);
                }
                else if (tile->hunter_spawnable)
                {
                    SDL_SetTextureColorMod(used_texture, 255, 0, 0);
                }
            }
            else if (tile->type == TILE_PIT)
            {
                 used_texture = tilemap->pPitTexture;
            }

            SDL_RenderCopyEx(tilemap->pRenderer, used_texture, &tile->src_rect, &possition, 0, NULL, SDL_FLIP_NONE);
            SDL_SetTextureColorMod(used_texture, 255, 255, 255);
        }
    }
}

Tile *get_tile(TileMap *tilemap, int x, int y)
{
    if (x < 0 || x >= tilemap->width || y < 0 || y >= tilemap->height)
    {
        return NULL;
    }
    return &tilemap->tiles[y][x];
}

void tilemap_free(TileMap *tilemap)
{
    free(tilemap->human_spawn.points);
    free(tilemap->hunter_spawn.points);
    free(tilemap->portal_spawn.points);
    SDL_DestroyTexture(tilemap->pWallTexture);
    SDL_DestroyTexture(tilemap->pFloorTexture);
    SDL_DestroyTexture(tilemap->pPitTexture);
    tilemap->pWallTexture = NULL;
    tilemap->pFloorTexture = NULL;
    tilemap->pPitTexture = NULL;
    tilemap->pRenderer = NULL;
}

void randomize_floor(TileMap *tilemap)
{
    for (int y = 0; y < tilemap->height; y++)
    {
        for (int x = 0; x < tilemap->width; x++)
        {
            Tile *tile = get_tile(tilemap, x, y);
            if (tile->type == TILE_FLOOR)
            {
                int y_rect = our_rand() % 6;
                int x_rect = our_rand() % 4;
                tile->src_rect = (SDL_Rect){x_rect * T_SIZE, y_rect * T_SIZE, T_SIZE, T_SIZE};
            }
        }
    }
}

TileType *get_neighbour_tiles_type(TileMap *tilemap, int x, int y)
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
            TileType *neighbours = get_neighbour_tiles_type(tilemap, x, y);
            if (tile->type == TILE_PIT)
            {
                if (neighbours[0] == TILE_WALL)
                {
                    tile->src_rect = pits[1];
                }
                else
                {
                    tile->src_rect = pits[0];
                }
            }
            if (tile->type != TILE_WALL) {
                free(neighbours);
                continue;
            }

            // this makes a 4 bit number that represents the walls around the tile
            int dir = NONE;
            if (neighbours[0] == TILE_FLOOR || neighbours[0] == TILE_PIT)
            {
                dir = dir | UP;
            }
            if (neighbours[1] == TILE_FLOOR|| neighbours[1] == TILE_PIT)
            {
                dir = dir | RIGHT;
            }
            if (neighbours[2] == TILE_FLOOR|| neighbours[2] == TILE_PIT)
            {
                dir = dir | DOWN;
            }
            if (neighbours[3] == TILE_FLOOR|| neighbours[3] == TILE_PIT)
            {
                dir = dir | LEFT;
            }
            if (dir == NONE)
            {
                // special case for corners
                if (neighbours[4] == TILE_FLOOR|| neighbours[4] == TILE_PIT)
                {
                    dir = dir | TOP_RIGHT;
                }
                else if (neighbours[5] == TILE_FLOOR|| neighbours[5] == TILE_PIT)
                {
                    dir = dir | BOTTOM_RIGHT;
                }
                else if (neighbours[6] == TILE_FLOOR|| neighbours[6] == TILE_PIT)
                {
                    dir = dir | BOTTOM_LEFT;
                }
                else if (neighbours[7] == TILE_FLOOR|| neighbours[7] == TILE_PIT)
                {
                    dir = dir | TOP_LEFT;
                }
                else
                {
                    tile->type = TILE_EMPTY;
                }
            }
            // walls is a array of SDL_Rect that is ordered in a way that it behaves a kind of hash map
            tile->src_rect = walls[dir];
            free(neighbours);
        }
    }
}
