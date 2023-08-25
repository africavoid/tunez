#ifndef INTERFACE_H
#define INTERFACE_H

/* curses screen */
typedef struct {
	WINDOW *win;
	size_t max_y, max_x;
	size_t old_y, old_x;
	const char *file_name;
}SCR;

void cmd_line (void);
int ctrl_init_curses(const char *fn);
void pause_control(void);
void draw_pause(int status);

#include "interface.c"

#endif // INTERFACE_H
