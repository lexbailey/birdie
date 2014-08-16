#ifndef BIRDIE_FUNCS_MUL_INCLUDE
#define BIRDIE_FUNCS_MUL_INCLUDE

#include "birdie_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct val_struct_t *intMulVal(int64_t a, struct val_struct_t *b);

struct val_struct_t *floatMulVal(double a, struct val_struct_t *b);

struct val_struct_t *stringMulVal(char *a, struct val_struct_t *b);

struct val_struct_t *valMul(struct val_struct_t *a, struct val_struct_t *b);

struct val_struct_t *listMulVal(struct val_struct_t *a, struct val_struct_t *b);

#endif //BIRDIE_FUNCS_MUL_INCLUDE
