#include "character.h"
#include "texture.h"
#include "collisions.h"
#include "tilemap.h"
#include "music.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

int speed = 300 / 60;
int hunter_characters = 0;

void init_character(Character* character, SDL_Renderer *pRenderer, const char *filePath, int isHunter){
    create_texture(&character->texture, pRenderer, filePath);
    SDL_QueryTexture(character->texture, NULL, NULL, &character->rect.w, &character->rect.h);
    character->rect.x = 100;
    character->rect.y = 100;
    character->rect.w /= 4;
    character->rect.h /= 4;
    if(hunter_characters == 0 && isHunter == 1){
        hunter_characters = 1;
        character->isHunter = isHunter;
    }else{
        character->isHunter = 0;
    }

}

void move_character(Character *character, TileMap *tilemap, int WINDOW_WIDTH, int WINDOW_HEIGHT, int up, int down, int left, int right){
    char soundPath2[] = "resources/music/sse2.mp3";
    Single_sound *wall = init_sound_effect(soundPath2, 10);

    SDL_Rect nextPosition = character->rect; 
    nextPosition.y += (down - up) * speed;  // Vertical movement
    nextPosition.x += (right - left) * speed;  // Horizontal movement

    // Check for collision with the tilemap
    if (!collides(&nextPosition, tilemap, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        character->rect = nextPosition; // Update position if no collision
        return;  // No collision occurred
    } else play_sound_once(wall);     
    
    return;
}

void kill_command(Character *character_one, Character *character_two){
    
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