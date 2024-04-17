#include "collisions.h" 
#include "window.h"
#include "tilemap.h"  
#include "character.h"  


bool collides(SDL_Rect *nextPosition, TileMap *tilemap, int windowWidth, int windowHeight) {

    if (nextPosition->x < 0 || nextPosition->y < 0 || 
        nextPosition->x + nextPosition->w > windowWidth || 
        nextPosition->y + nextPosition->h > windowHeight)   {  
            return true;   
        }     
    if (intersects_with_tile(nextPosition, tilemap, TILE_WALL)) return true;  
    return false;    
}    

bool intersects_with_tile(SDL_Rect *nextPosition, TileMap *tilemap, TileType tiletype) {
    int tileSize = tilemap->tile_size;   
    int leftTile = nextPosition->x / tileSize;  
    int rightTile = (nextPosition->x + nextPosition->w - 1) / tileSize;  
    int topTile = nextPosition->y / tileSize;    
    int bottomTile = (nextPosition->y + nextPosition->h - 1) / tileSize;

    for (int y = topTile; y <= bottomTile; y++) {
        for (int x = leftTile; x <= rightTile; x++) {
            Tile *tile = get_tile(tilemap, x, y);
            if (tile != NULL && tile->type == tiletype) {
                return true; 
            }
        }
    } 
    return false; // No intersection found
}
 

bool characters_collide(SDL_Rect *rect1, SDL_Rect *rect2) {
    return SDL_HasIntersection(rect1, rect2);
}
 
