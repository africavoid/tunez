#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "cmds.h"
#include "parse.h"

int list_files (char *args)
{
	DIR *dp;
	char *dir = malloc(1024);
	struct dirent *entry;

	dir = get_second_word(args);

	if (dir == NULL) dir = strndup(".", 1);

	dp = opendir(dir);

	if (dp == NULL) 
	{
		fprintf(stderr, "Cannot open dir %s\n", dir);
		return 1;
	}
	
	while ((entry = readdir(dp)) != NULL)
		fprintf(stdout, "%s\n", entry->d_name);

	closedir(dp);

	memset(dir, 0, strlen(dir));
	free(dir);
	return 0;
}
