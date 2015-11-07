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

#include "birdie_funcs_booland.h"
#include "birdie_funcs_boolor.h"
#include "birdie_funcs_boolxor.h"

#include "birdie_funcs_bool_util.h"

#include "birdie_types.h"

///This function evaluates the result of binary operation acting on two inputs
struct val_struct_t *reduceExpression2(struct val_struct_t *a, val_operation_2 op);

///This function evaluates the result of a unary operation acting on an input
struct val_struct_t *reduceExpression1(struct val_struct_t *a, val_operation_1 op);

#endif //BIRDIE_FUNCS_INCLUDE
