#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>

#include "playlist.h"
#include "controller.h"
#include "parse.h"
#include "error.h"

/* supported file types */
const char *valid_ex[] = {
	".wav",
	".mp3",
};

static int scan_args (char ch);

/* flag variables */
bool background_f = false;
bool help_f = false;
bool playback_f = false;
bool playlist_f = false;
bool shuffle_f = false;
bool is_wav_f = false;
bool is_mp3_f = false;

/* file that will be played */
char *file_name;
char *dir_name;

/* where the file is in the cmd line args */
int file_index = 0;

/* simple help function */
static int help (void)
{
	fprintf(stdout, "USAGE:\n");
	fprintf(stdout, "-p [file] to play a file\n");
	fprintf(stdout, "-f [directory] to play through a dir/playlist\n");
	fprintf(stdout, "-b to run in background\n");
	fprintf(stdout, "Supported file types:\n");
	fprintf(stdout, "\twav\n\tmp3\n");
	return 0;
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


/* checks if file exists */
static bool is_file (const char *fn)
{
	FILE *fp;
	
	if ((fp = fopen(fn, "r")) == NULL)
		return false;

	fclose(fp);
	return true;
}

/* sets the directory based on cmd line args */
static int set_dir (char *name)
{
	DIR *dp;
	struct dirent *entry;

	if (name == NULL)
		name = strndup(".", 1);

	dp = opendir(name);

	if (dp == NULL)
	{
		printe("no such directory");
		return 1;
	}

	closedir(dp);

	if (sizeof(name) > sizeof(dir_name))
		dir_name = realloc(dir_name, sizeof(name));

	dir_name = strndup(name, strlen(name));
	return 0;
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

/* goes through argv[] for args */
static int parse_args (char *argv[])
{

	for (size_t i = 1; argv[i] != NULL; i++)
		for (size_t k = 0; argv[i][k] != (char) 0; k++)
			if (argv[i][0] == '-')
			{
				if (scan_args(argv[i][k]) != 0) return 1;

				else file_index++;
			}

	return 0;
}

/* scans command line args and sets bools */
static int scan_args (char ch)
{
	switch (ch)
	{
		case 'p':
			playback_f = true;
			break;
		case 'h':
			help_f = true;
			break;
		case 'b':
			background_f = true;
			break;
		case 'f':
			playlist_f = true;
			break;
		case 'S':
			shuffle_f = true;
			break;
		case '-':
			break;
		default:
			fprintf(stderr, "ERROR: No such flag %c\n", ch);
			return 1;
	}

	return 0;
}

static int flag_parser (void)
{
	if (help_f == true) return help();

	if (playlist_f == true)
		playlist_entry(dir_name, shuffle_f);

	if (is_wav_f == true)
		wav_playback_entry(file_name);

	else if (is_mp3_f == true)
		mp3_playback_entry(file_name);

	free(file_name);
	free(dir_name);
	return 0;
}

static int setup (int argc, char *argv[])
{
	file_name = malloc(sizeof(*argv));
	dir_name = malloc(sizeof(*argv));

	if (file_name == NULL)
	{
		printe("malloc (file_name) setup");
		return 1;
	}
	
	if (argc < 3) return help();

	/* going through cmd line args */
	if (parse_args(argv) != 0) return 1;

	if (playlist_f == true)
		set_dir(argv[file_index]);

	/* setting the name of the file */
	else if (set_file(argv[file_index]) != 0) return 1;

	return flag_parser();
}

int main (int argc, char *argv[])
{
	return (setup(argc, argv) == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
