#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <curses.h>

#include "controller.h"
#include "error.h"

static double wav_calc_time (const char *fn);

/* curses screen */
typedef struct {
	WINDOW *win;
	size_t max_y, max_x;
}SCR;

/* audio data */
typedef struct {
	Uint32 wav_len;
	Uint8 *wav_buf;
	SDL_AudioSpec spec;
	SDL_AudioDeviceID device_id;
}AUD;

SCR *scr;
AUD *aud;

/* gets total seconds */
static int get_secs (double total_secs) 
{
	double total_raw_mins;
	int total_whole_mins;

	double remaining_raw_secs;
	int remaining_secs;

	total_raw_mins = total_secs / 60;
	total_whole_mins = (int) total_raw_mins;

	remaining_raw_secs = (double) total_raw_mins - (double) total_whole_mins;
	remaining_secs = remaining_raw_secs * 60;

	return remaining_secs;
}

/* gets total mins as whole number */
static int get_mins (double total_secs)
{
	double total_raw_mins;
	int total_whole_mins;

	total_raw_mins = total_secs / 60;
	total_whole_mins = (int) total_raw_mins;

	return total_whole_mins;
}

static void scr_count (int sec, double raw_time)
{
	if (sec <= 60)
		mvprintw(1, 0, "%d Seconds Elapsed / %d.%d Minutes", sec, get_mins(raw_time), get_secs(raw_time));
	else if (sec >= 60)
		mvprintw(1, 0, "%d.%d Minutes Elapsed / %d.%d Minutes", get_mins(sec), get_secs(sec), get_mins(raw_time), get_secs(raw_time));
	refresh();
}

static int ctrl_init_curses (const char *fn)
{
	if (initscr() == NULL) return 1;

	scr = malloc(sizeof(*scr));
	if (scr == NULL) return 1;

	/* create a basic curses window */
	getmaxyx(stdscr, scr->max_y, scr->max_x);
	scr->win = newwin(scr->max_y, scr->max_x, 0, 0);
	refresh();

	/* some basic settings */ 
	noecho();
	curs_set(0);
	nodelay(scr->win, true);

	/* some decor */
	mvwprintw(scr->win, 0, 0, "Playing %s", fn);
	wrefresh(scr->win);
	return 0;
}

static void ctrl_free_curses (void)
{
	free(scr);
	endwin();
}

void unpause (void)
{
	SDL_PauseAudioDevice(aud->device_id, 0);
}

void pause (void)
{
	SDL_PauseAudioDevice(aud->device_id, 1);
}

/* will see if ch is the same as a valid key
 * if it is it will run the function corresponding to key */
static void parse_keys (int ch)
{
	for (size_t i = 0; keymap[i].ch != (char) 0; i++)
		if (keymap[i].ch == ch) keymap[i].fn();
}

/* the main loop for controlling program */
void control_loop (const char *fn) 
{
	int ch;
	double time = wav_calc_time(fn);

	if (ctrl_init_curses(fn) != 0)
	{
		printe("failed to init curses");
		return;
	}

	for (size_t i = 0; i < time; i++)
	{
		if ((ch = wgetch(scr->win)) == 'q') break;

		parse_keys(ch);
		SDL_Delay(SECOND);
		scr_count((int) i, time);
	}

	ctrl_free_curses();
}

/* calculates length of wav file */
static double wav_calc_time (const char *fn)
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

/* called by wav_playback_entry to setup wav */
static int wav_init (const char *fn)
{
	/* load the file */
	SDL_Init(SDL_INIT_AUDIO);
	SDL_LoadWAV(fn, &aud->spec, &aud->wav_buf, &aud->wav_len);

	/* open audio device */
	aud->device_id = SDL_OpenAudioDevice(NULL, 0, &aud->spec, NULL, 0);
	return 0;
}

static void wav_cleanup (void)
{
	SDL_CloseAudioDevice(aud->device_id);
	SDL_FreeWAV(aud->wav_buf);
	SDL_Quit();
	free(aud);
}

/* gets ready for playback wav file */
void wav_playback_entry (const char *fn)
{
	aud = malloc(sizeof(*aud));
	if (aud == NULL)
	{
		printe("malloc (aud) wav_playback_entry");
		return;
	}

	if (wav_init(fn) != 0)
	{
		printe("failed to init SDL (wav_init)");
		return;
	}

	/* playback */
	if (SDL_QueueAudio(aud->device_id, aud->wav_buf, aud->wav_len) < 0)
	{
		printe("failed to read wav (wav_playback_entry)");
		return;
	}
	
	SDL_PauseAudioDevice(aud->device_id, 0);
	control_loop(fn);
	wav_cleanup();
}

/* gets ready for playback mp3 file */
void mp3_playback_entry (const char *fn)
{
	aud = malloc(sizeof(*aud));
	if (aud == NULL)
	{
		printe("malloc (aud) mp3_playback_entry");
		return;
	}

	/* init SDL for playback */
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		printe("failed to init SDL (mp3_playback_entry)");
		return;
	}

	/* init SDL mixer */
	if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3)
	{
		printe("couldn't init mixer (mp3_playback_entry)");
		return;
	}

	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
	Mix_Music *music = Mix_LoadMUS(fn);
	Mix_PlayMusic(music, 1);

	control_loop(fn);
	Mix_FreeMusic(music);
	SDL_Quit();
	free(aud);
}
