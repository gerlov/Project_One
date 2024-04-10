#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "music.h"

bool musicPaused = false;
static int volume = MIX_MAX_VOLUME / 2; 


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

void play_sound_once(void) {
    Mix_Chunk* sound = Mix_LoadWAV("resources/music/gun.mp3");
    if (!sound) {
        printf("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }
    Mix_PlayChannel(-1, sound, 0); // Play the sound once
}

void toggle_music(void) {
    if (musicPaused) {
        Mix_ResumeMusic(); 
        musicPaused = false;
    } else {
        Mix_PauseMusic();
        musicPaused = true;
    }
}

void increase_volume(void) {
    volume += 10; 
    if (volume > MIX_MAX_VOLUME) {
        volume = MIX_MAX_VOLUME; 
    }
    Mix_VolumeMusic(volume);
}

void decrease_volume(void) {
    volume -= 10; 
    if (volume < 0) {
        volume = 0; 
    }
    Mix_VolumeMusic(volume);
}