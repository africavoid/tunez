#include <stdio.h>
#include <stdlib.h>

#include "keys.h"

/* will see if ch is the same as a valid key
 * if it is it will run the function corresponding to key */
void parse_keys (int ch)
{
	for (size_t i = 0; keymap[i].ch != (char) 0; i++)
		if (keymap[i].ch == ch) keymap[i].fn();
}
