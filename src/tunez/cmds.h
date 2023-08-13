#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <menu.h>
#include "ui.h"
#include "error.h"

void quit (SCR *scr, char *argv[]);

struct CMD_KW {
	const char *str;
	void (*fn) (SCR *scr, char *argv[]);
};

struct CMD_KW cmd_kw[] = {
	{ "q", quit },
};

#include "cmds.c"
