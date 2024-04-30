#ifndef MAZE_GEN_H
#define MAZE_GEN_H
#include <stdbool.h>
#define T_NEW_PATH_PERCENT 10

void maze_init(int maze[], int visited[],int width, int height);

void recursive_backtrack(int maze[], int visited[], int width, int height, int current_x, int current_y);

void tilemap_new_paths(int maze[], int width, int height);

/// @brief used to validate posistion in the maze generation
static bool is_valid(int maze[], int width, int height, int x, int y);

bool tilemap_is_valid_new_path(int maze[],int width, int height, int x, int y);

int get_index(int x, int y, int width);
#endif