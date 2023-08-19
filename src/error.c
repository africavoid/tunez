#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void printe (const char *str)
{
	fprintf(stderr, "ERROR: %s\n", str);
}

void help (void)
{
	fprintf(stdout, "USAGE:\n");
	fprintf(stdout, "-h [help function]\n");
	fprintf(stdout, "-p [playback function]\n");
	fprintf(stdout, "-c [Enter command mode]\n");
}
