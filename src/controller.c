#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <curses.h>

#include "interface.h"
#include "controller.h"
#include "error.h"

static void wav_cleanup(void);
static void ctrl_free_curses (void);
static double wav_calc_time (const char *fn);

/* type of file, 0 is wav, 1 is mp3 */
uint8_t type = 0;

typedef struct {
	Mix_Music *music;
}MP3;

/* audio data */
typedef struct {
	uint32_t wav_len;
	uint8_t *wav_buf;
	SDL_AudioSpec wav_spec;
	SDL_AudioDeviceID device_id;
}AUD;

AUD *aud;
MP3 *mp3;

void quit (void)
{
	ctrl_free_curses();

	if (type == 0) wav_cleanup();

	if (aud != NULL)
		free(aud);

	if (mp3 != NULL)
		free(mp3);

	exit(0);
}

/* gets total seconds */
int get_secs (double total_secs) 
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
int get_mins (double total_secs)
{
	double total_raw_mins;
	int total_whole_mins;

	total_raw_mins = total_secs / 60;
	total_whole_mins = (int) total_raw_mins;

	return total_whole_mins;
}

/* unpauses audio depending on type */
void unpause (void)
{
	if (type == 0)
		SDL_PauseAudioDevice(aud->device_id, 0);
	else if (type == 1)
		Mix_ResumeMusic();
}

/* pauses audio depending on flag */
void pause (void)
{
	int ch;

	if (type == 0)
		SDL_PauseAudioDevice(aud->device_id, 1);
	else if (type == 1)
		Mix_PauseMusic();

	pause_control();

	
	play_settings();
	unpause();
}

void audio_position (size_t pos)
{
	Mix_SetMusicPosition(pos);
}

/* calculates length of wav file */
static double wav_calc_time (const char *fn)
{
	double time = 0.0;
	uint8_t *audio_buf;
	uint32_t audio_len;
	uint32_t sample_len = 0;
	uint32_t sample_size;
	uint32_t sample_count;
	SDL_AudioSpec spec;

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

	if (SDL_LoadWAV(fn, &aud->wav_spec, &aud->wav_buf, &aud->wav_len) == NULL)
	{
		printe("cannot load file");
		fprintf(stderr, "%s\n", SDL_GetError());
		return 1;
	}

	/* open audio device */
	aud->device_id = SDL_OpenAudioDevice(NULL, 0, &aud->wav_spec, NULL, 0);
	return 0;
}

/* cleanup function for wav files */
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

	type = 0;

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
	control_loop(fn, wav_calc_time(fn));
	wav_cleanup();
}

/* gets ready for playback mp3 file */
void mp3_playback_entry (const char *fn)
{
	mp3 = malloc(sizeof(*mp3));

	if (mp3 == NULL)
	{
		printe("malloc (mp3) mp3_playback_entry");
		return;
	}

	type = 1;

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
	mp3->music = Mix_LoadMUS(fn);
	Mix_PlayMusic(mp3->music, 1);

	control_loop(fn, Mix_MusicDuration(mp3->music));
	Mix_FreeMusic(mp3->music);
	SDL_Quit();
	free(mp3);
}
