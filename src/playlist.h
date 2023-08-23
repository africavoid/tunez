#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>

#include "parse.h"
#include "error.h"
#include "controller.h"
#include "playlist.h"

void playlist_entry(const char *dn, bool shuffle);

#include "playlist.c"
#endif // PLAYLIST_H
