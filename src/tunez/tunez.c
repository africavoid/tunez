/*
	This is the main file for the tunez project
	tunez is a curses frontend for playing music
	it relies on wavplay to play wavfiles
	TODO:
	- fix the type_check to check for dirs
	- create a nice interface
	- make a way of playing the music through wavplay
	- make a way of skipping pausing etc
	- config file
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <curses.h>
#include <menu.h>

#include "tunez.h"

static void global_cleanup(SCR *scr);

void create_menu (SCR *scr)
{
	scr->n_items = ARRAY_SIZE(scr->fns);
	scr->items = (ITEM **) calloc(scr->n_items + 1, sizeof(ITEM *));
	
	/* assigned fns (file names) to on screen items */
	for (int i = 0; i < scr->n_items; i++)
		scr->items[i] = new_item(scr->fns[i], TENWSP);

	scr->items[scr->n_items] = (ITEM *) NULL;
	
	/* creating the menu */
	scr->men = new_menu((ITEM **) scr->items);

	/* creating menu window */
	set_menu_win(scr->men, scr->win);
	set_menu_sub(scr->men, derwin(scr->win, 0, 0, 1, 1));

	/* set the indicator */
	set_menu_mark(scr->men, MARK);
	
	/* decorations */
	box(scr->win, 0, 0);

	/* posting to screen */	
	post_menu(scr->men);
	wrefresh(scr->win);
}

void mov_down (SCR *scr)
{
	if (scr->cur_y < scr->fns_max)
		scr->cur_y++;
	menu_driver(scr->men, REQ_DOWN_ITEM);
}

void mov_up (SCR *scr)
{
	if (scr->cur_y > 0)
		scr->cur_y--;
	menu_driver(scr->men, REQ_UP_ITEM);
}

void goto_top (SCR *scr)
{
	scr->cur_y = 0;
	menu_driver(scr->men, REQ_FIRST_ITEM);
}

void goto_bot (SCR *scr)
{
	scr->cur_y = scr->fns_max;
	menu_driver(scr->men, REQ_LAST_ITEM);
}

void sel (SCR *scr)
{
	size_t i = 0;

	menu_driver(scr->men, REQ_TOGGLE_ITEM);
	scr->cur_item = current_item(scr->men);

	while (scr->fns_type[scr->fns_max] != formats[i].ft)
	{
		if (formats[i+1].ft != (int) 0)
			i++;
		else
			break;
	}
}

/* the main loop */
static void input_loop (SCR *scr)
{
	int ch, i;

	while (true)
	{
		i = 0;	
		scr->cur_y = 0;
		ch = wgetch(scr->win);

		for (; key_map[i].ch != ch; i++)
		{
			if (key_map[i].ch == (char) 0)
				break;
		}
	
		if (key_map[i].ch == ch)
			key_map[i].fn(scr);
	}
}


unsigned int type_check (const char *n)
{
	size_t len, j = 0;
	bool match = false;
	char *tmp;
	
	tmp = malloc(KB);
	if (tmp == NULL)
	{
		fprintf(stderr, "ERROR: malloc (tmp)\n");
		return -1;
	}

	len = strlen(n);

	for (size_t i = len; n[i] != '.'; i--)
		strcat(tmp, &n[i]);


	while (formats[j].ff != NULL)
	{
		if (tmp == formats[j].ff)
		{
			match = true;
			break;
		}

		if (formats[j+1].ff != NULL)
			j++;
		else
			break;
	}
	
	free(tmp); 
	
	if (match == true)
		return formats[j].ft;

	return -1;
}

int scan_dir (SCR *scr)
{
	DIR *dp;
	struct dirent *entry;
	
	dp = opendir(DEFAULT_DIR);
	
	if (dp == NULL)
	{
		fprintf(stderr, "ERROR: couldn't open dir\n");
		return 1;
	}

	/* gets all files and dirs in dir */	
	for (size_t i = 0; (entry = readdir(dp)) != NULL; i++)
	{
		scr->fns[i] = strdup(entry->d_name);
		scr->fns_max = i;
		scr->fns_type[i] = type_check(scr->fns[i]);
	}

	closedir(dp);
	return 0;
}

/* setup for the tunez ui */
static int global_setup (const char **argv)
{
	SCR *scr;

	if ((scr = malloc(sizeof(*scr))) == NULL)
	{
		fprintf(stderr, "ERROR: malloc (scr)\n");
		return 1;
	}

	/* starting curses */	
	if (initscr() == NULL)
	{
		fprintf(stderr, "ERROR: couldn't start curses\n");
		return 1;
	}

	/* getting terminal dimensions */	
	getmaxyx(stdscr, scr->max_y, scr->max_x);
	
	/* some setup stuff */
	noecho();
	cbreak();
	curs_set(0);
	
	/* creating window */
	scr->win = newwin(scr->max_y, scr->max_x, START_Y, START_X);
	refresh();
	
	if (scan_dir(scr) != 0)
	{
		fprintf(stderr, "ERROR: couldn't open direcotory\n");
		return 1;
	}

	create_menu(scr);
	input_loop(scr);
	global_cleanup(scr);
	return 0;	
}

static void global_cleanup (SCR *scr)
{
	for (int i = 0; scr->items[i] != NULL; i++)
		free_item(scr->items[i]);

	unpost_menu(scr->men);	
	free_menu(scr->men);
	endwin();
	free(scr);	
}

int main (int argc, const char **argv)
{
	if (global_setup(argv) != 0)
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}
