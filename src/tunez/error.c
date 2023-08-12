#include <stdio.h>
#include <curses.h>
#include "ui.h"
#include "error.h"

void printce (SCR *scr, const char *errstr)
{
	mvwprintw(scr->win, scr->max_y - 2, 1, "%s", errstr);
	wrefresh(scr->win);
	wgetch(scr->win);
} 

int printe (int ec)
{
	fprintf(stderr, "ERROR: %s\n", em[0]);
	return EXIT_FAILURE;
}
