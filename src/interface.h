#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdbool.h>
#include <curses.h>
#include <menu.h>
#include <dirent.h>

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

int curses_setup(void);

static void goto_top(SCR *scr);
static void goto_bot(SCR *scr);
static void mov_up(SCR *scr);
static void mov_down(SCR *scr);
static void cmd_mode(SCR *scr);

struct KEY_MAP {
	int ch;
	void (*fn)(SCR *scr);
};

struct KEY_MAP key_map[] = {
	{ 'j', mov_down }, 
	{ 'k', mov_up },
	//{ 'l', sel },
	{ ':', cmd_mode },
	{ 'g', goto_top },
	{ 'G', goto_bot },
};


#include "interface.c"
#endif // INTERFACE_H
