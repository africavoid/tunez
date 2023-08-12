#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <curses.h>
#include <menu.h>
#include "error.h"
#include "ui.h"

/*
	creates the menu on the screen of all
	the dir elements and displays them 
*/
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

int parse_cmd (SCR *scr, const char *cmd)
{
	bool valid = false;
	size_t src_len = strlen(cmd);
	char *cmd_name = malloc(KB);
	char *argv[src_len];

	for (size_t i = 0; i < src_len; i++)
	{
		if (cmd[i] != WHITESPACE) strcat(cmd_name, &cmd[i]);
	}
	
	free(cmd_name);
	return 0;
}

void cmd_mode (SCR *scr)
{
	char *cmd = malloc(KB);
	
	/* drawing command mode : */
	mvwprintw(scr->win, scr->max_y - 2, 1, ":");
	wrefresh(scr->win);

	curs_set(1);
	echo();
	
	wscanw(scr->win, cmd);

	/* send input off to be parsed */
	if (parse_cmd(scr, cmd) != 0) printce(scr, "No such command!");

	/* reset options */
	curs_set(0);
	noecho();
	free(cmd);
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

/* sets up the curses interface */
int ui_setup (SCR *scr)
{
	if (initscr() == NULL) return 1;

	/* creating the window */	
	getmaxyx(stdscr, scr->max_y, scr->max_x);	
	scr->win = newwin(scr->max_y, scr->max_x, 0, 0);
	
	/* generic settings for curses */
	curs_set(0);
	noecho();
	cbreak();
	refresh();

	/* making the menu */
	create_menu(scr);
	return 0;	
}

void ui_clean (SCR *scr)
{
	for (int i = 0; scr->items[i] != NULL; i++)
		free_item(scr->items[i]);

	unpost_menu(scr->men);
	free_menu(scr->men);
	endwin();
	free(scr);
}
