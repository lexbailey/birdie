#include <stdio.h>
#include "birdie_opts.h"

const int ARGS_FOR_BIRDIE = 0;
const int ARGS_FOR_PROGRAM = 1;

void initOptionsStruct(struct birdieOptions *opts){
	opts->debugLevel = -1;
	opts->inputFile = NULL;
	opts->version = 0;
}

void checkArgNotEmpty(char *arg, char *text){
	if (text[0] == '\0') {
		fprintf(stderr, "Argument requires operand '%s'\n", arg);
		birdie_exit(EXIT_INVALID_OPTIONS);
	}
}

int signedIntArg(char **arg){
	char *end;
	int result = strtol(*arg, &end, 10);
	*arg = end;
	return result;
}

struct birdieOptions parseOptions(int argc, char **argv){
	struct birdieOptions options;
	initOptionsStruct(&options);
	int argsFor = ARGS_FOR_BIRDIE;
	int i;
	//Loop over interesting args (arg 0 is program name)
	for (i = 1; i <= argc -1; i++){
		char *thisArg = argv[i];
		if (thisArg[0] == '-'){
			if (thisArg[1] == '\0'){ fprintf(stderr, "Error: Invalid argument '-'\n"); birdie_exit(EXIT_INVALID_OPTIONS);}
			if (thisArg[1] == '-'){
				if (thisArg[2] == '\0'){
					// argument is '--'
					// Any args after this are program arguments
					argsFor = ARGS_FOR_PROGRAM;
					continue;
				}

				//Long args here

				fprintf(stderr, "Error: Birdie currently has no long arguments.\n");
				birdie_exit(EXIT_INVALID_OPTIONS);
			}
			char *thisArgText = thisArg + 1;
			while (*thisArgText != '\0'){
				if (argsFor == ARGS_FOR_BIRDIE){
					// This is a birdie single-char argument
					switch(*thisArgText){
						case 'v':

							options.version = 1;
							break;
						case 'd':
							checkArgNotEmpty("d", thisArgText+1);
							thisArgText +=1;
							options.debugLevel = signedIntArg(&thisArgText);
							thisArgText--;
							break;
						default:
							fprintf(stderr, "Error: Unrecognised argument '%c'\n", *thisArgText);
							birdie_exit(EXIT_INVALID_OPTIONS);
							break;
					}
				}
				thisArgText ++;
			}
		}
		else{
			// Must be a file name for the program we want to run.
			if (options.inputFile != NULL){
				fprintf(stderr, "Error: Program file specified twice. First as '%s' and then again as '%s'\n", options.inputFile, thisArg);
				birdie_exit(EXIT_INVALID_OPTIONS);
			}
			options.inputFile = thisArg;
		}
	}
	return options;
}
