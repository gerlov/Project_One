#include "collisions.h"  
#include "tilemap.h"  


bool collides(SDL_Rect *nextPosition, TileMap *tilemap, TileType tiletype) { // can check collision with other TileTypes
    int tileSize = tilemap->tile_size;   
    int leftTile = nextPosition->x / tileSize;  
    int rightTile = (nextPosition->x + nextPosition->w - 1) / tileSize;  
    int topTile = nextPosition->y / tileSize;    
    int bottomTile = (nextPosition->y + nextPosition->h - 1) / tileSize;

    for (int y = topTile; y <= bottomTile; y++) {
        for (int x = leftTile; x <= rightTile; x++) {
            Tile *tile = &tilemap->tiles[y][x];
            if (tile != NULL && tile->type == TILE_WALL) {
                return true; 
            }
        }
    } 
    return false; 
}  
