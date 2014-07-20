#ifndef BIRDIE_STACKMAN_INCLUDE
#define BIRDIE_STACKMAN_INCLUDE

#include "birdie_types.h"

/*!

	\file birdie_stackman.h
	\brief Birdie stack manager
	\author Daniel Bailey
	\copyright Copyright Daniel Bailey 2014
	
	This it the stack manager. It keeps track of the stacks. All of the stacks are located in one large stack called "The Almighty Stack".
	The stack manager contains functions for pushing and popping all of the stacks and for reading values from them when needed.
 
*/
 
///The almighty stack stack, a stack for all other stacks. The be all and end all of the stack world.
struct almighty_stack_item_t *almightyStackStack;
///A pointer to the current almighty stack.
struct almighty_stack_item_t *theAlmightyStack;

void initialiseAlmightyStack();

void pushStackStack();
void popStackStack();

void push(struct val_struct_t *newItem);
struct val_struct_t *pop();

void pushCondition(struct val_struct_t *newCondition);
struct val_struct_t *popCondition();

void pushAlmightyStackStack();
void popAlmightyStackStack();

struct val_struct_t *topOfStack();
struct val_struct_t *topOfStackStack();
struct val_struct_t *topOfConditionStack();
struct stack_state_item_t *topOfStackStateStack();
struct val_struct_t *topOfAlmightyStack();//Don't even think about calling this function! Or you will be eaten by this unfriendly smiley...  >:|

#endif //BIRDIE_STACKMAN_INCLUDE
