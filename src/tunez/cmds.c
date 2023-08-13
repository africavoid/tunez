#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include "ui.h"
#include "error.h"
#include "cmds.h"

void quit (SCR *scr, char *argv[])
{
	ui_clean(scr);
	exit((argv[0] == NULL) ? 0 : atoi(argv[0]));
}
