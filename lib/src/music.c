#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "music.h"

bool musicPaused = false;
int volume = 0; 

BackgroundMusic* init_background_music(char *soundPath, int start_volume) {

    BackgroundMusic *bgm = (BackgroundMusic *) malloc(sizeof(BackgroundMusic));
    if (bgm == NULL) {
        fprintf(stderr, "Failed to allocate memory for background music.\n");
        return NULL;
    }

    if (start_volume < 0) {
        bgm->volume = 0;
    } else if (start_volume > 100) {
        bgm->volume = 100;
    } else {
        bgm->volume = start_volume;
    }
    volume = start_volume;
    bgm->soundPath = soundPath;
    bgm->paused = false;
    Mix_VolumeMusic(start_volume);

    return bgm;
}

Single_sound* init_sound_effect(char *soundPath) {
    Single_sound *sse = malloc(sizeof(Single_sound));
    if (sse == NULL) {
        fprintf(stderr, "Failed to allocate memory for sound effect.\n");
        return NULL;
    }
    sse->soundPath = soundPath;
    sse->volume = volume;
    return sse;
}

void free_bgm(BackgroundMusic *bgm){
    if(bgm != NULL){free(bgm);}
}

void free_sse(Single_sound *sse){
    if(sse != NULL){ free(sse);}
}



void init_sounds(){
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }
}

void play_background_music(BackgroundMusic *bgm){
    if (!bgm || !bgm->soundPath) {
    printf("Background music or path is not initialized properly.\n");
    return;
}
    Mix_Music* backgroundMusic = Mix_LoadMUS(bgm->soundPath);

    if (!backgroundMusic) {
        printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }
    bgm->paused = false;
    Mix_PlayMusic(backgroundMusic, -1);
}

void play_sound_once(Single_sound *sse, int ss_volume) {
    Mix_Chunk* sound = Mix_LoadWAV(sse->soundPath);
    if (!sound) {
        printf("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }
    if(ss_volume < 0) { ss_volume = 0; }
    if(ss_volume > MIX_MAX_VOLUME) { ss_volume = MIX_MAX_VOLUME; }
    // Mix_VolumeMusic(ss_volume);
    Mix_VolumeChunk(sound, ss_volume);
    Mix_PlayChannel(-1, sound, 0); // Play the sound once
    // Mix_VolumeMusic(volume);
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

void set_volume(int new_volume){
    if(new_volume < 0){
        volume = 0;
    }
    if(new_volume > MIX_MAX_VOLUME){
        volume = MIX_MAX_VOLUME;
    }
    volume = new_volume;
    Mix_VolumeMusic(volume);
}