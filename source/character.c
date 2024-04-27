#include "character.h" 
#include "texture.h"
#include "collisions.h"
#include "tilemap.h"    
#include "powerup.h"
#include <math.h> 
#include "music.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

// int speed = 300 / 60;
int hunter_characters = 0; 
Single_sound *wall_sound;
Single_sound *oi_sound;  
Single_sound *kill_sound; 

void init_player_sounds() {
    wall_sound = init_sound_effect("resources/music/sse2.mp3", 10);
    oi_sound = init_sound_effect("resources/music/oi.mp3", 30);
    kill_sound = init_sound_effect("resources/music/sse1.mp3", 30);
}

void cleanup_player_sounds() {
    free_sse(wall_sound);
    free_sse(oi_sound); 
    free_sse(kill_sound);
}

Character* init_character(SDL_Renderer *pRenderer, const char *filePath, int isHunter){
    Character* character = malloc(sizeof(Character));
    if(character == NULL){
        printf("Error creating character");
        return NULL;
    }
    create_texture(&character->texture, pRenderer, filePath);
    SDL_QueryTexture(character->texture, NULL, NULL, &character->rect.w, &character->rect.h);
    character->rect.x = 400;
    character->rect.y = 200;
    character->rect.w /= 4;
    character->rect.h /= 4;
    character->direction='d';
    if(hunter_characters == 0 && isHunter == 1){
        hunter_characters = 1;
        character->isHunter = isHunter;
    }else{
        character->isHunter = 0;
    }
    character->isKilled = 0;
    character->isMoving = 0;
    character->currentFrame = 0;
    character->frameLastUpdated = SDL_GetTicks(); 
    character -> health = 100; 
    character -> speed = 5;  // aka 300 / 60 
    character -> visible = 1; 
    // powerup timers init: 
    character -> speedPowerupTime = character -> invisiblePowerupTime = 0;  
    return character;
}

void set_direction(Character *character, char direction){
    character->direction = direction;
    character->isMoving = 1;
}

void stop_moving(Character *character){
    character->isMoving = 0;
}

void move_character(Character *character, TileMap *tilemap,   
                    int up, int down, int left, int right, 
                    Character **other_characters, int num_other_characters, 
                    MazeView *mazeView) { 

    if(character->isKilled == 1) return; 

    Uint32 currentTicks = SDL_GetTicks(); // for powerup timers  

    //  resets speed and visibility for a character if powerup time has expired
    if (currentTicks > character->speedPowerupTime && character->speedPowerupTime != 0) {
        character->speed -= 20;  // set back to default speed (defined in init)
        character->speedPowerupTime = 0;  // reset poerup timer
    } 

    if (currentTicks > character->invisiblePowerupTime && character->invisiblePowerupTime != 0) {
        character->visible = 1;  // visible again
        character->invisiblePowerupTime = 0;  //reset powerup timer
    }    

    if (currentTicks > mazeView->displayTime && 
        mazeView->displayTime != 0 && 
        character->invisiblePowerupTime < currentTicks) {
            character->visible = 1;  
}


    SDL_Rect nextPosition = character->rect;
    nextPosition.y += (down - up) * character -> speed;  
    nextPosition.x += (right - left) * character -> speed; 

    // Check collision with the walls
    if (collides(&nextPosition, tilemap, TILE_WALL)) {  
        play_sound_once(wall_sound);
        return;  
    }

    // collision with other characters (remove if not needed)
    for (int i = 0; i < num_other_characters; i++) {
        if (character != other_characters[i] && 
            other_characters[i]->isKilled == 0 && 
            SDL_HasIntersection(&nextPosition, &other_characters[i]->rect))  return;  
    }

    // powerup tiles intersection 
     for (int i = 0; i < powerUpCount; i++) { 
        if (powerUps[i].visible && SDL_HasIntersection(&character->rect, &powerUps[i].rect)) {  
            if (powerUps[i].type == POWERUP_SKULL && character->isHunter == 0) {
                continue; // dont apply POWRUP_SKULL and DONT deactivate it if intersected by non-hunter
            }
            apply_powerUp(character, powerUps[i].type, other_characters, num_other_characters, mazeView);
            powerUps[i].visible = 0;  
        }
    }

    // Update the character's position if no collisions occurred
    character->rect = nextPosition;
}

// TODO: Fix so that we just send in one character and checks if any of the other characters are in range to kill
void kill_command(Character *hunter, Character **characters, int num_characters) {
    if(hunter == NULL) return; 
    if(!hunter->isHunter) return;

    const int killDistance = 80; // This represents the maximum distance in pixels

    for (int i = 0; i < num_characters; i++) {
        if(characters[i] == NULL) continue;
        if (characters[i]->isHunter == 0 && !characters[i]->isKilled) {
            int y_distance = abs(hunter->rect.y - characters[i]->rect.y);
            int x_distance = abs(hunter->rect.x - characters[i]->rect.x);
            if (y_distance <= killDistance && x_distance <= killDistance) {
                characters[i]->isKilled = 1; // Change target to kille
                cleanup_character(characters[i]);
                // Teleport to the target
                hunter->rect.x = characters[i]->rect.x;
                hunter->rect.y = characters[i]->rect.y;
                // kill sound  
                play_sound_once(kill_sound);               
                break; //Kills one, otherwise more in a small space
            }
        }
    }
}

void draw_character(SDL_Renderer *pRenderer, Character *character, SDL_FPoint *camera) {

    // only draw characters that are visible and not killed
    if (!character->visible || character->isKilled) return;

    // Needs to be same for all 
    // 128 x 192
    const int frameWidth = 32; // Sprite sheet width / frames per column
    const int frameHeight = 48; // Sprite sheet height / frames per row
    const int frameCount = 4; //Each frame in each row
    Uint32 timeSinceLastFrame = SDL_GetTicks() - character->frameLastUpdated;


    if(character->isMoving && timeSinceLastFrame > (700 / frameCount)) {
        character->currentFrame = (character->currentFrame + 1) % frameCount;
        character->frameLastUpdated = SDL_GetTicks();
    }


    SDL_Rect srcRect;
    srcRect.w = frameWidth;
    srcRect.h = frameHeight;
    srcRect.x = character->currentFrame * frameWidth;
    

    switch (character->direction) {
        case 'd': srcRect.y = 0 * frameHeight; break;
        case 'l': srcRect.y = 1 * frameHeight; break; 
        case 'r': srcRect.y = 2 * frameHeight; break; 
        case 'u': srcRect.y = 3 * frameHeight; break; 
        default:  srcRect.y = 0 * frameHeight; break; 
    }



    SDL_Rect destRect = character->rect;  
    destRect.w = character->rect.w;  
    destRect.h = character->rect.h;
    // Adjust the destination rectangle based on the tilemap's position
    destRect.x -= camera->x;
    destRect.y -= camera->y;
    SDL_RenderCopy(pRenderer, character->texture, &srcRect, &destRect);
    
    
}    

void draw_character_on_mazeview(Character *character, TileMap* tilemap, 
                                int window_width, int window_height,
                                MazeView *mazeView, SDL_Renderer *renderer) 
{
    // scale factor same as in the mazevie, prbbly need to refactor / DRY 
    float scale_factor_w = (float)window_width / (tilemap->width * tilemap->tile_size);
    float scale_factor_h = (float)window_height / (tilemap->height * tilemap->tile_size);
    float scale_factor = (scale_factor_w < scale_factor_h) ? scale_factor_w : scale_factor_h;

    // apply scale factor to current caracters position
    int scaled_x = character->rect.x * scale_factor;
    int scaled_y = character->rect.y * scale_factor;

    // will display on a mazeview as a small red (dot) rectangle
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  
    SDL_Rect dot = {scaled_x, scaled_y, 10, 10 }; 

    // ensure red dot is drawn relative to the MazeView's position on screen (its centered)
    dot.x += (window_width - mazeView->viewRect.w) / 2;
    dot.y += (window_height - mazeView->viewRect.h) / 2;

    SDL_RenderFillRect(renderer, &dot); // render 
}



void cleanup_character(Character* character) {
    if (character != NULL) {
        if (character->texture != NULL) {
            SDL_DestroyTexture(character->texture); 
            character->texture = NULL;  
        }
    }
}


void follow_player(SDL_FPoint *camera, SDL_Rect *player, int WINDOW_WIDTH, int WINDOW_HEIGHT) {
    camera->x = player->x - WINDOW_WIDTH / 2 + player->w / 2;
    camera->y = player->y - WINDOW_HEIGHT / 2 + player->h / 2;
}
