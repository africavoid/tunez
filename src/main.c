#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "controller.h"
#include "parse.h"
#include "error.h"

/* flag variables */
bool background_f = false;
bool help_f = false;
bool playback_f = false;
bool is_wav_f = false;
bool is_mp3_f = false;

/* used to represent no flags 
 * if args are supplied but no flags
 * the program will fail */
bool NULL_f = true;

/* file that will be played */
char *file_name;

/* supported file types */
const char *valid_ex[] = {
	".wav",
	".mp3",
};

static int help (void)
{
	fprintf(stdout, "USAGE:\n");
	fprintf(stdout, "-p [file] to play a file\n");
	fprintf(stdout, "-b to run in background\n");
	fprintf(stdout, "Supported file types:\n");
	fprintf(stdout, "\twav\n\tmp3\n");
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
		if (strncmp(valid_ex[i], ex, strlen(valid_ex[i])) == 0)
		{
			if (strncmp(valid_ex[i], ".mp3", strlen(valid_ex[i])) == 0) is_mp3_f = true;
			else if (strncmp(valid_ex[i], ".wav", strlen(valid_ex[i])) == 0) is_wav_f = true;
			return 0;
		}

	return 1;
}

/* sets the file name */ 
static int set_file (char *name)
{
	char *ex = malloc(sizeof(name));

	if (name == NULL)
	{
		printe("No file supplied!");
		return 1;
	}

	if (ex == NULL)
	{
		printe("malloc (ex) set_file");
		return 1;
	}

	/* making sure file exists */
	if (is_file(name) == false)
	{
		printe("no such file or directory");
		return 1;
	}

	/* getting the file extension */
	if ((ex = get_file_type(name)) == NULL)
	{
		printe("invalid file type");
		return 1;
	}

	/* checks extension is valid */
	if (check_extension(ex) != 0)
	{
		printe("invalid file type");
		return 1;
	}

	if (sizeof(name) > sizeof(file_name))
		file_name = realloc(file_name, sizeof(name));

	file_name = strndup(name, strlen(name));
	free(ex);
	return 0;
}

/* scans command line args and sets bools */
static int scan_args (char *argv[])
{
	int file_index = 0;

	/* scanning args */
	for (size_t i = 0; argv[i] != NULL; i++)
		if (argv[i][0] == '-' && argv[i][2] == (char) 0)
			switch (argv[i][1])
			{
				case 'p':
					playback_f = true;
					file_index = i+1;
					NULL_f = false;
					break;
				case 'h':
					help_f = true;
					NULL_f = false;
					break;
				case 'b':
					background_f = true;
					NULL_f = false;
					break;
				default:
					fprintf(stderr, "ERROR: No such arg %s\n", argv[i]);
					return 1;
			}
		else if (argv[i][0] == '-' && argv[i][2] != (char) 0)
		{
			fprintf(stderr, "ERROR: No such arg %s\n", argv[i]);
			return 1;
		}

	/* setting the file name */
	if (set_file(argv[file_index]) != 0) 
	{
		playback_f = false;
		NULL_f = true;
		return 1;
	}

	return 0;
}

static int flag_parser (void)
{
	if (NULL_f == true)
	{
		printe("no flags supplied, see -h");
		return 1;
	}

	if (help_f == true) return help();

	if (playback_f != true) return 1;

	if (is_wav_f == true)
		wav_playback_entry(file_name);
	else if (is_mp3_f == true)
		mp3_playback_entry(file_name);

	free(file_name);
	return 0;
}

static int setup (int argc, char *argv[])
{
	file_name = malloc(sizeof(*argv));

	if (file_name == NULL)
	{
		printe("malloc (file_name) setup");
		return 1;
	}
	
	if (argc < 2) return help();

	if (scan_args(argv) != 0) return 1;

	return flag_parser();
}

int main (int argc, char *argv[])
{
	return (setup(argc, argv) == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
