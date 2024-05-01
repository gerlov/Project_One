#ifndef MAZE_GEN_H
#define MAZE_GEN_H
#include <stdbool.h>

typedef enum
{
    MAZE_FLOOR = 0,
    MAZE_WALL = 1,
    MAZE_HUMAN_SPAWNABLE= 2,
    MAZE_HUNTER_SPAWNABLE = 3,
    MAZE_PIT = 4,
    MAZE_CASM_FLOOR = 5
} Maze_tile_type;

#define T_NEW_PATH_PERCENT 10
#define T_NEW_PIT_PERCENT 5

void maze_init(int maze[], int visited[],int width, int height);

void recursive_backtrack(int maze[], int visited[], int width, int height, int current_x, int current_y);

void tilemap_new_paths(int maze[], int width, int height);

/// @brief used to validate posistion in the maze generation
static bool is_valid(int maze[], int width, int height, int x, int y);

bool tilemap_is_valid_new_path_x(int maze[], int width, int height, int x, int y);
bool tilemap_is_valid_new_path_y(int maze[], int width, int height, int x, int y);
bool tilemap_is_valid_new_path_corner(int maze[], int width, int height, int x, int y);
bool tilemap_is_valid_new_path(int maze[],int width, int height, int x, int y);

int get_index(int x, int y, int width);
#endif