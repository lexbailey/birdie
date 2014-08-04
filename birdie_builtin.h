#ifndef BIDIE_BUILTIN_INCLUDE
#define BIDIE_BUILTIN_INCLUDE

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "birdie_types.h"


//Printing
struct val_struct_t *print(struct val_struct_t *inputs);
//String functions
struct val_struct_t *trim(struct val_struct_t *inputs);
//Input
struct val_struct_t *magicinput(struct val_struct_t *inputs);
struct val_struct_t *strinput(struct val_struct_t *inputs);
struct val_struct_t *intinput(struct val_struct_t *inputs);
struct val_struct_t *floatinput(struct val_struct_t *inputs);
//Maths
struct val_struct_t *count(struct val_struct_t *inputs);
//Odd stuff...
struct val_struct_t *magicTypeSort(struct val_struct_t *inputs);

#endif //BIDIE_BUILTIN_INCLUDE
