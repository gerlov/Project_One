#ifndef MAZE_VIEW_H
#define MAZE_VIEW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "tilemap.h" 
#include"texture.h"

typedef struct {
    SDL_Texture* texture;
    SDL_Rect viewRect; 
    int visible;
    Uint32 displayTime;
} MazeView;

void init_maze_view(MazeView* mazeView, SDL_Renderer* renderer, TileMap* tilemap, int window_width, int window_height); 
void render_maze_view(MazeView* mazeView, SDL_Renderer* renderer); 
void free_maze_view(MazeView* mazeView);

#endif // MAZE_VIEW_H
