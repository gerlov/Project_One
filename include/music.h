#ifndef MUSIC_H
#define MUSIC_H

/// @brief Initializes SDL_mixer and starts playing background music.
/// @param None
/// @return None
void init_music(void);

/// @brief Load and play a sound effect once. Suitable for short audio effects.
/// @param None
/// @return None
void play_sound_once(void);

/// @brief Toggles the playback of the background music between playing and paused state.
/// @param None
/// @return None
void toggle_music(void);

/// @brief Increases the volume of the background music.
/// @param None
/// @return None
void increase_volume(void);

/// @brief Decreases the volume of the background music.
/// @param None
/// @return None
void decrease_volume(void);

#endif /* MUSIC_H */
