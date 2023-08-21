#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "mp3.h"
#include "error.h"

static void mp3_controls (void)
{
	while (!SDL_QuitRequested())
		SDL_Delay(1000);
}

void mp3_playback_entry (const char *fn)
{
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

	mp3_controls();
	Mix_FreeMusic(music);
	SDL_Quit();
}
