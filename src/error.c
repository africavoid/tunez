#include <stdio.h>
#include <stdlib.h>

#include "error.h"

void printe (const char *str)
{
	fprintf(stderr, "ERROR: %s\n", str);
}
