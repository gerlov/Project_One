#include "character.h"
#include "texture.h"
#include "collisions.h"
#include "tilemap.h"
#include <math.h> 
#include "music.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

int speed = 300 / 60;
int hunter_characters = 0;

void init_character(Character* character, SDL_Renderer *pRenderer, const char *filePath, int isHunter){
    create_texture(&character->texture, pRenderer, filePath);
    SDL_QueryTexture(character->texture, NULL, NULL, &character->rect.w, &character->rect.h);
    character->rect.x = 400;
    character->rect.y = 200;
    character->rect.w /= 4;
    character->rect.h /= 4;
    if(hunter_characters == 0 && isHunter == 1){
        hunter_characters = 1;
        character->isHunter = isHunter;
    }else{
        character->isHunter = 0;
    }
    character->isKilled = 0;
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
    nextPosition.y += (down - up) * speed;  
    nextPosition.x += (right - left) * speed;

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

// TODO: Fix so that we just send in one character and checks if any of the other characters are in range to kill
void kill_command(Character *hunter, Character **characters, int num_characters) {
    if(hunter == NULL) return; 

    const int killDistance = 80; // This represents the maximum distance in pixels

    for (int i = 0; i < num_characters; i++) {
        if(characters[i] == NULL) continue;
        if (characters[i]->isHunter == 0 && !characters[i]->isKilled) {
            int y_distance = abs(hunter->rect.y - characters[i]->rect.y);
            int x_distance = abs(hunter->rect.x - characters[i]->rect.x);
            if (y_distance <= killDistance && x_distance <= killDistance) {
                characters[i]->isKilled = 1; // Change target to kille
                // If we want to teleport to the target, however doesnt work with collision of chars
                // hunter->rect.x =characters[i]->rect.x; 
                // hunter->rect.y = characters[i]->rect.y;
                // Play sound effect for kill
                char soundPath[] = "resources/music/sse1.mp3";
                Single_sound *kill_sound = init_sound_effect(soundPath, 30);
                play_sound_once(kill_sound);
                free_sse(kill_sound);
            }
        }
    }
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