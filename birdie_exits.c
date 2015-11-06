#include "birdie_exits.h"

void birdie_exit(int returnCode){
	EXPAND(FUNC_TRACE);
	EXPAND(FUNDAMENTAL)("Exit birdie with status %d", returnCode);
	exit(returnCode);
}
