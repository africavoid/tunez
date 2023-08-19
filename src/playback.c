#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

#include "playback.h"
#include "parse.h"
#include "error.h"
#include "audio.h"

static signed int keycheck (int ch)
{
	for (int i = 0; keys[i].ch != (char) 0; i++)
		if (keys[i].ch == ch) return i;

	return -1;
}

/* the interface for controlling
 * via terminal */
static void controls (const char *fn)
{
	unsigned int ch;
	signed int fd;

	if (initscr() == NULL)
	{
		printe("Couldn't init curses");
		return;
	}

	noecho();
	curs_set(0);
	
	printw("Now playing %s", fn);
	refresh();

	while ((ch = getch()) != ENTER)  
	{
		if ((fd = keycheck(ch)) != -1) keys[fd].fn();
		SDL_Delay(10);
	}

	echo();
	curs_set(1);
	endwin();
}

/* sets up the audio
 * see audio.h or audio.c 
 * for more details */
void playback_entry (char *arg)
{
	if (arg == NULL)
	{
		printe("No arg supplied\nsee -h");
		return;
	}

	SDL_Init(SDL_INIT_AUDIO);
	initAudio();

	playMusic(arg, SDL_MIX_MAXVOLUME);
	controls(arg);

	endAudio();
	SDL_Quit();
}
