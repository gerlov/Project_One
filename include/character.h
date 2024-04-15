#ifndef CHARACTER_H
#define CHARACTER_H

#include <SDL2/SDL.h>
#include "tilemap.h"

///@struct Character
///@brief Represents a game character with a position, size, and texture.
///The isHunter == 1 if they are a hunter, 0 otherwise.
typedef struct Character{
    SDL_Rect rect;
    SDL_Texture* texture;
    int isHunter;
    int isKilled;
} Character;



void kill_command(Character *hunter, Character **characters, int num_characters);



///@brief Initializes a character with a texture loaded from a specified file path.
///@param character Pointer to the Character to be initialized.
///@param pRenderer The renderer used to create the texture.
///@param filePath Path to the image file used to create the character's texture.
void init_character(Character* character, SDL_Renderer* pRenderer, const char* filePath, int isHunter);

///@brief Updates the position of the character based on input and checks for collisions with the environment and other characters.
///@param character Pointer to the Character that is being moved.
///@param tilemap Reference to the TileMap for collision checking against the map's tiles.
///@param WINDOW_WIDTH The width of the game window to consider for boundary collisions.
///@param WINDOW_HEIGHT The height of the game window to consider for boundary collisions.
///@param up Boolean flag indicating upward movement.
///@param down Boolean flag indicating downward movement.
///@param left Boolean flag indicating leftward movement.
///@param right Boolean flag indicating rightward movement.
///@param other_characters Array of pointers to other Character structures in the game for collision detection.
///@param num_other_characters Number of characters in the other_characters array to check for collisions against.
void move_character(Character *character, TileMap *tilemap, 
                    int WINDOW_WIDTH, int WINDOW_HEIGHT, 
                    int up, int down, int left, int right, 
                    Character **other_characters, int num_other_characters);

///@brief Draws the character on the provided renderer.
///@param pRenderer The renderer where the character will be drawn.
///@param character Pointer to the Character to be drawn.
void draw_character(SDL_Renderer* pRenderer, Character* character);

///@brief Frees the resources associated with a character.
///@param character Pointer to the Character whose resources need to be freed.
void cleanup_character(Character* character);

#endif