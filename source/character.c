#include "character.h"
#include "texture.h"
#include "collisions.h"
#include "tilemap.h"
#include "music.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

int speed = 300 / 60;

void init_character(Character* character, SDL_Renderer *pRenderer, const char *filePath){
    create_texture(&character->texture, pRenderer, filePath);
    SDL_QueryTexture(character->texture, NULL, NULL, &character->rect.w, &character->rect.h);
    character->rect.x = 200;
    character->rect.y = 200;
    character->rect.w /= 4;
    character->rect.h /= 4;
}   


void move_character(Character *character, TileMap *tilemap, 
                    int WINDOW_WIDTH, int WINDOW_HEIGHT, 
                    int up, int down, int left, int right, 
                    Character **other_characters, int num_other_characters) { 

    char soundPath2[] = "resources/music/sse2.mp3"; 
    char oiSoundPath[] = "resources/music/oi.mp3"; 

    Single_sound *wall = init_sound_effect(soundPath2, 10); 
    Single_sound *oi = init_sound_effect(oiSoundPath, 30);                   

    SDL_Rect nextPosition = character->rect;
    nextPosition.y += (down - up) * speed;  
    nextPosition.x += (right - left) * speed;

    // Check collision with the environment
    if (collides(&nextPosition, tilemap, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        play_sound_once(wall);
        return;  // collision with the world, return immediately
    }

    // Check collision with other characters
    for (int i = 0; i < num_other_characters; i++) {
        if (character != other_characters[i] && characters_collide(&nextPosition, &other_characters[i]->rect)) {
            play_sound_once(oi);
            return; // collision with another character, return immediately
        }
    }

    // Update the character's position if no collisions occurred
    character->rect = nextPosition;
}


void draw_character(SDL_Renderer *pRenderer, Character *character) {
    SDL_Rect destRect = character->rect;  
    destRect.w = character->rect.w;  
    destRect.h = character->rect.h;  
    SDL_RenderCopy(pRenderer, character->texture, NULL, &destRect);
    
    
}

void cleanup_character(Character* character) {
    if (character != NULL) {
        if (character->texture != NULL) {
            SDL_DestroyTexture(character->texture); 
            character->texture = NULL;  
        }
    }
}