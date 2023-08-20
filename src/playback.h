#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

#include "parse.h"
#include "error.h"
#include "audio.h"

#define ENTER 10

void playback_entry(char *arg);

struct KEYS {
	const char ch;
	void (*fn)(void);
	bool pause;
};

struct KEYS keys[] = {
	{ 'p', pauseAudio, true },
	{ 'u', unpauseAudio, false },
};

#include "playback.c"

#endif // PLAYBACK_H
