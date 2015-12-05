/*!

	\file birdie_control.h
	\brief Controls most of the inner functions
	\author Daniel Bailey
	\copyright Copyright Daniel Bailey 2014

	This code keeps track of variables and performs function calls.

*/

#ifndef BIRDIE_CONTROL_INCLUDE
#define BIRDIE_CONTROL_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "birdie_types.h"
#include "birdie_funcs.h"
#include "birdie_builtin.h"
#include "birdie_token_stream.h"

//Pointer to a built in birdie function.
typedef
		struct val_struct_t* (*birdieFuncPtr_t)(struct val_struct_t *);

struct val_list_item			*variables;
struct token_stream_list_item	*functions;

/**
 * Assigns a new value to a variable and sets the special flags.
 * if any flag is set to -1, the current value is retained.
 */
void assignSpecial(struct val_struct_t *assignee, int lazy, int magic, int sticky);

/**
 * Assigns a new value to a variable
 * If the value does not exist then it is created and assigned the new value.
 * @Param assignee[in] The value to store
 */
void assign(struct val_struct_t *assignee);

/**
 * Assigns a variable using data from another variable
 * @Param assignee[in] The value to store (only the ID is relevent)
 * @Param data[in] The data that should be stored in the variable
 */
void mergeAssign(struct val_struct_t *assignee, struct val_struct_t *data);

/**
 * Looks up a value in the list of variables.
 * If the value does not exist then it is created and initialised to the integer 0.
 * @Param name[in] Name of the value to look up
 * @Return Structure containing the variable that was found in the list.
 */
struct val_struct_t *readVar(const char *, var_read_mode);

struct val_struct_t *functionCallArgs(const char *funcName, struct val_struct_t *inputs);

void freeAllVariables();

struct token_stream_token *getUserFunc(char *);

void defineFunction(char *, struct token_stream_token *);

void freeAllFunctions();

#endif //BIRDIE_CONTROL_INCLUDE
