#include <stdlib.h>
#include <stdio.h>
#include <curses.h>

#include "interface.h"
#include "controller.h"
#include "keys.h"

SCR *scr;

/* the time counter on the screen */
static void scr_count (int sec, double raw_time)
{
	if (sec <= 60)
		mvwprintw(scr->win, 1, 0, SECSTR, sec, get_mins(raw_time), get_secs(raw_time));
	else if (sec >= 60)
	{
		for (int i = 0; i < strlen(SECSTR); i++)
			mvwprintw(scr->win, 1, 0, " ");
		mvwprintw(scr->win, 1, 0, MINSTR, get_mins(sec), get_secs(sec), get_mins(raw_time), get_secs(raw_time));
	}

	wrefresh(scr->win);
}

/* inits the cmd line */
void cmd_line (void)
{
	char *cmd = malloc(1024);
	nodelay(scr->win, false);
	pause();

	/* draws prompt to screen */
	mvwprintw(scr->win, scr->max_y - 1, 0, ":");
	wrefresh(scr->win);

	echo();
	curs_set(1);
	wgetnstr(scr->win, cmd, 1024);

	noecho();
	curs_set(0);
	unpause();
	nodelay(scr->win, true);
	free(cmd);
}

/* cleanup function for curses */
static void ctrl_free_curses (void)
{
	free(scr);
	endwin();
}

static void draw_decor (const char *fn)
{
	box(scr->win, 0, 0);
	mvwprintw(scr->win, 2, 2, "Playing %s", fn);
	wrefresh(scr->win);
}

/* inits curses interface */ 
int ctrl_init_curses (const char *fn)
{
	if (initscr() == NULL) return 1;

	scr = malloc(sizeof(*scr));

	if (scr == NULL)
	{
		printe("malloc (scr) ctrl_init_curses");
		return 1;
	}

	/* create a basic curses window */
	getmaxyx(stdscr, scr->max_y, scr->max_x);
	scr->win = newwin(scr->max_y, scr->max_x, 0, 0);
	refresh();

	/* some basic settings */ 
	noecho();
	curs_set(0);
	nodelay(scr->win, true);

	/* some decor */
	draw_decor(fn);
	return 0;
}

void pause_control (void)
{
	int ch;

	while ((ch = wgetch(scr->win)) != 'u')
		parse_keys(ch);
}

void play_settings (void)
{
	curs_set(0);
	noecho();
	nodelay(scr->win, true);
}

/* the main loop for controlling program */
void control_loop (const char *fn, double time) 
{
	int ch;

	if (ctrl_init_curses(fn) != 0)
	{
		printe("failed to init curses");
		return;
	}

	for (size_t i = 0; i < time; i++)
	{
		if ((ch = wgetch(scr->win)) == 's') break;
	
		if (ch == 'h') audio_position(i--);
		else if (ch == 'l') audio_position(i++);
		parse_keys(ch);
		scr_count((int) i, time);
		SDL_Delay(SECOND);
	}

	ctrl_free_curses();
}
