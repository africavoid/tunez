#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <curses.h>

#include "error.h"

#define SECOND 1000
#define SECSTR "%d Seconds Elapsed / %d Mins %d Secs Total"
#define MINSTR "%d Mins %d Secs Elapsed / %d Mins %d Secs Total"

void cmd_line(void);
void pause(void);
void unpause(void);
void wav_playback_entry (const char *fn);
void mp3_playback_entry (const char *fn);
void control_loop(const char *fn, double time);

struct KEYMAP {
	const int ch;
	void (*fn)(void);
};

struct KEYMAP keymap[] = {
	{ 'p', pause },
	{ 'u', unpause },
	{ ':', cmd_line },
};

#include "controller.c"

#endif // CONTROLLER_H
