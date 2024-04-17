#include <stdlib.h>
#include <time.h>
#include "tilemap.h"

void place_powerup(TileMap *tilemap, TileType powerup_type) {
    srand(time(NULL));
    int available_positions[tilemap->width * tilemap->height];
    int num_available_positions = 0;
    for (int y = 0; y < tilemap->height; y++) {
        for (int x = 0; x < tilemap->width; x++) {
            Tile *tile = get_tile(tilemap, x, y);
            if (tile != NULL && tile->type == TILE_FLOOR) {
                available_positions[num_available_positions++] = y * tilemap->width + x;
            }
        }
    }
    if (num_available_positions > 0) {
        int random_index = rand() % num_available_positions;
        int random_position = available_positions[random_index];
        int x = random_position % tilemap->width;
        int y = random_position / tilemap->width;
        Tile powerup_tile;
        powerup_tile.type = powerup_type;
        tilemap_set_tile(tilemap, x, y, &powerup_tile);
    } else {
        printf("No available positions to place powerup.\n");
    }
}

void remove_powerup(TileMap *tilemap, int x, int y) {
    if (x < 0 || x >= tilemap->width || y < 0 || y >= tilemap->height) {
        printf("Invalid position to remove powerup.\n");
        return;
    }
    Tile floor_tile;
    floor_tile.type = TILE_FLOOR;
    tilemap_set_tile(tilemap, x, y, &floor_tile);
}
