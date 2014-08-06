#ifndef BIRDIE_FUNCS_INCLUDE
#define BIRDIE_FUNCS_INCLUDE


#include <stdio.h>
#include <stdlib.h>

#include "birdie_types.h"

struct val_struct_t *reduceExpression2(struct val_struct_t *a, struct val_struct_t *b, val_operation_2 op);

struct val_struct_t *reduceExpression1(struct val_struct_t *a, val_operation_1 op);

struct val_struct_t *valInvert(struct val_struct_t *in);

int isTrueVal(struct val_struct_t *input);

void printVal(struct val_struct_t *a);

void fullReport();

#endif //BIRDIE_FUNCS_INCLUDE
