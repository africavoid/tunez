#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include <stdlib.h>

#include "cmds.h"
#include "parse.h"
#include "cli.h"

void cmd_line_entry(char *args);

#include "cli.c"
#endif // CLI_H
