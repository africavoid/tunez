#include <curses.h>
#include <menu.h>
#include "ui.h"
#include "audio.h"

int playback_entry (const char *fn, int sig)
{
	SDL_Init(SDL_INIT_AUDIO);
	initAudio();
	
	playMusic(fn, SDL_MIX_MAXVOLUME);
	SDL_Delay(10000);
	endAudio();
	SDL_Quit();
	return 0;
}
