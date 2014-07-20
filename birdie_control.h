#ifndef BIRDIE_CONTROL_INCLUDE
#define BIRDIE_CONTROL_INCLUDE

#include <stdio.h>
#include <stdlib.h>

#include "birdie_types.h"
#include "birdie_funcs.h"
#include "birdie_builtin.h"

struct val_list_item *variables;

void assign(struct val_struct_t *assignee);

void mergeAssign(struct val_struct_t *assignee, struct val_struct_t *data);

void readVar(struct val_struct_t *item);

struct val_struct_t functionCallArgs(const char *funcName, struct val_struct_t *inputs);

struct val_struct_t functionCall(const char *funcName);

#endif //BIRDIE_CONTROL_INCLUDE
