#pragma once

#include <stdbool.h>

#define DEFAULT_RATE 44100
#define DEFAULT_CHANNELS 2
#define MILLION 1000000
#define PCM_DEVICE "default"

typedef struct {
	unsigned int pcm;
	unsigned int tmp;
	unsigned int rate;
	int buf_size;
	int loops;
	int seconds;
	int channels;
	char *buf;
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *pcm_params;
	snd_pcm_uframes_t pcm_frames;
	bool rf;
	bool cf;
	bool sf;
}WAV;
