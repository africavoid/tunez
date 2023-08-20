#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "cmds.h"
#include "parse.h"

void list_files (char *arg)
{
	DIR *dp;
	char *dir = malloc(1024);
	struct dirent *entry;

	dir = get_second_word(arg);

	if (dir == NULL) dir = strndup(".", 1);

	dp = opendir(dir);

	if (dp == NULL) 
	{
		fprintf(stderr, "Cannot open dir %s\n", dir);
		return;
	}
	
	while ((entry = readdir(dp)) != NULL)
		fprintf(stdout, "%s\n", entry->d_name);

	closedir(dp);

	memset(dir, 0, strlen(dir));
	free(dir);
}
