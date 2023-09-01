#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

/* pauses playback */
void pause_audio (void);

/* unpauses playback */
void unpause_audio (void);

/* calculates remaing seconds from a minute double value */
int get_remaining_secs (double total_secs);

/* returns remaing minutes as int */
int get_mins (double total_secs);

/* gets the time in seconds of a wav file */
double get_time_wav (const char *file_name);

/* cleans up SDL */
void cleanup (void);

/* starts playback of a wav file */
int playback_entry (const char *file_name);

#include "playback.c"

#endif // PLAYBACK_H
