#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "keywords.h"
#include "interface.h"
#include "parse.h"
#include "playback.h"
#include "error.h"
#include "audio.h"
#include "cmds.h"

const char *valid_ex[] = {
	"wav",
	"mp3",
};

/* makes sure file is valid */
static bool is_file (const char *fn)
{
	FILE *fp;
	
	if ((fp = fopen(fn, "r")) == NULL)
		return false;

	fclose(fp);
	return true;
}

/* sees if flag is a valid flag in keywords.h */
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

/* decides if arguments provided is valid */
static int check_arg (char *arg)
{
	char *ex = malloc(sizeof(arg));

	if (is_file(arg) != true)
	{
		printe("No such file or directory");
		return 1;
	}

	if (ex == NULL)
	{
		printe("malloc (ex) check_arg");
		return 1;
	}

	if (arg == NULL)
	{
		free(ex);
		printe("No arg supplied");
		return 1;
	}

	ex = get_extension(arg);

	if (ex == NULL)
	{
		printe("Invalid file type (0)");
		return 1;
	}

	/* is the file extension supported? */
	for (int i = 0; valid_ex[i] != NULL; i++)
	{
		if (strcmp(valid_ex[i], ex) == 0)
		{
			free(ex);
			return 0;
		}
	}

	printe("Invalid file type (1)");
	free(ex);
	return 1;
}

static int global_setup (int argc, char *argv[])
{
	signed int valid_flag = -1;
	char *flag = malloc(sizeof(argv[1]));
	char *arg = malloc(sizeof(argv[2]));

	/* sets up curses if no args */
	if (argc < 2) return (curses_setup() == 0) ? 0 : 1;

	flag = strndup(argv[1], strlen(argv[1]));
	arg = strndup(argv[2], strlen(argv[2]));

	if (check_arg(arg) != 0) return 1;
	
	/* sees if flag is valid -2 is the help function */
	if ((valid_flag = is_valid_arg(flag)) == -1)
	{
		printe("Invalid arg\nsee -h");
		free(flag);
		free(arg);
		return 1;
	} else if (valid_flag == -2) return 0;

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
