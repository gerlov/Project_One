#include "character.h" 
#include "tilemap.h"  
#include "music.h" 



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
                cleanup_character(characters[i]);
                // Teleport to the target
                hunter->rect.x = characters[i]->rect.x;
                hunter->rect.y = characters[i]->rect.y;
                // kill sound
                char soundPath[] = "resources/music/sse1.mp3";
                Single_sound *kill_sound = init_sound_effect(soundPath, 30);
                play_sound_once(kill_sound);
                free_sse(kill_sound);
               
                break; //Kills one, otherwise more in a small space
            }
        }
    }
}

void apply_powerup(Character *character, TileType type) {
    switch (type) {
        case POWERUP_HEALTH:
            character->health += 20;
            if (character->health > 100) character->health = 100;
            break;
        case POWERUP_SPEED:
            character->speed += 50;
            if (character->speed > 500) character->speed = 500;
            break;
        case POWERUP_INVISIBLE:
            character->visible = 0;
            break;
        // place more powerup types, either for hunter och player. 
        default:
            // handle unknown or inappropriate types
            break;
    }
}
