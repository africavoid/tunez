#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keywords.h"
#include "playback.h"
#include "cli.h"
#include "cmds.h"

/* checks in keywords.h if string is valid
 * returns -1 if invalid
 * if valid retuns an interger to the index,
 * of the matching string */ 
signed int valid_kw(const char *str, size_t len)
{
	int i = 0;

	for (; kw[i].str != NULL; i++)
		if (strcmp(str, kw[i].str) == 0) return i;

	return -1;
}
