#ifndef INTERFACE_H
#define INTERFACE_H

/* curses screen */
typedef struct {
	WINDOW *win;
	size_t max_y, max_x;
}SCR;

void cmd_line (void);
int ctrl_init_curses(const char *fn);
void pause_control(void);

#include "interface.c"

#endif // INTERFACE_H
