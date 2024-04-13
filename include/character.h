#ifndef CHARACTER_H
#define CHARACTER_H

#include <SDL2/SDL.h>
#include "tilemap.h"

///@struct Character
///@brief Represents a game character with a position, size, and texture.
typedef struct Character{
    SDL_Rect rect;
    SDL_Texture* texture;
} Character;

///@brief Initializes a character with a texture loaded from a specified file path.
///@param character Pointer to the Character to be initialized.
///@param pRenderer The renderer used to create the texture.
///@param filePath Path to the image file used to create the character's texture.
void init_character(Character* character, SDL_Renderer* pRenderer, const char* filePath);

///@brief Updates the position of the character based on input and checks for collisions.
///@param character Pointer to the Character that is being moved.
///@param tilemap Reference to the TileMap for collision checking.
///@param WINDOW_WIDTH The width of the window to consider for boundary collisions.
///@param WINDOW_HEIGHT The height of the window to consider for boundary collisions.
///@param up Boolean flag indicating upward movement.
///@param down Boolean flag indicating downward movement.
///@param left Boolean flag indicating leftward movement.
///@param right Boolean flag indicating rightward movement.
void move_character(Character* character, TileMap* tilemap, int WINDOW_WIDTH, int WINDOW_HEIGHT, int up, int down, int left, int right);

///@brief Draws the character on the provided renderer.
///@param pRenderer The renderer where the character will be drawn.
///@param character Pointer to the Character to be drawn.
void draw_character(SDL_Renderer* pRenderer, Character* character);

///@brief Frees the resources associated with a character.
///@param character Pointer to the Character whose resources need to be freed.
void cleanup_character(Character* character);

#endif