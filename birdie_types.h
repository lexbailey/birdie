/*!

	\file birdie_types.h
	\brief Birdie type definitions
	\author Daniel Bailey
	\copyright Copyright Daniel Bailey 2014
	
	This file defines various types used by other parts of the birdie code. It also defines some functions to manipulate these types.
 
*/

#ifndef BIRDIE_TYPE_INCLUDE
#define BIRDIE_TYPE_INCLUDE

#include <stdio.h>
#include <stdlib.h>

///Enumerated type for the storage type of a variable
typedef enum {vtString, vtInt, vtFloat, vtIdentifier} val_type_t;

///Enumerated type for a binary operator
typedef enum {voAdd, voSubtract, voMultiply, voDivide, voModulus} val_operation_2;

///Enumerated type for a unary operator
typedef enum {voInvert, voAsScalar, voAsString, voAsList} val_operation_1;

//isList is true when this item is a list
//the list pointer points to a link list made up of val_list_items. Items in this list can be items
//The end of a list is only noted by the lack of a pointer to the next item in the list. (NULL pointer)

struct val_list_item;

///Link list item that holds one stack state.
struct stack_state_item_t{
	struct stack_state_item_t *nextStackState; //!< Pointer to the next item in the link list
	int stackMode, //!< Current stack mode (on or off)
	 autoPush;	//!< Current auto push mode (on or off)
};

//Link list item that holds one stack state stack. One of these is part of the stack state stack stack.
//struct stack_state_stack_item_t{
//	struct stack_state_stack_item_t *nextStackStateStack; //!< Pointer to the next item in the link list
//};

///A birdie value. This provides an abstract 'value' type. Birdie is duck typed, this struct stores a value of any type.
struct val_struct_t{
	val_type_t valueType;
	char *valName;
	char *valS;
	int64_t valI;
	double valF;
	char *valID;

	int isList;
	struct val_list_item *list;
};

struct val_list_item{
	struct val_list_item *nextItem;
	struct val_struct_t *item;
};

struct almighty_stack_item_t{
	struct almighty_stack_item_t *nextItem;
	//Here are the many magical stacks of birdie
	struct val_struct_t *stackStack; //The stack of stacks. This is a list that contains lists, each list in this list is a stack
	struct val_struct_t *theStack; //This is just to point to the stack currently at the top of the stack list.
	
	struct stack_state_item_t *stackStateStack; //Stack state stack, this stack should be the same length at the stack stack (stackStack)
	struct stack_state_item_t *theStackState; //This is just a pointer to the current state stack
	
	struct val_struct_t *conditionStackStack; //This is a stack of condition stacks
	struct val_struct_t *theConditionStack; //This is just a pointer to the current condition stack


	//So, to summarise, we have the following stacks...
	//"The stack" is the general purpose program stack. This is the stack that is currently on the top of the stack stack
	//"The stack stack" this is a stack of general purpose program stacks
	//"The stack state stack" this is a stack of stack states. Each general purpose program stack on the stack stack has a state in this stack (they are effectively part of the same stack)
	//"The condition stack" Used for executing code conditionally. Commands will only be executed if the top of the condition stack is a 'true' value of some kind
	//There is also the last stack. The stack to beat all stacks. "The almighty stack" :O ... What's that?
	//The almighty stack contains all stack items.
	
	//So, how many times can we get the word stack in to one sentance?
	
	//The almighty stack is made up of the stack stack, the stack state stack and the condition stack.
};

void freeListItem(struct val_list_item *victim);

void freeVal(struct val_struct_t*);

void freeStackStateItem(struct stack_state_item_t*);

//void freeStackStateStackItem(struct stack_state_stack_item_t*);

struct val_struct_t* copyVal(struct val_struct_t*);

struct val_list_item* copyValList(struct val_list_item *data);

void appendList(struct val_struct_t*, struct val_struct_t*);

void prependList(struct val_struct_t *existing, struct val_struct_t *newItem);

void concatLists(struct val_struct_t *listInOut, struct val_struct_t *listTwo);

void initValStruct(struct val_struct_t* val);

struct val_struct_t* createValStruct();

void initValListItem(struct val_list_item* val);

struct val_list_item* createValListItem();

void initStackStateItem(struct stack_state_item_t* val);

struct stack_state_item_t* createStackStateItem();

//void initStackStateStackItem(struct stack_state_stack_item_t*);

//struct stack_state_stack_item_t* createStackStateStackItem();

void debugVal(struct val_struct_t *val);

#endif //BIRDIE_TYPE_INCLUDE
