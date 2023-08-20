#include <curses.h>
#include <menu.h>
#include <dirent.h>

#include "interface.h"
#include "error.h"
#include "playback.h"

static void curses_clean(SCR *scr);

static int curses_scan_dir (SCR *scr, const char *dn)
{
	DIR *dp = opendir(dn);
	struct dirent *entry;
	
	if (dp == NULL)
	{
		printe("Cannot open dir\nExiting...");
		return 1;
	}

	/* seting file names */
	for (size_t i = 0; (entry = readdir(dp)) != NULL; i++)
	{
		scr->fns[i] = strndup(entry->d_name, strlen(entry->d_name));
		scr->fns_max = i;
	}

	closedir(dp);
	return 0;
}

void printce (SCR *scr, const char *errstr)
{
	mvwprintw(scr->win, scr->max_y - 2, 1, "%s", errstr);
	wrefresh(scr->win);
	curs_set(0);	
	echo();
} 

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

/* moves down menu */
void mov_down (SCR *scr)
{
	if (scr->cur_y < scr->fns_max)
	{
		scr->cur_y++;
		menu_driver(scr->men, REQ_DOWN_ITEM);
	}
}

/* moves up menu */
void mov_up (SCR *scr)
{
	scr->cur_y--;
	menu_driver(scr->men, REQ_UP_ITEM);
}

/* parses string and calls function if it's a valid cmd */
int parse_cmd (SCR *scr, const char *cmd)
{
	bool valid = false;
	size_t src_len = strlen(cmd);
	size_t i = 0, j = 0, k = 0;
	char *argv[src_len];
	char *tmpstr = malloc(sizeof(cmd));

	/*while (i < src_len)
	{
		if (cmd[i] != WHITESPACE)
		{
			strcat(tmpstr[j], cmd[i]);
			i++;
		}
		else
		{
			j++;
		}
	}	

	for (; cmd_kw[k].str != NULL; k++)
	{
		if (strcmp(cmd_kw[k].str, cmd) == 0)
		{
			valid = true;
			break;
		}
	}

	if (valid == true) cmd_kw[k].fn(scr, argv);
	else return 1;*/
	free(tmpstr);
	return 0;
}

/* enters the cmd line */
void cmd_mode (SCR *scr)
{
	char *cmd = malloc(KB);

	if (nodelay(scr->win, 0) == ERR) printce(scr, "no delay");

	/* drawing command mode : */
	mvwprintw(scr->win, scr->max_y - 2, 1, ":");
	wrefresh(scr->win);

	curs_set(1);
	echo();
	
	wgetstr(scr->win, cmd);

	/* send input off to be parsed */
	if (parse_cmd(scr, cmd) != 0) printce(scr, "No such command!");

	/* reset options */
	if (nodelay(scr->win, 1) == ERR) printce(scr, "no delay");
	curs_set(0);
	noecho();
	free(cmd);
}

/* moves to top of menu */
static void goto_top (SCR *scr)
{
	scr->cur_y = 0;
	menu_driver(scr->men, REQ_FIRST_ITEM);
}

/* moves to bottom of menu */
static void goto_bot (SCR *scr)
{
	scr->cur_y = scr->fns_max;
	menu_driver(scr->men, REQ_LAST_ITEM);
}

static void curses_input_parser (SCR *scr, int ch)
{
	for (size_t i = 0; key_map[i].ch != (char) 0; i++)
		if (key_map[i].ch == ch) key_map[i].fn(scr);
}

static void curses_input_loop (SCR *scr)
{
	int ch;

	while ((ch = wgetch(scr->win)) != 'q')
		curses_input_parser(scr, ch);
}

/* sets up the curses interface */
int curses_setup (void)
{
	SCR *scr = malloc(sizeof(*scr));

	if (curses_scan_dir(scr, ".") != 0)
		return 1;

	if (scr == NULL)
	{
		printe("malloc (scr) curses_setup");
		return 1;
	}

	if (initscr() == NULL) return 1;

	/* creating the window */	
	getmaxyx(stdscr, scr->max_y, scr->max_x);	
	scr->win = newwin(scr->max_y, scr->max_x, 0, 0);
	
	/* generic settings for curses */
	if (nodelay(scr->win, 1) == ERR) printce(scr, "no delay");

	curs_set(0);
	noecho();
	cbreak();
	refresh();

	/* making the menu */
	create_menu(scr);
	curses_input_loop(scr);
	curses_clean(scr);
	return 0;	
}

static void curses_clean (SCR *scr)
{
	for (int i = 0; scr->items[i] != NULL; i++)
		free_item(scr->items[i]);

	unpost_menu(scr->men);
	free_menu(scr->men);
	endwin();
	free(scr);
}
