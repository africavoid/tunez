#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <curses.h>

#include "playback.h"
#include "parse.h"
#include "error.h"
#include "audio.h"

/* calculates length of wav file */
static double calc_time (char *fn)
{
	SDL_AudioSpec spec;
	double time = 0.0;
	uint8_t *audio_buf;
	uint32_t audio_len;
	uint32_t sample_len = 0;
	uint32_t sample_size;
	uint32_t sample_count;

	/* loads wav file and does a calculation based on the file */
	if (SDL_LoadWAV(fn, &spec, &audio_buf, &audio_len) != NULL)
	{
		SDL_FreeWAV(audio_buf);

		sample_size = SDL_AUDIO_BITSIZE(spec.format) / 8;
		sample_count = audio_len / sample_size;

		if (spec.channels)
			sample_len = sample_count / spec.channels;
		else
			sample_len = sample_count;

		time = (double) sample_len / (double) spec.freq;
	}

	return time;
}

/* converts seconds to minutes
 * only if seconds is longer than 60 */
static double convert_to_min (double secs)
{
	return (secs > 60.00) ? (secs / 60.00) : secs;
}

/* checks if key pressed is a valid key
 * if it is valid it returns the index 
 * to the valid key */
static signed int keycheck (int ch)
{
	for (int i = 0; keys[i].ch != (char) 0; i++)
		if (keys[i].ch == ch) return i;

	return -1;
}

static void pause_loop (void)
{
	int ch;

	while ((ch = getch()) != 'u')
		if (ch == 'q') return;

	unpauseAudio();
}

static void count (double min, int sec, double min_time)
{
	if (sec < 61) 
		mvprintw(1, 0, "%d Seconds Elapsed \\ %.2f Total Time", sec, min_time);
	else if (sec > 61) 
		mvprintw(1, 0, "%d.%d Minutes Elapsed \\ %.2f Total Time", min, sec, min_time);

	refresh();
}

/* the interface for controlling via terminal */
static void controls (char *fn)
{
	unsigned int ch;
	signed int fd;
	double raw_time = 0.0, min_time = 0.0, i = 0.0;

	if (initscr() == NULL)
	{
		printe("Couldn't init curses");
		return;
	}

	/* get length of file in seconds */
	raw_time = calc_time(fn);
	min_time = convert_to_min(raw_time);
	
	/* general setup for curses */
	noecho();
	curs_set(0);
	nodelay(stdscr, true);

	/* print file name on screen */
	mvprintw(0, 0, "Now playing %s", fn);
	refresh();

	/* main loop */
	for (int k = 0; i < raw_time; i++, k++)
	{
		if ((ch = getch()) == 'q') break;
		else if ((fd = keycheck(ch)) != -1)
			keys[fd].fn();

		/* 1000 is 1 second */
		SDL_Delay(1000);

		if (ch == 'p') pause_loop();

		count(i, k, min_time);
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
