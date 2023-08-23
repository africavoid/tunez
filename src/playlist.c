#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

#include "parse.h"
#include "error.h"
#include "controller.h"
#include "playlist.h"

char *path;

typedef struct {
	char *fn[1024];
	size_t ft[1024];
	size_t fn_max;
}DIRDAT;

DIRDAT *dir;

/* checks if file type is supported */
static size_t playlist_check_extension (char *ex)
{
	if (strncmp(ex, ".wav", 4) == 0)
		return 1;

	if (strncmp(ex, ".mp3", 4) == 0)
		return 2;

	return 0;
}

static int get_valid_files (const char *dn)
{
	DIR *dp;
	struct dirent *entry;
	char *ex = malloc(sizeof(dn));
	char *tmp = malloc(sizeof(dn));

	path = strndup(dn, strlen(dn));

	dp = opendir(dn);

	if (dp == NULL) return 1;

	dir->fn_max = 0;

	for (size_t i = 0; (entry = readdir(dp)) != NULL;)
	{
		ex = strndup(get_file_type(entry->d_name), strlen(entry->d_name));
	
		if (playlist_check_extension(ex) != 0)
		{
			strncat(tmp, path, strlen(path));
			strncat(tmp, entry->d_name, strlen(entry->d_name));

			dir->fn[i] = strndup(tmp, strlen(tmp));
			dir->ft[i] = playlist_check_extension(ex);
			dir->fn_max++;

			memset(tmp, 0, strlen(tmp));
			i++;
		}
	}

	closedir(dp);
	free(tmp);
	free(ex);
	return 0;
}

void playlist_start (bool shuffle)
{
	for (size_t i = 0; i < dir->fn_max; i++)
	{
		switch (dir->ft[i])
		{
			case 1:
				wav_playback_entry(dir->fn[i]);
				break;
			case 2:
				mp3_playback_entry(dir->fn[i]);
				break;
			default:
				break;
		}
	}
}

void playlist_entry (const char *dn, bool shuffle)
{
	dir = malloc(sizeof(*dir));
	path = malloc(sizeof(dn));

	if (dir == NULL)
	{
		printe("malloc (dir) playlist_entry");
		return;
	}

	if (get_valid_files(dn) != 0)
	{
		printe("No such directory");
		return; 
	}

	playlist_start(shuffle);
	free(path);
}
