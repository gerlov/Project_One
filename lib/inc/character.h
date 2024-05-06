#ifndef CHARACTER_H
#define CHARACTER_H

#include <SDL2/SDL.h>
#include "tilemap.h"

///@struct Character
///@brief Represents a game character with a position, size, and texture.
///The isHunter == 1 if they are a hunter, 0 otherwise.
typedef struct Character{
    SDL_FPoint position;
    SDL_FPoint velocity;
    SDL_Rect rect;
    SDL_Texture* texture;
    int isHunter;
    int isKilled;
    char direction;
    int isMoving;
    int currentFrame;
    int health, speed, visible;
    Uint32 speedPowerupTime; // speed powerup timer
    Uint32 invisiblePowerupTime; // invisibility powerup timer  
    Uint32 frameLastUpdated;
} Character;



///@brief Stops the movement of the character.
///@param character Pointer to the Character whose movement is to be stopped.
void stop_moving(Character *character);

///@brief Sets the direction of movement for the character.
///@param character Pointer to the Character whose direction is to be set.
///@param direction The direction to set
void set_direction(Character *character, char direction);


///@brief Hunt function for a hunter character against other characters within kill distance.
///@param hunter Pointer to the Character designated as the hunter; must have isHunter set to 1.
///@param characters Array of pointers to Character structures, including both hunters and non-hunters.
///@param num_characters Total number of characters in the characters array.
void kill_command(Character *hunter, Character **characters, int num_characters);


///@brief Initializes a character with a texture loaded from a specified file path.
///@param character Pointer to the Character to be initialized.
///@param pRenderer The renderer used to create the texture.
///@param filePath Path to the image file used to create the character's texture.
///@return Returns an initialized character with set strcutres
Character* init_character(SDL_Renderer* pRenderer, const char* filePath, int isHunter);

///@brief Updates the position of the character based on input and checks for collisions with the environment and other characters.
///@param character Pointer to the Character that is being moved.
///@param tilemap Reference to the TileMap for collision checking against the map's tiles.  
///@param deltaTime Time since the last frame in seconds.
///@param other_characters Array of pointers to other Character structures in the game for collision detection.
///@param num_other_characters Number of characters in the other_characters array to check for collisions against.
void move_character(Character *character, TileMap *tilemap, 
                    float deltaTime, 
                    Character **other_characters, int num_other_characters);

///@brief Draws the character on the provided renderer.
///@param pRenderer The renderer where the character will be drawn.
///@param character Pointer to the Character to be drawn.
///@param camera The camera to adjust for.
void draw_character(SDL_Renderer* pRenderer, Character* character, bool isMainCharacter, SDL_FPoint *camera);


///@brief Frees the resources associated with a character.
///@param character Pointer to the Character whose resources need to be freed.
void cleanup_character(Character* character);

/// @brief Moves the camera to follow the player character within the game window.
/// @param camera the camera position to update
/// @param player the character rectangle to follow
/// @param WINDOW_WIDTH the width of the game window
/// @param WINDOW_HEIGHT the height of the game window
void follow_player(SDL_FPoint *camera, SDL_Rect *player, int WINDOW_WIDTH, int WINDOW_HEIGHT);  

/// @brief updates the character's rectangle xy based on the character's position
void update_character_rect(Character *character, SDL_FPoint *position);


/// @brief gets the center of the character's rectangle
/// @return the center of the character's rectangle
SDL_FPoint get_character_center(Character *character);

///@brief Initializes player-related sounds 
void init_player_sounds();

///@brief Cleans up player-related sound resources 
void cleanup_player_sounds();

#endif