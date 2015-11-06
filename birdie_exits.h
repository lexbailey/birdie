#ifndef BIRDIE_EXITS_INCLUDE
#define BIRDIE_EXITS_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug_firehose.h"

//Status codes you can easily get
#define EXIT_PROGRAM_EXCEPTION (1)
#define EXIT_INVALID_OPTIONS   (2)
#define EXIT_INVALID_SYNTAX    (3)
#define EXIT_PROGRAM_NOT_FOUND (4)

//Status codes that you shouldn't ever get. If you get one of these, there is a bug.
#define EXIT_UNKNOWN_INTERNAL_ERROR (90)
#define EXIT_EXIT_ERROR             (91)

void birdie_exit(int returnCode);

#endif // BIRDIE_EXITS_INCLUDE
