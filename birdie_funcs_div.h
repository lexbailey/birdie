#ifndef BIRDIE_FUNCS_DIV_INCLUDE
#define BIRDIE_FUNCS_DIV_INCLUDE

#include "birdie_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct val_struct_t *intDivVal(int64_t a, struct val_struct_t *b);

struct val_struct_t *floatDivVal(double a, struct val_struct_t *b);

struct val_struct_t *stringDivVal(char *a, struct val_struct_t *b);

struct val_struct_t *valDiv(struct val_struct_t *a, struct val_struct_t *b);

struct val_struct_t *listDivVal(struct val_struct_t *a, struct val_struct_t *b);

#endif //BIRDIE_FUNCS_DIV_INCLUDE
