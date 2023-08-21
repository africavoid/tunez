#include <string.h>

#include "parse.h"
#include "error.h"

/* This file contains functions used for parsing strings */

// TODO get this to work
// as get_first_word get_second_word is
// horribly ineffcient
char *get_word (const char *str, size_t index)
{
	size_t len = strlen(str);
	char *word = malloc(sizeof(str));

	if (word == NULL)
	{
		printe("malloc (tmpstr) get_word");
		return NULL;
	}

	if (len == 0) return NULL;

	/* k to represent word count, i for char count */
	for (size_t i = 0, k = 0; k < index && str[i] != (char) 0;)
	{
		if (str[i] != ' ' && k < index) k++;
		else if (str[i] != ' ' && k == index)
		{
			for (; str[i] != ' ' && str[i] != (char) 0; i++)
				strncat(word, &str[i], 1);
		}else i++;
	}

	return word;
}

char *get_first_word (const char *str)
{
	char *nstr = malloc(1024);
	size_t len = strlen(str);

	if (len == 0) return NULL;

	for (size_t i = 0; i < len; i++)
	{
		if (str[i] == ' ') break;
		strncat(nstr, &str[i], 1);
	}

	return nstr;
}

char *get_second_word (const char *str)
{
	char *nstr = malloc(1024);
	size_t len = strlen(str);
	size_t i = 0;
	bool is_space = false;

	if (len == 0) return NULL;

	for (; i < len; i++)
	{
		if (str[i] == ' ') 
		{
			is_space = true;
			break;
		}
	}

	if (is_space == false) return NULL;

	i++;

	for (; i < len; i++)
	{
		if (str[i] == ' ') break;
		strncat(nstr, &str[i], 1);
	}

	return nstr;
}

char *get_third_word (const char *str)
{
	char *nstr = malloc(1024);
	size_t len = strlen(str);
	size_t i = 0;
	bool is_space = false;

	if (len == 0) return NULL;

	for (; i < len; i++)
	{
		if (str[i] == ' ') 
		{
			is_space = true;
			break;
		}
	}

	if (is_space == false) return NULL;

	i++;
	is_space = false;

	for (; i < len; i++)
	{
		if (str[i] == ' ') 
		{
			is_space = true;
			break;
		}
	}

	if (is_space == false) return NULL;
	i++;

	for (; i < len; i++)
	{
		if (str[i] == ' ') break;
		strncat(nstr, &str[i], 1);
	}

	return nstr;
}


char *mono_str (char *strarr[])
{
	char *mstr = malloc(sizeof(*strarr));

	for (size_t i = 0; strarr[i] != NULL; i++)
	{
		strncat(mstr, strarr[i], strlen(strarr[i]));
		strncat(mstr, " ", strlen(strarr[i]));
	}

	return mstr;
}

/* gets file type from string */
static char *get_file_type (char *str)
{
	char *ex = malloc(sizeof(str));
	size_t len = strlen(str), dot_index = 0;

	if (ex == NULL) 
	{
		printe("malloc (ex) get_file_type");
		return NULL;
	}

	for (int i = 0; str[i] != (char) 0; i++)
		if (str[i] == '.')
		{
			dot_index = i;
			break;
		}

	for (size_t j = dot_index; str[j] != (char) 0; j++)
		strncat(ex, &str[j], 1);

	return ex;
}
