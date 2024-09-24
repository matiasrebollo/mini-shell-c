#ifndef PARSING_H
#define PARSING_H

#include "defs.h"
#include "types.h"
#include "createcmd.h"
#include "utils.h"
#include "runcmd.h"


struct cmd *parse_line(char *b);

#endif  // PARSING_H
