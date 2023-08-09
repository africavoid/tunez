#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <curses.h>
#include <menu.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define START_Y 0
#define START_X 0
#define DEFAULT_DIR "."

typedef struct {
	WINDOW *win;
	MENU *men;
	ITEM **items;
	ITEM *cur_item;
	int n_items;
	size_t max_y, max_x;
	size_t cur_y, cur_x;
	size_t fns_max;
	char *cur_dir;
	/* TODO make this resizable */
	char *fns[1024]; 
}SCR;
