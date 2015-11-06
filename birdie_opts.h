#ifndef BIRDIE_OPTS_INLUDE
#define BIRDIE_OPTS_INLUDE

#include "birdie_exits.h"

struct birdieOptions{
	int debugLevel;
	char *inputFile;
	int version;
};

void initOptionsStruct(struct birdieOptions *opts);

struct birdieOptions parseOptions(int argc, char **argv);

#endif // BIRDIE_OPTS_INLUDE
