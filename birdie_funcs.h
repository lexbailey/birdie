/*!

	\file birdie_funcs.h
	\brief Birdie internal function definitions
	\author Daniel Bailey
	\copyright Copyright Daniel Bailey 2014

	This file defines functions used internally for performing operations on data.

*/

#ifndef BIRDIE_FUNCS_INCLUDE
#define BIRDIE_FUNCS_INCLUDE


#include <stdio.h>
#include <stdlib.h>

#include "birdie_funcs_add.h"
#include "birdie_funcs_sub.h"
#include "birdie_funcs_mul.h"
#include "birdie_funcs_div.h"
#include "birdie_types.h"


///This function evaluates the result of binary operation acting on two inputs
struct val_struct_t *reduceExpression2(struct val_struct_t *a, struct val_struct_t *b, val_operation_2 op);

///This function evaluates the result of a unary operation acting on an input
struct val_struct_t *reduceExpression1(struct val_struct_t *a, val_operation_1 op);

///This function returns a value that is the boolean opposite of its input. For integers, 0 is the only false value. For floats, negative values are false while positives are true. For strings, please see the \ref string_boolean reference page
struct val_struct_t *valInvert(struct val_struct_t *in);

///This function negates a value. For numerical types, this is simply a subtract from 0 or change of sign. For strings, the string is reversed.
struct val_struct_t *valNegate(struct val_struct_t *in);

int isTrueVal(struct val_struct_t *input);

void printVal(struct val_struct_t *a);

void fullReport();

#endif //BIRDIE_FUNCS_INCLUDE
