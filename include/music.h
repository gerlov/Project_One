#ifndef MUSIC_H
#define MUSIC_H

/******************************************************
 * Function: init_music                              *
 * Description: Initializes SDL_mixer and starts     *
 *              playing background music.            *
 * Parameters: None                                  *
 * Returns: None                                     *
 ******************************************************/
void init_music(void);

/******************************************************
 * Function: play_sound_once                         *
 * Description: Load and play a sound effect once.   *
 *              Suitable for short audio effects.    *
 * Parameters: None                                  *
 * Returns: None                                     *
 ******************************************************/
void play_sound_once(void);

/******************************************************
 * Function: toggle_music                            *
 * Description: Toggles the playback of the          *
 *              background music between playing     *
 *              and paused state.                    *
 * Parameters: None                                  *
 * Returns: None                                     *
 ******************************************************/
void toggle_music(void);

#endif
