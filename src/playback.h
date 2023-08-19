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
};

struct KEYS keys[] = {
	{ 'p', pauseAudio },
	{ 'u', unpauseAudio },
};

#include "playback.c"

#endif // PLAYBACK_H
