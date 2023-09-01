#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "playback.h"

/* holds wav data */
typedef struct {
	uint32_t file_length;
	uint8_t *buf;
	double wav_secs;
	SDL_AudioSpec spec;
	SDL_AudioDeviceID device_id;
	Mix_Music *music;
}AUD;

AUD *aud;

void pause_audio (void)
{
	//Mix_PlayChannel(-1, wav->sample[0], 1);
}

void unpause_audio (void)
{
	//Mix_PlayChannel(-1, wav->sample[0], 0);
}

/* calculates remaing seconds from a minute double value */
int get_remaining_secs (double total_secs)
{
	double remaining_secs;
	double whole_mins;

	whole_mins = (double) total_secs / 60;
	
	remaining_secs = (double) whole_mins - (int) whole_mins;

	return (int) remaining_secs * 60;
}

/* returns remaing minutes as int */
int get_mins (double total_secs)
{
	double mins;

	mins = (double) total_secs / 60;

	return (int) mins;
}

/* calculates length in seconds of a given wav file */
double get_time_wav (const char *file_name)
{
	double time;
	uint8_t *buf;
	uint32_t wav_length;
	uint32_t sample_length;
	uint32_t sample_size;
	uint32_t sample_count;
	SDL_AudioSpec spec;

	/* loads file and calculates the length in seconds */
	if (SDL_LoadWAV(file_name, &spec, &buf, &wav_length) != NULL) {
		sample_length = 0;

		SDL_FreeWAV(buf);

		sample_size = SDL_AUDIO_BITSIZE(spec.format) / 8;
		sample_count = wav_length / sample_size;

		if (spec.channels)
			sample_length = wav_length / spec.channels;
		else
			sample_length = sample_count;

		time = (double) sample_length / (double) spec.freq;
	} else return 0.0;

	return time;
}

/* setups up SDL for playback */
static int init (const char *file_name)
{
	/* init SDL */
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "ERROR: Failed to init SDL\n");
		return 1;
	}

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512) < 0) {
		fprintf(stderr, "ERROR: Unable to open audio\n%s\n", SDL_GetError());
		return 1;
	}

	if (Mix_AllocateChannels(4) < 0) {
		fprintf(stderr, "ERROR: Unable to allocate mixing channel\n%s\n", SDL_GetError());
		return 1;
	}

	aud->music = Mix_LoadMUS(file_name);
	Mix_PlayMusic(aud->music, 1);
	aud->music = NULL;
	return 0;
}

/* cleanups SDL */
void cleanup (void)
{
	aud->music = NULL;
	Mix_CloseAudio();
	SDL_Quit();
}

/* starts playing file */
int playback_entry (const char *file_name)
{
	aud = (AUD*)malloc(sizeof(*aud));

	if (init(file_name) != 0)
		return 1;
	
	free(aud);
	return 0;
}
