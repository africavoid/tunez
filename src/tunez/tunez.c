/*
	This is the main file for the tunez project
	tunez is a curses frontend for playing music
	it relies on wavplay to play wavfiles
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
		scr->items[i] = new_item(scr->fns[i], "len");

	scr->items[scr->n_items] = (ITEM *) NULL;
	
	/* creating the menu */
	scr->men = new_menu((ITEM **) scr->items);
	post_menu(scr->men);
	refresh();
}

void mov_down (SCR *scr)
{
	menu_driver(scr->men, REQ_DOWN_ITEM);
}

void mov_up (SCR *scr)
{
	menu_driver(scr->men, REQ_UP_ITEM);
}

/* the main loop */
void input_loop (SCR *scr)
{
	int ch, i;

	while (true)
	{
		i = 0;	
		ch = getch();

		for (; key_map[i].ch != ch; i++)
		{
			if (key_map[i].ch == (char) 0)
				break;
		}
	
		if (key_map[i].ch == ch)
			key_map[i].fn(scr);
	}
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
