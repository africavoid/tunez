#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "keywords.h"
#include "parse.h"
#include "playback.h"
#include "error.h"
#include "audio.h"
#include "cmds.h"

static signed int is_valid_arg (const char *flag)
{
	for (int i = 0; flag_map[i].str != NULL; i++)
		if (strcmp(flag, flag_map[i].str) == 0) return i;

	if (strcmp(flag, "-h") == 0) 
	{
		help();
		return -2;
	}

	return -1;
}

static int global_setup (int argc, char *argv[])
{
	signed int valid_flag = -1;
	char *flag = malloc(sizeof(argv[1]));
	char *arg = malloc(sizeof(argv[2]));

	/* sets up curses if no args */
	if (argc < 2)
	{
		return 0;
	}

	flag = strndup(argv[1], strlen(argv[1]));
	arg = strndup(argv[2], strlen(argv[2]));
	
	/* sees if flag is valid -2 is the help function */
	if ((valid_flag = is_valid_arg(flag)) == -1)
	{
		printe("Invalid arg\nsee -h");
		return 1;
	}else if (valid_flag == -2) return 0;

	// TEMP
	printf("%s\n", argv[1]);
	printf("%s\n", argv[2]);

	/* execute command */
	flag_map[valid_flag].fn(arg);
	free(arg);
	free(flag);
	return 0;
}

int main (int argc, char *argv[])
{
	return (global_setup(argc, argv) == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
