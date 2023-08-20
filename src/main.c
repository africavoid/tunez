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

/* flag variables */
bool background_f = false;
bool help_f = false;
bool playback_f = false;

/* file that will be played */
char *file_name;

/* supported file types */
const char *valid_ex[] = {
	"wav",
	"mp3",
};

static int help (void)
{
	fprintf(stdout, "USAGE:\n");
	fprintf(stdout, "-p [file] to play a file\n");
	fprintf(stdout, "-b to run in background\n");
	fprintf(stdout, "Supported file types:\n");

	for (int i = 0; valid_ex[i] != NULL; i++)
		fprintf(stdout, "%s\n", valid_ex[i]);

	return 0;
}

/* checks if file exists */
static bool is_file (const char *fn)
{
	FILE *fp;
	
	if ((fp = fopen(fn, "r")) == NULL)
		return false;

	fclose(fp);
	return true;
}

/* checks if file type is supported */
static int check_extension (char *ex)
{
	for (size_t i = 0; valid_ex[i] != NULL; i++)
		if (strncmp(valid_ex[i], ex, strlen(valid_ex[i])))
			return 0;

	return 1;
}

/* sets the file name */ 
static void set_file (char *name)
{
	char *ex = malloc(sizeof(name));

	if (ex == NULL)
	{
		printe("malloc (ex) set_file");
		return;
	}

	if (is_file(name) == false)
	{
		printe("no such file or directory");
		return;
	}

	if (check_extension(ex) != 0)
	{
		printe("invalid file type");
		return;
	}

	if (name == NULL)
	{
		printe("No file supplied!\n");
		return;
	}

	if (sizeof(name) > sizeof(file_name))
		file_name = realloc(file_name, sizeof(name));

	file_name = strndup(name, strlen(name));
	free(ex);
}

/* scans command line args and sets bools */
static int scan_args (char *argv[])
{
	for (size_t i = 0; argv[i] != NULL; i++)
		if (argv[i][0] == '-' && argv[i][2] == (char) 0)
			switch (argv[i][1])
			{
				case 'p':
					playback_f = true;
					set_file(argv[i+1]);
					break;
				case 'h':
					help_f = true;
					break;
				case 'b':
					background_f = true;
					break;
				default:
					fprintf(stderr, "ERROR: No such arg %s\n", argv[i]);
					return 1;
					break;
			}
		else if (argv[i][0] == '-' && argv[i][2] != (char) 0)
		{
			fprintf(stderr, "ERROR: No such arg %s\n", argv[i]);
			return 1;
		}

	return 0;
}

static int setup (int argc, char *argv[])
{
	file_name = malloc(1024);

	if (file_name == NULL)
	{
		printe("malloc (file_name) setup");
		return 1;
	}

	if (argc < 2) return curses_setup();

	if (scan_args(argv) != 0) return 1;

	if (help_f == true) return help();

	if (playback_f == true)
		playback_entry(file_name);

	free(file_name);
	return 0;
}

int main (int argc, char *argv[])
{
	return (setup(argc, argv) == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
