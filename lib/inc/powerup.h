#ifndef POWERUP_H
#define POWERUP_H

#include "tilemap.h"
#include "our_rand.h"
#include "mazeview.h"
#include "character.h"
#include "texture.h"
#include "music.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define MAX_POWERUPS 100 // increased temporarily, for testning, change!

typedef enum {
    POWERUP_SPEED,
    POWERUP_INVISIBLE,
    POWERUP_MAP, 
    POWERUP_SKULL
} PowerUpType;

typedef struct PowerUp {
    int powerupid;
    SDL_Rect rect;          
    PowerUpType type;       
    int active;  // AKA POWERUP STATE: COLLECTED = NOT VISIBLE ANYMORE          
    SDL_Texture* texture;   
} PowerUp;   

extern PowerUp powerUps[MAX_POWERUPS];
extern int powerUpCount;

///@brief Loads resources for power-ups: textures and sounds 
///@param renderer Pointer to the SDL_Renderer used for loading textures.
void load_powerup_resources(SDL_Renderer* renderer); 

///@brief Cleans up and frees all loaded power-up resources (ie textures, sounds)
void cleanup_powerup_resources(); 

///@brief Initializes power-ups in the game world and places them on the tile map.
///@param renderer Pointer to the SDL_Renderer used for rendering.
///@param tilemap Pointer to the TileMap where power-ups will be placed.
///@param tile_size The size of the tiles in the map to determine placement coordinates.
void init_powerUps(SDL_Renderer* renderer, TileMap* tilemap, int tile_size); 

///@brief Draws power-ups on the renderer based on their visibility and positions relative to the camera.
///@param renderer Pointer to the SDL_Renderer where the power-ups will be drawn.
///@param tilemap Pointer to the TileMap used to adjust power-up positions based on the camera's current view.
void draw_powerUps(SDL_Renderer* renderer, TileMap* tilemap); 

///@brief Applies the effect of a power-up to a character.
///@param character Pointer to the Character to receive the power-up effects.
///@param type The type of power-up to apply.
///@param characters Array of all characters in the game 
///@param num_characters Total number of characters in the characters array.
/// 
void apply_powerUp(Character *character, PowerUpType type, Character **characters, 
                   int num_characters, MazeView *mazeView); 





#endif // POWERUP_H
