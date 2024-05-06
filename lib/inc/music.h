#ifndef MUSIC_H
#define MUSIC_H

#include <stdbool.h>

/// @brief Structure to represent a single sound effect.
/// @param soundPath Path to the sound file.
/// @param volume Volume level for the sound effect, ranging from 0-100.
typedef struct Single_sound{
    char *soundPath;
    int volume;
} Single_sound;

/// @brief Structure to represent background music.
/// @param soundPath Path to the music file.
/// @param volume Volume level for the music, ranging from 0-100.
/// @param paused Initial pause state of the music.
typedef struct BackgroundMusic {
    char *soundPath;
    int volume;
    bool paused;
} BackgroundMusic;


/// @brief Sets a direct volume.
/// @param new_volume takes an int to set a specific volume. If outside of bounds, sets inside.
void set_volume(int new_volume);

/// @brief Initializes background music with specified parameters.
/// @param soundPath Path to the music file.
/// @param volume Initial volume setting.
BackgroundMusic* init_background_music(char *soundPath, int start_volume);


/// @brief Initializes a sound effect with specified parameters.
/// @param soundPath Path to the sound file.
Single_sound* init_sound_effect(char *soundPath);

/// @brief Frees a Single_sound structure and any associated resources.
/// @param sse Pointer to the Single_sound structure to be freed.
void free_sse(Single_sound *sse);

/// @brief Frees a BackgroundMusic structure and any associated resources.
/// @param bgm Pointer to the BackgroundMusic structure to be freed.
void free_bgm(BackgroundMusic *bgm);

/// @brief Initializes the SDL_mixer, setting up audio for playback.
void init_sounds();

/// @brief Plays the loaded background music continuously.
/// @param bgm Pointer to the BackgroundMusic structure containing the loaded music.
void play_background_music(BackgroundMusic *bgm);

/// @brief Plays a sound effect once.
/// @param sound Pointer to the Single_sound structure containing the sound effect.
<<<<<<< HEAD
/// @param volume Sets that sound volume.
=======
>>>>>>> 017391d (Lowered the volume and fix SSeffects)
void play_sound_once(Single_sound *sound, int volume);

/// @brief Toggles the playback state of the background music between paused and playing.
void toggle_music(void);

/// @brief Increases the volume of the background music.
void increase_volume(void);

/// @brief Decreases the volume of the background music.
void decrease_volume(void);

#endif /* MUSIC_H */
