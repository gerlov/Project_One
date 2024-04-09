#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "music.h"

bool musicPaused = false;

/******************************************************
 * Function: init_music                              *
 * Description: Initializes SDL_mixer and starts     *
 *              playing background music.            *
 * Parameters: None                                  *
 * Returns: None                                     *
 ******************************************************/

void init_music(void){
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }

    Mix_Music* backgroundMusic = Mix_LoadMUS("resources/music/pop.mp3");;

    if (!backgroundMusic) {
        printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }
    Mix_PlayMusic(backgroundMusic, -1);
}


/******************************************************
 * Function: play_sound_once                         *
 * Description: Load and play a sound effect once.   *
 *              Suitable for short audio effects.    *
 * Parameters: None                                  *
 * Returns: None                                     *
 ******************************************************/
void play_sound_once(void) {
    Mix_Chunk* sound = Mix_LoadWAV("resources/music/gun.mp3");
    if (!sound) {
        printf("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }
    Mix_PlayChannel(-1, sound, 0); // Play the sound once
}

/******************************************************
 * Function: toggle_music                            *
 * Description: Toggles the playback of the          *
 *              background music between playing     *
 *              and paused state.                    *
 * Parameters: None                                  *
 * Returns: None                                     *
 ******************************************************/
void toggle_music(void) {
    if (musicPaused) {
        Mix_ResumeMusic(); 
        musicPaused = false;
    } else {
        Mix_PauseMusic();
        musicPaused = true;
    }
}