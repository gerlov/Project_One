#include "powerup.h"  

PowerUp powerUps[MAX_POWERUPS];
int powerUpCount = 0; 

SDL_Texture* textureSpeed;
SDL_Texture* textureInvisible;
SDL_Texture* textureSkull;
Single_sound* soundSpeed;
Single_sound* soundInvisible;
Single_sound* soundSkull;



void load_powerup_resources(SDL_Renderer* renderer) {  
    create_texture(&textureSpeed, renderer, "resources/testpack/red.png");
    create_texture(&textureInvisible, renderer, "resources/testpack/green.png"); 
    create_texture(&textureSkull, renderer, "resources/testpack/skull.png"); 
    soundSpeed = init_sound_effect("resources/music/horror3.mp3", 100);
    soundInvisible = init_sound_effect("resources/music/horror2.mp3", 100); 
    soundSkull = init_sound_effect("resources/music/horror1.mp3", 100);
}


void cleanup_powerup_resources() {
    SDL_DestroyTexture(textureSpeed);
    SDL_DestroyTexture(textureInvisible);
    SDL_DestroyTexture(textureSkull);
    free_sse(soundSpeed);
    free_sse(soundInvisible);
    free_sse(soundSkull);
}


void init_powerUps(SDL_Renderer* renderer, TileMap* tilemap, int tile_size) {
    int placedCount = 0;
    char powerUpPlacement[tilemap->width][tilemap->height];
    memset(powerUpPlacement, 0, sizeof(powerUpPlacement)); //stk  


    for (int i = 0; i < MAX_POWERUPS; i++) {
        int x = rand() % tilemap->width;
        int y = rand() % tilemap->height;
        Tile* tile = get_tile(tilemap, x, y);


        if (tile->type == TILE_FLOOR && powerUpPlacement[x][y] == 0) {
            int powerUpType = rand() % 3; // rand type of power-up
            PowerUpType type;
            SDL_Texture* texture; 


            switch (powerUpType) {
                case 0:
                    type = POWERUP_SPEED;
                    texture = textureSpeed;
                    break;
                case 1:
                    type = POWERUP_INVISIBLE;
                    texture = textureInvisible;
                    break;
                case 2:
                    type = POWERUP_SKULL;
                    texture = textureSkull;
                    break;
            }


            powerUps[placedCount] = (PowerUp){
                .rect = {x * tile_size, y * tile_size, tile_size, tile_size},
                .visible = 1,
                .texture = texture,
                .type = type
            };

            powerUpPlacement[x][y] = 1;
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
        //case POWERUP_HEALTH:
            // dont really know what to do with health yet cos we dont apply any damage yet?
            // we only kill x_x 
            // character->health += 50;  
            // TODO  
           // break;
        case POWERUP_SPEED:
            // adjust powerup effect / speed here
            // OBS adjust also in the move_character upon timer expiration 
            if (character->speedPowerupTime == 0) character->speed += 20;   
            // adjust powerup effect time here, now it worx for 10 seconds only
            character->speedPowerupTime = SDL_GetTicks() + 10000;
            play_sound_once(soundSpeed); 
            break;
        case POWERUP_INVISIBLE:
            character->visible = 0;
            character->invisiblePowerupTime = SDL_GetTicks() + 3000; // now invisible for 3 sec only, change here
            play_sound_once(soundInvisible);
            break;  
        case POWERUP_SKULL: 
            if (character->isHunter) {  
                play_sound_once(soundSkull); 
                for (int i = 0; i < num_characters; i++) {
                    if (characters[i] != character && characters[i]->visible == 1) { // kills only visible characters
                        characters[i]->isKilled = 1;
                    }
                }
            }
            break;     
    }
}  

