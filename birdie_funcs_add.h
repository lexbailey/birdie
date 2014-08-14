#ifndef BIRDIE_FUNCS_ADD_INCLUDE
#define BIRDIE_FUNCS_ADD_INCLUDE

#include "birdie_types.h"

#include <stdio.h>
#include <stdlib.h>

struct val_struct_t *intAddVal(int64_t a, struct val_struct_t *b);

struct val_struct_t *floatAddVal(double a, struct val_struct_t *b);

struct val_struct_t *stringAddVal(char *a, struct val_struct_t *b);

struct val_struct_t *valAdd(struct val_struct_t *a, struct val_struct_t *b);

struct val_struct_t *listAddVal(struct val_struct_t *a, struct val_struct_t *b);

#endif //BIRDIE_FUNCS_ADD_INCLUDE
