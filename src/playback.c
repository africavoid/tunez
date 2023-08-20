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

/* the interface for controlling via terminal */
static void controls (char *fn)
{
	unsigned int ch;
	signed int fd;
	int time = 1000;
	bool pause = false;

	if (initscr() == NULL)
	{
		printe("Couldn't init curses");
		return;
	}

	noecho();
	curs_set(0);
	nodelay(stdscr, true);

	mvprintw(0, 0, "Now playing %s", fn);
	refresh();

	for (int i = 0; i < time; i++)
	{
		if ((ch = getch()) == 'q') break;
		else if ((fd = keycheck(ch)) != -1) keys[fd].fn();

		SDL_Delay(1000);

		pause = keys[fd].pause;

		if (pause == false)
		{
			mvprintw(1, 0, "%d Seconds Elapsed\\%d Total\n", i, time);
			refresh();
		}
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
