#ifndef BIRDIE_CONTROL_INCLUDE
#define BIRDIE_CONTROL_INCLUDE

#include <stdio.h>
#include "birdie_types.h"
#include "birdie_funcs.h"

struct val_list_item *variables;

void assign(struct val_struct_t *assignee);

void mergeAssign(struct val_struct_t *assignee, struct val_struct_t *data);

void readVar(struct val_struct_t *item);

#endif //BIRDIE_CONTROL_INCLUDE
