#ifndef MP3_H
#define MP3_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

/* the entry point for mp3 playback */
void mp3_playback_entry(const char *fn);

#include "mp3.c"
#endif // MP3_H
