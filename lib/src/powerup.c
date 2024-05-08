#include "powerup.h"  

PowerUp powerUps[MAX_POWERUPS];
int powerUpCount = 0; 

SDL_Texture* textureSpeed;
SDL_Texture* textureInvisible;
Single_sound* soundSpeed;
Single_sound* soundInvisible;


void load_powerup_resources(SDL_Renderer* renderer) {  
    create_texture(&textureSpeed, renderer, "../lib/assets/testpack/red.png");
    create_texture(&textureInvisible, renderer, "../lib/assets/testpack/green.png"); 
    soundSpeed = init_sound_effect("../lib/assets/music/horror3.mp3");
    soundInvisible = init_sound_effect("../lib/assets/music/horror2.mp3");
}


void cleanup_powerup_resources() {
    SDL_DestroyTexture(textureSpeed);
    SDL_DestroyTexture(textureInvisible);
    free_sse(soundSpeed);
    free_sse(soundInvisible);
}


void init_powerUps(SDL_Renderer* renderer, TileMap* tilemap, int tile_size) {
    int placedCount = 0;
    for (int i = 0; i < MAX_POWERUPS; i++) {
        int x = our_rand() % tilemap->width;
        int y = our_rand() % tilemap->height;
        Tile* tile = get_tile(tilemap, x, y);

        if (tile->type == TILE_FLOOR) {
            int powerUpType = our_rand() % 2; // rand type of power-up
            PowerUpType type = (powerUpType == 0) ? POWERUP_SPEED : POWERUP_INVISIBLE;
            SDL_Texture* texture = (powerUpType == 0) ? textureSpeed : textureInvisible;

            powerUps[placedCount] = (PowerUp){
                .rect = {x * tile_size, y * tile_size, tile_size, tile_size},
                .visible = 1,
                .texture = texture,
                .type = type
            };
            placedCount++;
        }
    }
    powerUpCount = placedCount;
}



void draw_powerUps(SDL_Renderer* renderer, TileMap* tilemap) {
    if (!renderer) return; 

    SDL_Rect srcRect = {0, 0, 16, 16};
    SDL_Rect dstRect = {0, 0, 32, 32};  

    for (int i = 0; i < powerUpCount; i++) {
        if (powerUps[i].visible) { 
            srcRect.x = (SDL_GetTicks() / 100 % 5) * 16;
            dstRect.x = powerUps[i].rect.x - tilemap->camera.x;
            dstRect.y = powerUps[i].rect.y - tilemap->camera.y;
            SDL_RenderCopy(renderer, powerUps[i].texture, &srcRect, &dstRect);
        }
    }
}


void apply_powerUp(Character *character, PowerUpType type, Character **characters, int num_characters) {
    switch (type) {
        case POWERUP_SPEED:
            // adjust powerup effect / speed here
            // OBS adjust also in the move_character upon timer expiration 
            if (character->speedPowerupTime == 0) character->speed += 600;   
            // adjust powerup effect time here, now it worx for 10 seconds only
            character->speedPowerupTime = SDL_GetTicks() + 10000;
            play_sound_once(soundSpeed, 5); 
            break;
        case POWERUP_INVISIBLE:
            character->visible = 0;
            character->invisiblePowerupTime = SDL_GetTicks() + 3000; // now invisible for 3 sec only, change here
            play_sound_once(soundInvisible, 5);
            break;  
    }
}  

