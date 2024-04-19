#ifndef POWERUP_H
#define POWERUP_H

#include "tilemap.h"
#include "character.h"
#include "texture.h"
#include "music.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef enum {
    POWERUP_HEALTH,
    POWERUP_SPEED,
    POWERUP_INVISIBLE,
    // add other power-up types here
} PowerUpType;

typedef struct PowerUp {
    SDL_Rect rect;          
    PowerUpType type;       
    int visible;            
    SDL_Texture* texture;   
} PowerUp;

#define MAX_POWERUPS 100

extern PowerUp powerUps[MAX_POWERUPS];  
extern int powerUpCount;                


void init_powerUps(SDL_Renderer* renderer, TileMap* tilemap, int tile_size); 
void draw_powerUps(SDL_Renderer* renderer);  
void apply_powerUp(Character *character, PowerUpType type);  
void cleanup_powerUps();  
SDL_Texture* load_png_texture(SDL_Renderer* renderer, const char* filePath);

#endif // POWERUP_H
