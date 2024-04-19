#include "powerup.h"
#include <time.h>
#include <stdlib.h> 

#define MAX_POWERUPS 100 // change here

// globals to manage power-ups 
PowerUp powerUps[MAX_POWERUPS];
int powerUpCount = 0;


SDL_Texture* load_png_texture(SDL_Renderer* renderer, const char* filePath) {
    SDL_Texture* texture = NULL;
    create_texture(&texture, renderer, filePath);
    return texture;
}

void init_powerUps(SDL_Renderer* renderer, TileMap* tilemap, int tile_size) {
    SDL_Texture* textureSpeed = load_png_texture(renderer, "resources/testpack/red.png"); // red = speed
    SDL_Texture* textureInvisible = load_png_texture(renderer, "resources/testpack/green.png"); // green = invisible

    if (!textureSpeed || !textureInvisible) {
        printf("Error loading power-up textures.\n");
        return;
    }

    srand(time(NULL));
    int placedCount = 0;
    while (placedCount < MAX_POWERUPS) {
        int x = rand() % tilemap->width;
        int y = rand() % tilemap->height;
        Tile* tile = get_tile(tilemap, x, y);

        if (tile->type == TILE_FLOOR) {
            powerUps[placedCount].rect.x = x * tile_size;
            powerUps[placedCount].rect.y = y * tile_size;
            powerUps[placedCount].rect.w = tile_size;
            powerUps[placedCount].rect.h = tile_size;
            powerUps[placedCount].visible = 1;

            // rand choose powerup tile to place 
            if (rand() % 2) {
                powerUps[placedCount].texture = textureSpeed;
                powerUps[placedCount].type = POWERUP_SPEED;
            } else {
                powerUps[placedCount].texture = textureInvisible;
                powerUps[placedCount].type = POWERUP_INVISIBLE;
            }

            placedCount++;
        }
    }
    powerUpCount = placedCount;
}


void draw_powerUps(SDL_Renderer* renderer) {
    if (!renderer) {
        printf("Renderer is NULL in draw_powerUps()\n");
        return;
    }

    int frameWidth = 16;   
    int frameHeight = 16;  
    SDL_Rect srcRect;      
    SDL_Rect dstRect;      

    for (int i = 0; i < powerUpCount; i++) {
        if (powerUps[i].visible) { 
            srcRect.x = (SDL_GetTicks() / 100 % 5) * frameWidth;
            srcRect.y = 0;
            srcRect.w = frameWidth;
            srcRect.h = frameHeight;

            dstRect.x = powerUps[i].rect.x;
            dstRect.y = powerUps[i].rect.y;
            dstRect.w = 32; //  change powerup size if needed 
            dstRect.h = 32;  

            SDL_RenderCopy(renderer, powerUps[i].texture, &srcRect, &dstRect);
        }
    }
}


void cleanup_powerUps() {
    for (int i = 0; i < powerUpCount; i++) {
        if (powerUps[i].texture) {
            SDL_DestroyTexture(powerUps[i].texture);
            powerUps[i].texture = NULL;
        }
    }
    powerUpCount = 0;
}

void apply_powerUp(Character *character, PowerUpType type) {
    switch (type) {
        case POWERUP_HEALTH:
            // dont really know what to do with health yet cos we dont apply any damage yet?
            // we only kill x_x 
            // character->health += 50;  
            // TODO  
            break;
        case POWERUP_SPEED:
            // adjust powerup effect / speed here
            // OBS adjust also in the move_character upon timer expiration 
            if (character->speedPowerupTime == 0) character->speed += 20;   
            // adjust powerup effect time here, now it worx for 10 seconds only
            character->speedPowerupTime = SDL_GetTicks() + 10000;   
            play_sound_once(init_sound_effect("resources/music/horror3.mp3", 100));  
            break;
        case POWERUP_INVISIBLE:
            character->visible = 0;
            character->invisiblePowerupTime = SDL_GetTicks() + 10000;
            play_sound_once(init_sound_effect("resources/music/horror2.mp3", 100)); 
            break;
    }
}  

