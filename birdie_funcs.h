#ifndef BIRDIE_FUNCS_INCLUDE
#define BIRDIE_FUNCS_INCLUDE

#include "birdie_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *newString(char *);

struct val_struct_t reduceExpression2(struct val_struct_t a, struct val_struct_t b, val_operation_2 op);

struct val_struct_t reduceExpression1(struct val_struct_t a, val_operation_1 op);

void printVal(struct val_struct_t a);

#endif //BIRDIE_FUNCS_INCLUDE
