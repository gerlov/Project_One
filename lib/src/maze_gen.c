#include "maze_gen.h"
#include "debug.h"
#include "our_rand.h"

void maze_init(int maze[], int visited[], int width, int height)
{
    for (int i = 0; i < width * height; i++)
    {
        maze[i] = 1;
        visited[i] = 0;
    }
}

/// Recursive backtracking algorithm for maze generation
/// Source: 
/// https://en.wikipedia.org/wiki/Maze_generation_algorithm#Randomized_depth-first_search
/// generates a maze with the given width and height.
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
        int j = our_rand() % (i + 1);
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
            maze[new_y * width + new_x] = MAZE_FLOOR;
            // Remove the wall between the current cell and the new cell
            maze[(current_y + directions[i][1]) * width + (current_x + directions[i][0])] = MAZE_FLOOR;
            // Recursively call the function with the new cell
            recursive_backtrack(maze, visited, width, height, new_x, new_y);
        }
    }
}
void tilemap_new_paths(int maze[], int width, int height)
{
    DEBUG_PRINT2("Creating new paths\n");
    int inset = 1;
    for (int i = 0; i < (width) * (height); i++)
    {
        // find two floors that are separated by a wall
        int x = i % width;
        int y = i / width;
        if (y > inset && y < height - inset && x > inset && x < width - inset)
        {
            if (maze[i] == MAZE_WALL)
            {

                if (tilemap_is_valid_new_path_y(maze, width, height, x, y) && !tilemap_is_valid_new_path_corner(maze, width, height, x, y) &&
                    our_rand() % 100 <= T_NEW_PIT_PERCENT)
                {
                        DEBUG_PRINT3("New Pit at tile %d,%d\n", x, y);
                        maze[get_index(x, y, width)] = MAZE_PIT;
                }
                else if (tilemap_is_valid_new_path(maze, width, height, x, y) &&
                         our_rand() % 100 <= T_NEW_PATH_PERCENT)
                {
                        DEBUG_PRINT3("New Path at tile %d,%d\n", x, y);
                        maze[get_index(x, y, width)] = MAZE_FLOOR;
                }
            }
        }
    }
}

static bool is_valid(int maze[], int width, int height, int x, int y)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return false;
    }
    return true;
}
bool tilemap_is_valid_new_path_x(int maze[], int width, int height, int x, int y)
{
    return maze[get_index(x + 1, y, width)] == MAZE_FLOOR && maze[get_index(x - 1, y, width)] == MAZE_FLOOR && maze[get_index(x, y + 1, width)] == MAZE_WALL && maze[get_index(x, y - 1, width)] == MAZE_WALL;
}
bool tilemap_is_valid_new_path_y(int maze[], int width, int height, int x, int y)
{
    return maze[get_index(x, y + 1, width)] == MAZE_FLOOR && maze[get_index(x, y - 1, width)] == MAZE_FLOOR && maze[get_index(x + 1, y, width)] == MAZE_WALL && maze[get_index(x - 1, y, width)] == MAZE_WALL;
}
bool tilemap_is_valid_new_path_corner(int maze[], int width, int height, int x, int y)
{
    return maze[get_index(x + 1, y + 1, width)] == MAZE_FLOOR || maze[get_index(x - 1, y + 1, width)] == MAZE_FLOOR || maze[get_index(x + 1, y - 1, width)] == MAZE_FLOOR || maze[get_index(x - 1, y - 1, width)] == MAZE_FLOOR;
}
bool tilemap_is_valid_new_path(int maze[], int width, int height, int x, int y)
{

    // check if the wall has a floor on the top and bottom
    bool top_bottom = tilemap_is_valid_new_path_y(maze, width, height, x, y);
    // check if the wall has a floor on the left and right
    bool left_right = tilemap_is_valid_new_path_x(maze, width, height, x, y);
    // check if the corners are floors
    // bool corner = tilemap_is_valid_new_path_corner(maze, width, height, x, y);

    return (top_bottom || left_right);
}

int get_index(int x, int y, int width)
{
    return y * width + x;
}
