#ifndef KEYS_H
#define KEYS_H

#include <stdio.h>
#include <stdlib.h>

#include "interface.h"

struct KEYMAP {
	const int ch;
	void (*fn)(void);
};

struct ALTKEYMAP {
	const int ch;
	void (*fn)(size_t foo);
};

struct KEYMAP keymap[] = {
	{ 'p', pause },
	{ 'u', unpause },
	{ ':', cmd_line },
	{ 'q', quit },
};

struct ALTKEYMAP altkeymap[] = {
	{ 'h', audio_position },
};

#include "keys.c"

#endif // KEYS_H
