#ifndef BIRDIE_FUNCS_BOOL_UTIL_INCLUDE
#define BIRDIE_FUNCS_BOOL_UTIL_INCLUDE

#include "birdie_types.h"

///This function returns a value that is the boolean opposite of its input. For integers, 0 is the only false value. For floats, negative values are false while positives are true. For strings, please see the \ref string_boolean reference page
struct val_struct_t *valInvert(struct val_struct_t *in);

///This function negates a value. For numerical types, this is simply a subtract from 0 or change of sign. For strings, the string is reversed.
struct val_struct_t *valNegate(struct val_struct_t *in);

///Returns 1 if the value of the input is considered to be a true value.
int isTrueVal(struct val_struct_t *input);

#endif //BIRDIE_FUNCS_BOOL_UTIL_INCLUDE
