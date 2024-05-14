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

const char *characterFiles[] = {
    "../lib/assets/characters/warriorTwo.png",
    "../lib/assets/characters/femaleOne.png",
    "../lib/assets/characters/maleOne.png",
    "../lib/assets/characters/warriorOne.png",
    "../lib/assets/characters/maleOne.png"};
const char *hunterClothes[] = {
    "../lib/assets/characters/monster.png"};

void init_player_sounds()
{
    wall_sound = init_sound_effect("../lib/assets/music/sse2.mp3");
    oi_sound = init_sound_effect("../lib/assets/music/oi.mp3");
    kill_sound = init_sound_effect("../lib/assets/music/sse1.mp3");
}

void cleanup_player_sounds()
{
    free_sse(wall_sound);
    free_sse(oi_sound);
    free_sse(kill_sound);
}

Character *init_character(SDL_Renderer *pRenderer, const char *filePath, int isHunter)
{
    Character *character = malloc(sizeof(Character));
    if (character == NULL)
    {
        printf("Error creating character");
        return NULL;
    }

    character->position = (SDL_FPoint){0, 0};
    character->velocity = (SDL_FPoint){0, 0};
    update_character_rect(character, &character->position);
    character->direction = 'd';
    char *file;
    if (hunter_characters == 0 && isHunter == 1)
    {
        hunter_characters = 1;
        character->isHunter = isHunter;
    }
    else
    {
        character->isHunter = 0;
    }
    create_texture(&character->texture, pRenderer, filePath);
    SDL_QueryTexture(character->texture, NULL, NULL, &character->rect.w, &character->rect.h);
    character->rect.w /= 4;
    character->rect.h /= 4;
    character->isKilled = 0;
    character->isMoving = 0;
    character->currentFrame = 0;
    character->frameLastUpdated = SDL_GetTicks();
    character->health = 100;
    character->speed = 300;
    character->visible = 1;
    // powerup timers init:
    character->speedPowerupTime = character->invisiblePowerupTime = 0;
    return character;
}

void set_direction(Character *character, char direction)
{
    if (fabsf(character->velocity.x) > fabsf(character->velocity.y))
    {
        if (character->velocity.x > 0)
        {
            character->direction = 'r';
        }
        else
        {
            character->direction = 'l';
        }
        character->isMoving = 1;
    }
    else if (fabsf(character->velocity.x) < fabsf(character->velocity.y))
    {
        if (character->velocity.y > 0)
        {
            character->direction = 'd';
        }
        else
        {
            character->direction = 'u';
        }
        character->isMoving = 1;
    }
    else {
        character->isMoving = 0;
    }
}

void stop_moving(Character *character)
{
    character->isMoving = 0;
}

void move_character(Character *character, TileMap *tilemap,
                    float deltaTime, Character **other_characters, 
                    int num_other_characters, MazeView *mazeView)
{

    if (character->isKilled == 1) return;
    Uint32 currentTicks = SDL_GetTicks(); // for powerup timers

    // 2 st check for powerup timers expiration, resets everything to default values if expired
    if (currentTicks > character->speedPowerupTime && character->speedPowerupTime != 0)
    {
        character->speed -= 200;         // set back to default speed (defined in init)
        character->speedPowerupTime = 0; // reset poerup timer
    }

    if (currentTicks > character->invisiblePowerupTime && character->invisiblePowerupTime != 0)
    {
        character->visible = 1;              // visible again
        character->invisiblePowerupTime = 0; // reset powerup timer
    }

    if (currentTicks > mazeView->displayTime && 
        mazeView->displayTime != 0 && 
        character->invisiblePowerupTime < currentTicks) {
            character->visible = 1;  
    }

    // Normalize velocity
    float magnitude = sqrt(character->velocity.x * character->velocity.x + character->velocity.y * character->velocity.y);
    if (magnitude != 0)
    {
        character->velocity.x /= magnitude;
        character->velocity.x *= character->speed;
        character->velocity.y /= magnitude;
        character->velocity.y *= character->speed;
    }

    // set_direction(character, character->direction);
    // Collision detection and movement
    // inspired by Jonathan Whiting at https://jonathanwhiting.com/tutorial/collision/

    SDL_FPoint oldPosition = character->position;
    SDL_FPoint preNewPosition = character->position;
    SDL_FPoint newPosition = character->position;
    bool hasCollided = false;

    // Check the x-axis
    newPosition.x += character->velocity.x * deltaTime;
    // Update the character's rect to the new position
    update_character_rect(character, &newPosition);
    // Check collision with the walls on the x-axis
    if (collides(&character->rect, tilemap, TILE_WALL))
    {
        // Collision with the wall, reset the position
        newPosition = preNewPosition;
        hasCollided = true;
    }
    // Update the character's rect to the new position
    update_character_rect(character, &newPosition);

    // Prepare for the next check
    preNewPosition = newPosition;

    // Check the y-axis
    newPosition.y += character->velocity.y * deltaTime;

    update_character_rect(character, &newPosition);
    // Check collision with the walls on the y-axis
    if (collides(&character->rect, tilemap, TILE_WALL))
    {
        // Collision with the wall, reset the position
        newPosition = preNewPosition;
        hasCollided = true;
    }
    // Set the new position and update the character's rect
    character->position = newPosition;
    update_character_rect(character, &newPosition);

    if (hasCollided)
    {
        // Has collided with a wall, play sound and return
        play_sound_once(wall_sound, 2);
        return;
    }

    // collision with other characters (remove if not needed)
    for (int i = 0; i < num_other_characters; i++)
    {
        if (character != other_characters[i] &&
            other_characters[i]->isKilled == 0 &&
            SDL_HasIntersection(&character->rect, &other_characters[i]->rect))
        {
            character->position = oldPosition;
            update_character_rect(character, &oldPosition);
            // play_sound_once(oi_sound); // <- diagnostic, remove
            return; // collision with another character, return
        }
    }

    // powerup tiles intersection
    for (int i = 0; i < powerUpCount; i++)
    {
        if (powerUps[i].active && SDL_HasIntersection(&character->rect, &powerUps[i].rect))
        {
            if (powerUps[i].type == POWERUP_SKULL && character->isHunter == 0)
            {
                continue; // dont apply POWRUP_SKULL and DONT deactivate it if intersected by non-hunter
            }
            apply_powerUp(character, powerUps[i].type, other_characters, num_other_characters, mazeView);
            powerUps[i].active = 0;
            character->lastPowerupCollected = powerUps[i].powerupid;
        }
    }
}

// TODO: Fix so that we just send in one character and checks if any of the other characters are in range to kill
void kill_command(Character *hunter, Character **characters, int num_characters)
{
    if (hunter == NULL)
        return;
    if (!hunter->isHunter)
        return;

    const int killDistance = 80; // This represents the maximum distance in pixels

    for (int i = 0; i < num_characters; i++)
    {
        if (characters[i] == NULL)
            continue;
        if (characters[i]->isHunter == 0 && !characters[i]->isKilled)
        {
            int y_distance = abs(hunter->rect.y - characters[i]->rect.y);
            int x_distance = abs(hunter->rect.x - characters[i]->rect.x);
            if (y_distance <= killDistance && x_distance <= killDistance)
            {
                characters[i]->isKilled = 1; // Change target to kille
                //cleanup_character(characters[i]);
                // Teleport to the target
                hunter->position.x = characters[i]->position.x;
                hunter->position.y = characters[i]->position.y;
                // kill sound
                play_sound_once(kill_sound, 5);
                // free_sse(kill_sound);

                break; // Kills one, otherwise more in a small space
            }
        }
    }
}

void draw_character(SDL_Renderer *pRenderer, Character *character, bool isMainCharacter, SDL_FPoint *camera)
{
    if (character->isKilled)
    {
        SDL_SetTextureColorMod(character->texture, 255, 128, 128);
    }
    else {
        set_direction(character, character->direction);
    }
    if (character->visible) {
        SDL_SetTextureAlphaMod(character->texture, 255);
    }
    else if (isMainCharacter && !character->visible) {
        SDL_SetTextureAlphaMod(character->texture, 128);
    }
    else if (!character->visible)
    {
        return;
    }
    set_direction(character, character->direction);
    
    // Needs to be same for all
    // 128 x 192
    const int frameWidth = 32;  // Sprite sheet width / frames per column
    const int frameHeight = 48; // Sprite sheet height / frames per row
    const int frameCount = 4;   // Each frame in each row
    Uint32 timeSinceLastFrame = SDL_GetTicks() - character->frameLastUpdated;

    if (character->isMoving && timeSinceLastFrame > (700 / frameCount))
    {
        character->currentFrame = (character->currentFrame + 1) % frameCount;
        character->frameLastUpdated = SDL_GetTicks();
    }

    SDL_Rect srcRect;
    srcRect.w = frameWidth;
    srcRect.h = frameHeight;
    srcRect.x = character->currentFrame * frameWidth;

    switch (character->direction)
    {
    case 'd':
        srcRect.y = 0 * frameHeight;
        break;
    case 'l':
        srcRect.y = 1 * frameHeight;
        break;
    case 'r':
        srcRect.y = 2 * frameHeight;
        break;
    case 'u':
        srcRect.y = 3 * frameHeight;
        break;
    default:
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
    dot.x += (window_width - mazeView->viewRect.w) / 2 - 10;
    dot.y += (window_height - mazeView->viewRect.h) / 2;

    SDL_RenderFillRect(renderer, &dot); // render 
}

void cleanup_character(Character *character)
{
    if (character != NULL)
    {
        if (character->texture != NULL)
        {
            SDL_DestroyTexture(character->texture);
            character->texture = NULL;
        }
    }
    free(character);
}

void follow_player(SDL_FPoint *camera, SDL_Rect *player, int WINDOW_WIDTH, int WINDOW_HEIGHT)
{
    camera->x = player->x - WINDOW_WIDTH / 2 + player->w / 2;
    camera->y = player->y - WINDOW_HEIGHT / 2 + player->h / 2;
}

void update_character_rect(Character *character, SDL_FPoint *position)
{
    character->rect.x = (int)position->x;
    character->rect.y = (int)position->y;
}

SDL_FPoint get_character_center(Character *character)
{
    return (SDL_FPoint){character->position.x + character->rect.w / 2, character->position.y + character->rect.h / 2};
}