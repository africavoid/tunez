#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <curses.h>
#include <menu.h>
#include "playback.h"
#include "ui.h"
#include "error.h"

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


static int global_setup (const char **argv)
{
	SCR *scr = malloc(sizeof(*scr) + KB);
		
	if (scr == NULL) return printe(ALLOC_ERROR);

	if (scan_dir(scr) != 0) return printe(DIR_ERROR);

	if (ui_setup(scr) != 0) return printe(CURSES_ERROR);

	input_loop(scr);
	ui_clean(scr);

	return 0;
}

int main (int argc, const char **argv)
{
	return (global_setup(argv) == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
