#pragma once

#include <stdio.h>

#define ALLOC_ERROR 0
#define DIR_ERROR 1
#define CURSES_ERROR 2

const char *em[] = {
	"Couldn't allocate memory",
	"Couldn't read directory",
	"Couldn't init curses",
};

int printe (int ec);

#include "error.c"
