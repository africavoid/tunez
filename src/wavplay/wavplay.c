/*
	The wavplay is a program for playing wav files.
	This is effectivly the backend for tunez and should be compiled along side it.
	The wavplay binary can be used on it's own to play wav files.
	To use wavplay as a standalone program you must pipe the path on the cmd line, see:
	'wavplay < /home/will/music/beanieman_whoami.wav'
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>

#include "wavplay.h"

static void usage (void)
{
	fprintf(stdout, "USAGE:\n");
	fprintf(stdout, "wavplay < [path to wav]\n");
	fprintf(stdout, "wavplay -r [rate] -s [seconds] -c [channels] < [path to wav]\n");
	exit(0);
}

/*
	 these are the default settings
	 these are ignored if cmd line args
	 are presented
*/
void set_defaults (WAV *wav)
{
	if (wav->rf == false)
		wav->rate = DEFAULT_RATE;

	if (wav->cf == false) 
		wav->channels = DEFAULT_CHANNELS;
	
	if (wav->sf == false)
		wav->seconds = (int) MILLION;
}

/* invalid flags are ignored */
void get_flags (WAV *wav, const char **argv)
{
	for (int i = 0; argv[i] != NULL; i++)
	{
		if (argv[i][0] == '-' && argv[i][2] == (char) 0)
		{
			switch (argv[i][1])
			{
			case 'r':
				wav->rf = true;
				wav->rate = atoi(argv[i+1]);
				break;
			case 'c':
				wav->cf = true;
				wav->rate = atoi(argv[i+1]);
				break;
			case 's':
				wav->sf = true;
				wav->rate = atoi(argv[i+1]);
				break;
			case 'h':
				usage();
				break;
			default:
				fprintf(stderr, "WARNING: no such flag '%s' !\n", argv[i]);
				fprintf(stderr, "Playback will continue with default values\n");
				fprintf(stderr, "See -h for usage\n");
			}
		}
	} 
}

/* long function, but it sets up pcm in short */
int setup_pcm (WAV *wav)
{
	/* open pcm device for playback */
	if ((wav->pcm = snd_pcm_open(&wav->pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
	{
		fprintf(stderr, "ERROR: cannot open '%s' pcm device!\n%s\n", PCM_DEVICE, snd_strerror(wav->pcm));
		return 1;
	}
	
	/* setting defaults in pcm */
	snd_pcm_hw_params_alloca(&wav->pcm_params);
	snd_pcm_hw_params_any(wav->pcm_handle, wav->pcm_params);

	/* setting parameters */
	if ((wav->pcm = snd_pcm_hw_params_set_access(wav->pcm_handle, wav->pcm_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
		fprintf(stderr, "ERROR: cannot set interleaved!\n%s\n", snd_strerror(wav->pcm));
		return 1;
	}

	/* setting the format */	
	if ((wav->pcm = snd_pcm_hw_params_set_format(wav->pcm_handle, wav->pcm_params, SND_PCM_FORMAT_S16_LE)) < 0)
	{
		fprintf(stderr, "ERROR: cannot set format!\n%s\n", snd_strerror(wav->pcm));
		return 1;
	}

	/* setting up the channels */
	if ((wav->pcm = snd_pcm_hw_params_set_channels(wav->pcm_handle, wav->pcm_params, wav->channels)) < 0)
	{
		fprintf(stderr, "ERROR: cannot set channels!\n%s\n", snd_strerror(wav->pcm));
		return 1;
	}
	
	/* setting the rate */
	if ((wav->pcm = snd_pcm_hw_params_set_rate_near(wav->pcm_handle, wav->pcm_params, &wav->rate, 0)) < 0)
	{
		fprintf(stderr, "ERROR: cannot set rate!\n%s\n", snd_strerror(wav->pcm));
		return 1;
	}
	
	/* writing the parameters */	
	if ((wav->pcm = snd_pcm_hw_params(wav->pcm_handle, wav->pcm_params)) < 0)
	{
		fprintf(stderr, "ERROR: cannot write hardware parameters!\n%s\n", snd_strerror(wav->pcm));
		return 1;
	}

	/* allocate buf to hold for a single period */	
	snd_pcm_hw_params_get_period_size(wav->pcm_params, &wav->pcm_frames, 0);
	wav->buf_size = wav->pcm_frames * wav->channels * 2;
	wav->buf = (char *) malloc(wav->buf_size);

	/* get the playback time */
	snd_pcm_hw_params_get_period_time(wav->pcm_params, &wav->tmp, NULL);
	return 0;
}

/* plays the .wav file */
void playback_entry (WAV *wav)
{
	wav->loops = (wav->seconds * MILLION) / wav->tmp;

	for (; wav->loops > 0; wav->loops--)
	{
		/* if seconds > the length of .wav */
		if ((wav->pcm = read(0, wav->buf, wav->buf_size)) == 0)
			break;

		if ((wav->pcm = snd_pcm_writei(wav->pcm_handle, wav->buf, wav->pcm_frames)) == -EPIPE)
			snd_pcm_prepare(wav->pcm_handle);
		else if (wav->pcm < 0)
		{
			fprintf(stderr, "ERROR: cannot write to pcm device!\n%s\n", snd_strerror(wav->pcm));
			break;
		}
	}
}

int global_setup (const char **argv)
{
	WAV *wav;

	wav = malloc(sizeof(*wav));

	if (wav == NULL)
	{
		fprintf(stderr, "ERROR:	malloc (wav)\n");
		return 1;
	}

	/* cmd line flags if flase means no flag so use defs */
	wav->rf = false;
	wav->cf = false;
	wav->sf = false;
	
	get_flags(wav, argv);
	set_defaults(wav);

	if (setup_pcm(wav) != 0)
	{
		fprintf(stderr, "ERROR: pcm\n");
		free(wav);
		return 1;
	}
	
	playback_entry(wav);
	snd_pcm_drain(wav->pcm_handle);
	snd_pcm_close(wav->pcm_handle);
	free(wav->buf);
	free(wav);
	return 0;
}

int main (int argc, const char **argv)
{
	if (global_setup(argv) != 0)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
