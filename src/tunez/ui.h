#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <curses.h>
#include <menu.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define WHITESPACESTR " "
#define WHITESPACE ' '
#define TENWSP "          "
#define MARK "-> "
#define START_Y 0
#define START_X 0
#define DEFAULT_DIR "."
#define KB 1024

/* keys */
#define ENTER 10

typedef struct {
	WINDOW *win;
	MENU *men;
	ITEM **items;
	ITEM *cur_item;
	int n_items;
	size_t max_y, max_x;
	size_t cur_y, cur_x;
	size_t fns_max;
	size_t fns_type[KB];
	char *cur_dir;
	/* TODO make this resizable */
	char *fns[KB]; 
}SCR;

void mov_down(SCR *scr);
void mov_up(SCR *scr);
void cmd_mode(SCR *scr);
void goto_top(SCR *scr);
void goto_bot(SCR *scr);

enum format_types { wav = 0, mp3 = 1 };

struct FORMATS {
	const char *ff;
	enum format_types ft;
};

struct FORMATS formats[] = {
	{ "wav", 0 },
};

struct KEY_MAP {
	int ch;
	void (*fn)(SCR *scr);
};

struct KEY_MAP key_map[] = {
	{ 'j', mov_down }, 
	{ 'k', mov_up },
	{ ':', cmd_mode },
	{ 'g', goto_top },
	{ 'G', goto_bot },
};

#include "ui.c"
