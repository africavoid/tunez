#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <curses.h>

#include "error.h"

#define SECOND 1000
void pause(void);
void unpause(void);
void wav_playback_entry (const char *fn);
void mp3_playback_entry (const char *fn);
void control_loop(const char *fn);


struct KEYMAP {
	const int ch;
	void (*fn)(void);
};

struct KEYMAP keymap[] = {
	{ 'p', pause },
	{ 'u', unpause },
};

#include "controller.c"

#endif // CONTROLLER_H
