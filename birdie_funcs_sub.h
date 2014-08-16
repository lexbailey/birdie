#ifndef BIRDIE_FUNCS_SUB_INCLUDE
#define BIRDIE_FUNCS_SUB_INCLUDE

#include "birdie_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct val_struct_t *intSubVal(int64_t a, struct val_struct_t *b);

struct val_struct_t *floatSubVal(double a, struct val_struct_t *b);

struct val_struct_t *stringSubVal(char *a, struct val_struct_t *b);

struct val_struct_t *valSub(struct val_struct_t *a, struct val_struct_t *b);

struct val_struct_t *listSubVal(struct val_struct_t *a, struct val_struct_t *b);

#endif //BIRDIE_FUNCS_SUB_INCLUDE
