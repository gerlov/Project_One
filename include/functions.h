#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "character.h" 
#include "tilemap.h"  

///@brief Hunt function for a hunter character against other characters within kill distance.
///@param hunter Pointer to the Character designated as the hunter; must have isHunter set to 1.
///@param characters Array of pointers to Character structures, including both hunters and non-hunters.
///@param num_characters Total number of characters in the characters array.
void kill_command(Character *hunter, Character **characters, int num_characters);

void apply_powerup(Character *character, TileType type);

#endif // FUNCTIONS_H
