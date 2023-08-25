#include <stdlib.h>
#include <stdio.h>
#include <curses.h>

#include "interface.h"
#include "controller.h"
#include "keys.h"

SCR *scr;

static void scr_count (int sec, double raw_time);
void draw_status(int status);
void cmd_line (void);
static void ctrl_free_curses (void);
static void clear_screen (void);
static void redraw (void);
static void draw_decor (void);
int ctrl_init_curses (const char *fn);
void pause_control (void);
void play_settings (void);
void control_loop (const char *fn, double time);

/* the time counter on the screen */
static void scr_count (int sec, double raw_time)
{
	if (sec <= 60)
	{
		mvwprintw(scr->win, 1, 2, SECSTR, sec, get_mins(raw_time), get_secs(raw_time));
		draw_pause(1);
	}

	else if (sec >= 60)
	{
		for (int i = 0; i < strlen(SECSTR); i++)
			mvwprintw(scr->win, 1, 2, " ");
		mvwprintw(scr->win, 1, 2, MINSTR, get_mins(sec), get_secs(sec), get_mins(raw_time), get_secs(raw_time));
		draw_pause(1);
	}

	wrefresh(scr->win);
}

/* draws the paused indicator */
void draw_pause (int status)
{
	if (status == 0)
		mvwprintw(scr->win, 3, 2, "Paused (u)  >>");

	else if (status == 1)
		mvwprintw(scr->win, 3, 2, "Playing (p) ||");
}

/* inits the cmd line */
void cmd_line (void)
{
	char *cmd = (char*)malloc(1024);
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

/* fills the screen with whitespace */
static void clear_screen (void)
{
	for (size_t i = 0; i < scr->max_y; i++)
		for (size_t k = 0; k < scr->max_x; k++)
			mvwprintw(scr->win, i, k, " ");

	wrefresh(scr->win);
}

/* redraws the screen if it's resized */
static void redraw (void)
{
	scr->old_y = scr->max_y;
	scr->old_x = scr->max_x;

	getmaxyx(stdscr, scr->max_y, scr->max_x);

	if (scr->max_y != scr->old_y || scr->max_x != scr->old_x)
	{
		clear_screen();
		draw_decor();
	}
}

/* draws useful info to the screen */
static void draw_decor (void)
{
	box(scr->win, 0, 0);
	mvwprintw(scr->win, 2, 2, "Playing %s", scr->file_name);
	wrefresh(scr->win);
}

/* inits curses interface */ 
int ctrl_init_curses (const char *fn)
{
	if (initscr() == NULL) return 1;

	scr = (void*)malloc(sizeof(*scr));

	if (scr == NULL)
	{
		printe("malloc (scr) ctrl_init_curses");
		return 1;
	}

	/* setting file name for output */
	scr->file_name = strndup(fn, strlen(fn));

	/* create a basic curses window */
	getmaxyx(stdscr, scr->max_y, scr->max_x);
	scr->win = newwin(scr->max_y, scr->max_x, 0, 0);
	refresh();

	/* some basic settings */ 
	noecho();
	curs_set(0);
	nodelay(scr->win, true);

	/* some decor */
	draw_decor();
	return 0;
}

void pause_control (void)
{
	int ch;

	draw_pause(0);

	while ((ch = wgetch(scr->win)) != 'u')
		parse_keys(ch);

	draw_pause(1);
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

		/* display the timer on the screen */
		scr_count((int) i, time);

		/* redraw the screen if it's dimensions have changed */ 
		redraw();

		/* pause SDL for 1 second */
		SDL_Delay(SECOND);
	}

	ctrl_free_curses();
}
