#include "character.h"
#include "texture.h"
#include "collisions.h"
#include "tilemap.h"
#include <math.h> 
#include "music.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

// int speed = 300 / 60;
int hunter_characters = 0;

void init_character(Character* character, SDL_Renderer *pRenderer, const char *filePath, int isHunter){
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
    character -> health = 100;    
    character -> speed = 5;       // 300 / 60
    character -> visible = 1;
}

void move_character(Character *character, TileMap *tilemap, 
                    int WINDOW_WIDTH, int WINDOW_HEIGHT, 
                    int up, int down, int left, int right, 
                    Character **other_characters, int num_other_characters) { 

    char soundPath2[] = "resources/music/sse2.mp3"; 
    char oiSoundPath[] = "resources/music/oi.mp3"; 

    if(character->isKilled == 1) return;

    Single_sound *wall = init_sound_effect(soundPath2, 10); 
    Single_sound *oi = init_sound_effect(oiSoundPath, 30);                   

    SDL_Rect nextPosition = character->rect;
    nextPosition.y += (down - up) * character -> speed;  
    nextPosition.x += (right - left) * character -> speed;

    // Check collision with the environment
    if (collides(&nextPosition, tilemap, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        play_sound_once(wall);
        return;  // collision with the world, return immediately
    }

    // Check collision with other characters
    for (int i = 0; i < num_other_characters; i++) {
        if (character != other_characters[i] && other_characters[i]->isKilled == 0 && characters_collide(&nextPosition, &other_characters[i]->rect)) {
            play_sound_once(oi);
            return; // collision with another character, return immediately
        }
    }

    // Update the character's position if no collisions occurred
    character->rect = nextPosition;
}


void draw_character(SDL_Renderer *pRenderer, Character *character, char direction) {

    SDL_Rect srcRect;
    
    if (character->isHunter == 1) {
        if (direction == 'u') {
        srcRect.x = 50;
        srcRect.y = 195;
        srcRect.w = character->rect.w * 1.3;
        srcRect.h = character->rect.h;
    } else if (direction == 'd') {
        srcRect.x = 50;
        srcRect.y = 0;
        srcRect.w = character->rect.w * 1.3;
        srcRect.h = character->rect.h;
        } else if (direction == 'l') {
            
        srcRect.x = 50;
        srcRect.y = 65;
        srcRect.w = character->rect.w * 1.3;
        srcRect.h = character->rect.h;
    } else if (direction == 'r') {
        srcRect.x = 50;
        srcRect.y = 130;
        srcRect.w = character->rect.w * 1.3;
        srcRect.h = character->rect.h;
        }
    } else {
        if (direction == 'u') {
        srcRect.x = 100;
        srcRect.y = 0;
        srcRect.w = character->rect.w;
        srcRect.h = character->rect.h;
    } else if (direction == 'd') {
        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.w = character->rect.w;
        srcRect.h = character->rect.h;
        } else if (direction == 'l') {
        srcRect.x = 50;
        srcRect.y = 0;
        srcRect.w = character->rect.w;
        srcRect.h = character->rect.h;
    } else if (direction == 'r') {
        srcRect.x = 150;
        srcRect.y = 0;
        srcRect.w = character->rect.w;
        srcRect.h = character->rect.h;
        }
    }


    SDL_Rect destRect = character->rect;  
    destRect.w = character->rect.w;  
    destRect.h = character->rect.h;  

    SDL_RenderCopy(pRenderer, character->texture, &srcRect, &destRect);
}

void cleanup_character(Character* character) {
    if (character != NULL) {
        if (character->texture != NULL) {
            SDL_DestroyTexture(character->texture); 
            character->texture = NULL;  
        }
    }
}