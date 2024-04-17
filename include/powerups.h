#ifndef POWERUPS_H
#define POWERUPS_H

#include "tilemap.h"

// Functions for managing powerups in a tilemap
void place_powerup(TileMap *tilemap, TileType powerup_type);
void remove_powerup(TileMap *tilemap, int x, int y);

#endif // POWERUPS_H
