#include "powerup.h"  

PowerUp powerUps[MAX_POWERUPS];
int powerUpCount = 0; 

SDL_Texture* textureSpeed;
SDL_Texture* textureInvisible;
SDL_Texture* textureSkull;
SDL_Texture* textureMap; 
Single_sound* soundSpeed;
Single_sound* soundInvisible;
Single_sound* soundSkull; 
Single_sound* soundElevator10;


void load_powerup_resources(SDL_Renderer* renderer) {  
    create_texture(&textureSpeed, renderer, "../lib/assets/powerup_pack/red.png");
    create_texture(&textureInvisible, renderer, "../lib/assets/powerup_pack/green.png"); 
    create_texture(&textureSkull, renderer, "../lib/assets/powerup_pack/skull.png");  
    create_texture(&textureMap, renderer, "../lib/assets/powerup_pack/map.png"); 
    soundSpeed = init_sound_effect("../lib/assets/music/horror3.mp3");
    soundInvisible = init_sound_effect("../lib/assets/music/horror2.mp3");
    soundSkull = init_sound_effect("../lib/assets/music/horror1.mp3");
    soundElevator10 = init_sound_effect("../lib/assets/music/elevator10.mp3");
}


void cleanup_powerup_resources() {
    SDL_DestroyTexture(textureSpeed);
    SDL_DestroyTexture(textureInvisible);
    SDL_DestroyTexture(textureSkull); 
    SDL_DestroyTexture(textureMap); 
    free_sse(soundSpeed);
    free_sse(soundInvisible);
    free_sse(soundSkull);
    free_sse(soundElevator10);
}


void init_powerUps(SDL_Renderer* renderer, TileMap* tilemap, int tile_size) 
{  

    int placedCount = 0;
    char powerUpPlacement[tilemap->width][tilemap->height];
    memset(powerUpPlacement, 0, sizeof(powerUpPlacement)); //stk  

    int skullCount = 0;  
    int maxSkull = 3;  ////////////////////////////// OBS OBS OBS CHANGE HERE


    for (int i = 0; i < MAX_POWERUPS; i++) {
        int x = our_rand() % tilemap->width;
        int y = our_rand() % tilemap->height;
        Tile* tile = get_tile(tilemap, x, y);


        if (tile->type == TILE_FLOOR && powerUpPlacement[x][y] == 0) {
            int powerUpType; 
            PowerUpType type;
            SDL_Texture* texture; 


            if (skullCount < maxSkull) {
                powerUpType = 3; // force skull powerup type
                skullCount++;    
            } 
            else powerUpType = our_rand() % 3; 

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
                    type = POWERUP_MAP;
                    texture = textureMap;
                    break;      
                case 3:
                    type = POWERUP_SKULL;
                    texture = textureSkull;
                    break;    
            }


            powerUps[placedCount] = (PowerUp){
                .powerupid = i, 
                .rect = {x * tile_size, y * tile_size, tile_size, tile_size},
                .active = 1,
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
        if (powerUps[i].active) {
            if (powerUps[i].texture == textureMap) {
                //  map (simple 16x16 px icon)
                srcRect.x = 0;
            } else {
                // others are spritesheets (5 st 16x16 px frames per sheet)
                srcRect.x = (SDL_GetTicks() / 100 % 5) * 16;
            }
            dstRect.x = powerUps[i].rect.x - tilemap->camera.x;
            dstRect.y = powerUps[i].rect.y - tilemap->camera.y;
            SDL_RenderCopy(renderer, powerUps[i].texture, &srcRect, &dstRect);
        }
    }
}


void apply_powerUp(Character *character, PowerUpType type, Character **characters, 
                   int num_characters, MazeView *mazeView) 
{
    switch (type) {
        case POWERUP_SPEED:
                character->speed += 200;
                character->speedPowerupTime = SDL_GetTicks() + 10000;
                play_sound_once(soundSpeed, 10);
            break;
        case POWERUP_INVISIBLE:
                character->visible = 0;
                character->invisiblePowerupTime = SDL_GetTicks() + 10000;
                play_sound_once(soundInvisible, 10);
            break;
        case POWERUP_SKULL:
            if (character->isHunter) {  
                for (int i = 0; i < num_characters; i++) {
                    if (characters[i] != character && characters[i]->visible == 1) {
                        characters[i]->isKilled = 1;
                    }
                }
                play_sound_once(soundSkull, 10);
            } 
            break;
        case POWERUP_MAP:
            character->visible = 0;
            mazeView->visible = 1;
            mazeView->displayTime = SDL_GetTicks() + 10000; // display maze for 10 sec
            play_sound_once(soundElevator10, 10);
            break;
    }
}  
