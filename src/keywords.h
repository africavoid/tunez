#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmds.h"
#include "cli.h"
#include "error.h"
#include "playback.h"

void playback_entry(char *arg);

struct FLAG_MAP {
	const char *str;
	void (*fn)(char *arg);
};

struct FLAG_MAP flag_map[] = {
	{ "-c", cmd_line_entry },
	{ "-p", playback_entry },
};

struct KEYWORDS {
	const char *str;
	void (*fn)(char *arg);
};

struct KEYWORDS kw[] = {
	{ "ls", list_files },
	{ "play", playback_entry },
};

signed int valid_kw(const char *str, size_t len);

#include "keywords.c"
#endif //KEYWORDS_H
