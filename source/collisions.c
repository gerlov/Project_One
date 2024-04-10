// collisions.c 
// sources, concepts, principles, etc:   
// 1. Tile based map collision detection  
// https://www.parallelrealities.co.uk/tutorials/sdl1/intermediate/tutorial14.php 
// 2. Entity to tile collision detection 
// https://gamedev.stackexchange.com/questions/101689/entity-to-tile-collision-detection-c-sdl   


#include "collisions.h" 
#include "window.h"
#include "tilemap.h"  


bool collides(SDL_Rect *nextPosition, TileMap *tilemap, int windowWidth, int windowHeight) {

    // 1. check if outside window borders 
    if (nextPosition->x < 0 || nextPosition->y < 0 || 
        nextPosition->x + nextPosition->w > windowWidth || 
        nextPosition->y + nextPosition->h > windowHeight)   {  
            return true;   // collision with border 
        } 

    // 2. check for collision with solid tiles in the tilemap  
    // leftTile, rightTile -- these find which map tiles are at the sides of the object,
    // so we check only these tiles for collision; 
    // topTile and bottomTile -- the same but for top and bottom of the object.
    int tileSize = tilemap->tile_size;   
    int leftTile = nextPosition->x / tileSize;  
    int rightTile = (nextPosition->x + nextPosition->w - 1) / tileSize;  
    int topTile = nextPosition->y / tileSize;    
    int bottomTile = (nextPosition->y + nextPosition->h - 1) / tileSize;

    // checks each tile the object might touch. if the tile is solid, 
    // the object can not move there. 
    for (int y = topTile; y <= bottomTile; y++) {
        for (int x = leftTile; x <= rightTile; x++) {
            Tile *tile = get_tile(tilemap, x, y);
            if (tile != NULL && tile->is_solid) {
                return true; // collision with a solid tile
            }
        }
    } 

    return false;     // no collisions detected  
}


